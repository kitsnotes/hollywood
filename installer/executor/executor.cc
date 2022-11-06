/*
 * executor.cc - Implementation of the HorizonScript executor
 * Project Horizon
 *
 * Copyright (c) 2019-2020 Adélie Linux and contributors.  All rights reserved.
 * This code is licensed under the AGPL 3.0 license, as noted in the
 * LICENSE-code file in the root directory of this repository.
 *
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#include <unistd.h>
#include "hscript/script.hh"
#include "util/output.hh"

bool pretty = false;

int main(int argc, char *argv[]) {
    const Horizon::Script *my_script;
    Horizon::ScriptOptions opts;
    int result_code = EXIT_SUCCESS;
    using Horizon::ScriptOptionFlags;

    /* Default to pretty if we are using a TTY, unless -n specified. */
    if(isatty(1) && isatty(2)) {
        pretty = true;  /* LCOV_EXCL_LINE */
    }

    opts.set(ScriptOptionFlags::InstallEnvironment);
    opts.set(ScriptOptionFlags::UseNetwork);
    opts.set(ScriptOptionFlags::StrictMode);

    std::cout << "HorizonScript Executor version " << VERSTR;
#ifdef NON_LIBRE_FIRMWARE
    colour_if_pretty(std::cout, "31");
    std::cout << " (supports non-free firmware)";
    reset_if_pretty(std::cout);
#endif
    std::cout << std::endl;
    std::cout << "Copyright (c) 2022 Originull Software."
              << std::endl;
    std::cout << "Copyright (c) 2019-2020 Adélie Linux and contributors."
              << std::endl;
    std::cout << "This software is licensed to you under the terms of the "
              << std::endl << "AGPL 3.0 license, unless otherwise noted.";
    std::cout << std::endl << std::endl;

    if(argc > 2)
    {

    }
    std::string path = "/etc/hollywood.hs";
    if(argc == 2)
    {
        std::string arg(argv[1]);
        if(arg == "--help") {
            /* Banner printed above is our version statement. */
            std::cerr << "usage: " << std::endl;
            std::cerr << "\thscript-executor [path]" << std::endl;
            return EXIT_SUCCESS;
        }
        path = arg;
    }
    my_script = Horizon::Script::load(path, opts);
    if(my_script == nullptr) {
        std::cout << "Could not load the HorizonScript:" << path << std::endl;
        return EXIT_FAILURE;
    }

    if(!my_script->execute()) {
        output_error("internal", "Script failed.  Stop.", "");
        result_code = EXIT_FAILURE;
    } else {
        output_info("internal", "Installation completed successfully.", "");
    }

    delete my_script;
    return result_code;
}
