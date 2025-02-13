// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFutureWatcher>
#include <QtCore/QSocketNotifier>
#include <QtConcurrent/QtConcurrentRun>

#include "xwayland.h"
#include "xwaylandserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <unistd.h>

#include <wayland-server.h>

class ServerProcess : public QProcess
{
public:
    ServerProcess(QObject *parent = nullptr)
        : QProcess(parent)
    {
        setChildProcessModifier([] {
            sigset_t userSignals;
            sigemptyset(&userSignals);
            sigaddset(&userSignals, SIGUSR1);
            sigaddset(&userSignals, SIGUSR2);
            pthread_sigmask(SIG_UNBLOCK, &userSignals, nullptr);
        });
    }
};


XWaylandServer::XWaylandServer(Compositor *compositor, QObject *parent)
    : QObject(parent)
    , m_compositor(compositor)
    , m_display(0)
    , m_process(nullptr)
    , m_client(nullptr)
{
    m_serverPairFd[0] = m_serverPairFd[1] = -1;
    m_wmPairFd[0] = m_wmPairFd[1] = -1;
}

XWaylandServer::~XWaylandServer()
{
    shutdown();
}

Compositor *XWaylandServer::compositor() const
{
    return m_compositor;
}

QString XWaylandServer::displayName() const
{
    return m_displayName;
}

bool XWaylandServer::start()
{
    if (::pipe(m_serverPairFd) < 0) {
        qCWarning(gLcXwayland, "Failed to create pipe for XWayland server: %s",
                  strerror(errno));
        return false;
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, m_wmPairFd) < 0) {
        qCWarning(gLcXwayland, "Failed to create socket pair for window manager: %s",
                  strerror(errno));
        return false;
    }

    int fd = ::dup(m_wmPairFd[1]);
    if (fd < 0) {
        qCWarning(gLcXwayland, "Failed to duplicate socket for window manager: %s",
                  strerror(errno));
        return false;
    }

    int sx[2];
    if (::socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, sx) < 0) {
        qCWarning(gLcXwayland, "Failed to create socket pair for XWayland client: %s",
                  strerror(errno));
        return false;
    }
    m_client = wl_client_create(m_compositor->display(), sx[0]);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("WAYLAND_SOCKET"), QString::number(::dup(sx[1])));
    env.insert(QStringLiteral("EGL_PLATFORM"), QStringLiteral("DRM"));

    m_process = new ServerProcess();
    m_process->setProcessChannelMode(QProcess::ForwardedErrorChannel);
    m_process->setProcessEnvironment(env);
    connect(m_process, (void (QProcess::*)())&QProcess::started, [this] {
        qCDebug(gLcXwayland) << "Xwayland process started";

        QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater, Qt::QueuedConnection);
        watcher->setFuture(QtConcurrent::run([this] { handleServerStarted(); }));
    });
    connect(m_process, &QProcess::errorOccurred, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart)
            Q_EMIT failedToStart();
    });
    connect(m_process, (void (QProcess::*)(int))&QProcess::finished, [this](int exitCode) {
        qCDebug(gLcXwayland) << "Xwayland finished with exit code" << exitCode;

        if (m_process) {
            delete m_process;
            m_process = nullptr;
        }
    });

    QStringList args = QStringList()
            << QStringLiteral("-displayfd") << QString::number(m_serverPairFd[1])
            << QStringLiteral("-rootless")
            << QStringLiteral("-wm") << QString::number(fd);
    qCDebug(gLcXwayland) << "Running:" << "Xwayland" << qPrintable(args.join(QStringLiteral(" ")));
    m_process->start(QStringLiteral("Xwayland"), args);

    ::close(m_serverPairFd[1]);

    return true;
}

void XWaylandServer::shutdown()
{
    // Terminate XWayland server
    if (m_process) {
        m_process->terminate();
        m_process->waitForFinished();

        // Kill the process only if it's still running
        if (m_process && m_process->state() == QProcess::Running)
            m_process->kill();

        m_process->deleteLater();
        m_process = nullptr;
    }
}

void XWaylandServer::handleServerStarted()
{
    QFile readPipe;
    if (!readPipe.open(m_serverPairFd[0], QFile::ReadOnly)) {
        qCWarning(gLcXwayland, "Failed to open pipe to start Xwayland: %s",
                  readPipe.errorString().toLatin1().constData());
        return;
    }

    QByteArray displayNumber = readPipe.readLine().trimmed();

    bool ok = false;
    m_display = displayNumber.toInt(&ok);
    if (!ok) {
        qCWarning(gLcXwayland, "Xwayland display read from pipe is not a number: %s",
                  displayNumber.constData());
        return;
    }

    displayNumber.prepend(QByteArray(":"));
    m_displayName = QString::fromLatin1(displayNumber);
    Q_EMIT displayNameChanged();

    qputenv("DISPLAY", m_displayName.toLatin1());

    qCInfo(gLcXwayland) << "Xwayland started on display" << m_displayName.toLatin1().constData();

    ::close(m_serverPairFd[0]);

    Q_EMIT started(m_displayName);
}
