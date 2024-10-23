#define _GNU_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include "wl-socket.h"

typedef enum {
    MODE_KDE = 0,
    MODE_ENV = 2,
} pass_mode_t;

typedef struct {
    struct wl_socket * socket;

    char ** compositor_argv;
    int compositor_argc;

    pid_t compositor_pid;
    int restart_counter;
    int max_restarts;
    pass_mode_t mode;
} ctx_t;

void init(ctx_t * ctx) {
    ctx->socket = NULL;

    ctx->compositor_argv = NULL;
    ctx->compositor_argc = 0;

    ctx->compositor_pid = -1;
    ctx->restart_counter = 0;
    ctx->max_restarts = 10;
    ctx->mode = MODE_KDE;
}

void cleanup(ctx_t * ctx) {
    // close socket
    if (ctx->socket != NULL) {
        wl_socket_destroy(ctx->socket);
        ctx->socket = NULL;
    }

    // free argv
    if (ctx->compositor_argv != NULL) {
        for (int i = 0; i < ctx->compositor_argc; i++) {
            if (ctx->compositor_argv[i] != NULL) free(ctx->compositor_argv[i]);
        }

        free(ctx->compositor_argv);
        ctx->compositor_argv = NULL;
    }

    if (ctx->compositor_pid != -1) {
        kill(ctx->compositor_pid, SIGTERM);
        ctx->compositor_pid = -1;
    }
}

void exit_fail(ctx_t * ctx) {
    cleanup(ctx);
    exit(1);
}

void create_socket(ctx_t * ctx, int argc, char ** argv) {
    ctx->socket = wl_socket_create();
    if (ctx->socket == NULL) {
        fprintf(stderr, "error: failed to create wayland socket\n");
        exit_fail(ctx);
    }

    // allocate space for two extra options with arguments for cli modes
    ctx->compositor_argc = argc + 4;
    // add 1 for null terminator
    ctx->compositor_argv = calloc(ctx->compositor_argc + 1, sizeof (char *));

    // copy compositor arguments into argv array
    for (int i = 0; i < argc; i++) {
        ctx->compositor_argv[i] = strdup(argv[i]);
    }

    // prepare environment for env modes
    const char * socket_name_var = "WAYLAND_SOCKET_NAME";
    const char * socket_fd_var = "WAYLAND_SOCKET_FD";

    // set environment vars
    setenv(socket_name_var, wl_socket_get_display_name(ctx->socket), true);

    char * socket_fd = NULL;
    if (asprintf(&socket_fd, "%d", wl_socket_get_fd(ctx->socket)) == -1) {
        fprintf(stderr, "error: failed to convert fd to string\n");
        exit_fail(ctx);
    }

    setenv(socket_fd_var, socket_fd, true);
    free(socket_fd);
}

void start_compositor(ctx_t * ctx) {
    pid_t pid = fork();
    if (pid == 0) {
        // exec into compositor process
        execvp(ctx->compositor_argv[0], ctx->compositor_argv);

        fprintf(stderr, "error: failed to start compositor\n");
        exit(1);
    } else {
        ctx->compositor_pid = pid;
    }
}

int wait_compositor(ctx_t * ctx) {
    int stat;
    while (waitpid(ctx->compositor_pid, &stat, 0) == -1) {
        if (errno == EINTR) {
            // interrupted by signal
            // try again
        } else if (errno == ECHILD) {
            // signal handler probably already waited for this child
            // try again
        } else {
            fprintf(stderr, "error: failed to wait for compositor: %s\n", strerror(errno));
            exit_fail(ctx);
        }
    }

    ctx->compositor_pid = -1;
    return stat;
}

static ctx_t * signal_ctx = NULL;
void handle_quit_signal(int signal) {
    ctx_t * ctx = signal_ctx;

    fprintf(stderr, "info: signal %s received, quitting\n", strsignal(signal));
    exit_fail(ctx);
}

void handle_restart_signal(int signal) {
    ctx_t * ctx = signal_ctx;

    if (ctx->compositor_pid != -1) {
        fprintf(stderr, "info: signal %s received, restarting compositor\n", strsignal(signal));
        kill(ctx->compositor_pid, SIGTERM);
        wait_compositor(ctx);
        start_compositor(ctx);
        fprintf(stderr, "info: compositor restarted\n");
    }
}

void register_signals(ctx_t * ctx) {
    signal_ctx = ctx;
    signal(SIGINT, handle_quit_signal);
    signal(SIGTERM, handle_quit_signal);
    signal(SIGHUP, handle_restart_signal);
}

void run(ctx_t * ctx) {
    while (ctx->restart_counter < ctx->max_restarts) {
        start_compositor(ctx);
        int status = wait_compositor(ctx);

        if (!WIFSIGNALED(status) && WEXITSTATUS(status) == 0) {
            fprintf(stderr, "info: compositor exited successfully, quitting\n");
            cleanup(ctx);
            exit(0);
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGTRAP) {
            ctx->restart_counter = 0;
            fprintf(stderr, "info: compositor exited with SIGTRAP, resetting counter\n");
        } else {
            ctx->restart_counter++;
            fprintf(stderr, "info: compositor exited with code %d, incrementing restart counter (%d)\n", status, ctx->restart_counter);
        }

        // format new restart count
        char * restart_counter_str = NULL;
        if (asprintf(&restart_counter_str, "%d", ctx->restart_counter) == -1) {
            fprintf(stderr, "error: failed to convert restart counter to string\n");
            exit_fail(ctx);
        }

        // add restart count to environment
        setenv("WL_RESTART_COUNT", restart_counter_str, true);
        free(restart_counter_str);
    }

    fprintf(stderr, "error: too many restarts, quitting\n");
    exit_fail(ctx);
}

void usage(ctx_t * ctx) {
    printf("usage: wlproxy [[options] --]\n");
    printf("\n");
    printf("compositor restart helper. restarts your compositor when it\n");
    printf("crashes and keeps the wayland socket alive.\n");
    printf("\n");
    printf("options:\n");
    printf("  -h,   --help           show this help\n");
    printf("  -n N, --max-restarts N restart a maximum of N times (default 10)\n");
    cleanup(ctx);
    exit(0);
}

int main(int argc, char ** argv) {
    if (argc > 0) {
        // skip program name
        argc--, argv++;
    }

    ctx_t ctx;
    init(&ctx);

    while (argv[0] != NULL && argv[0][0] == '-') {
        char * opt = argv[0];
        char * arg = argv[1];
        argc--, argv++;

        if (strcmp(opt, "-h") == 0 || strcmp(opt, "--help") == 0) {
            usage(&ctx);
        } else if (strcmp(opt, "-n") == 0 || strcmp(opt, "--max-restarts") == 0) {
            if (arg == NULL) {
                fprintf(stderr, "error: option '%s' needs an argument\n", opt);
                exit_fail(&ctx);
            }

            argc--, argv++;
            ctx.max_restarts = atoi(arg);
        } else if (strcmp(opt, "--") == 0) {
            break;
        } else {
            fprintf(stderr, "error: unknown option '%s', see --help\n", opt);
            exit_fail(&ctx);
        }
    }

    if (argc == 0) {
        usage(&ctx);
    } else {
        create_socket(&ctx, argc, argv);
        register_signals(&ctx);
        run(&ctx);
    }

    cleanup(&ctx);
}
