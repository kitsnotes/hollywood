# Hollywood Operating System
This is the **Hollywood** operating system a custom Wayland based freedesktop environment on a lightweight Linux distribution.

## What is this?
Hollywood is a custom freedekstop compliant desktop environment.  It is a work in progress based upon Qt6, Wayland & PipeWire to provide a world-class butter-smooth traditional desktop environment.  The design of the Hollywood desktop is based around well thought out core applications.  It is otherwise a minimal environment providing only what is *required* of an operating system.  Accessories and value-add software is outside of the scope of this project.

Hollywood is also a set of minimal packages to produce a Linux distribution.  Hollywood uses the *apk* package manager from Alpine Linux as well as a subset of Alpine's aports tree retargeted towards glibc.  Hollywood provides glibc, OpenRC init, ext4 and btrfs filesystems, core utilities, Qt5 & Qt6, GTK3 & GTK4 (coming soon), as well as most popular languages.

The Hollywood desktop is designed specifically for the Hollywood packages and vice versa.  The end result goal is an easy to use, low maintenance fully integrated Linux system ready to run your favorite applications.

This is designed for AMD64 and ARM64.  Usage on AMD64 requires UEFI firmware.

## Screenshots

**Hollywood is not yet ready for regular usage!** It has bugs and still requires more work.  Here's some screenshots of current progress:

![Screenshot of a Hollywood desktop](screenshots/nov05-2022-desktop.png "Hollywood Desktop")
> *A Hollywood desktop as of Nov 05 2022 containing layer-shell/xdg-popups, new icon support, etc.*

![Screenshot of the About Hollywood application showing system specifications](screenshots/about.jpg "About Hollywood")
> *About Hollywood, showing system specifications.*

![Hollywood Desktop showing System Settings](screenshots/desktop-with-settings.png "Desktop with System Settings")
> *Hollywood Desktop, featuring System Settings.*

![System Settings in Dark Mode](screenshots/dark-system-settings.png "Dark Mode System Settings")
> *System Settings featured in dark mode.*

![System Settings Network Applet](screenshots/network.png "System Settings Network Applet")
> *The network applet in System Settings.*
 
![System Monitor](screenshots/sysmon.png "System Monitor")
> *System Monitor utility showing CPU usage graph.*

![polkit authentication agent](screenshots/polkit.png "System Monitor Polkit Auth")
> *System monitor utility controlling foreign owned process via polkit authentication agent.*

![Dual Screen Terminal](screenshots/dual-screen-term.jpg "Dual Screen Terminull")
> *Beginning of multiple-screen support with spanning Terminull window.*

## Licensing

Hollywood is free software.  The projects within the desktop/ folder are licensed various versions of (L)GPL 2 and 3.  Please see the individual projects and code headers for more information.

## About & Thanks

This is my vision for the Linux desktop.  While I have wrote a lot of this code, I have not composed it all!

Much thanks needs to be given to

* [Adélie Linux](https://www.adelielinux.org/) for the installer.
* [Alpine Linux](https://alpinelinux.org) for the base package build scripts.
* [Arch Linux](https://archlinux.org) for serving as an excellent reference during the glibc transition.
* [KDE Plasma](https://kde.org) for a lot of re-used code.
* [Liri OS](https://github.com/lirios) for the base of our Qt EGLFS plugin and other Qt/Wayland reference bits.
* [LXQt](https://lxqt.org) for a lot more re-used code.
* [postmarketOS](https://postmarketos.org/) for the ApkQt package library.
* [cmst](https://github.com/andrew-bibb/cmst) for the basis of network settings.
* The Linux Kernel Developers
* Many, many more.
