# The Cat Presents: Arion

This is the Arion Desktop Environment.  This is an **experimental** XDG compliant desktop
for the Arion Operating System.  This code is not designed to operate on GNU/Linux and adoptions for GNU/Linux (systemd, etc) are outside the scope of this project.

This code *should* build on Alpine or Adiele Linux, however, that is also outside of scope.

## Requirements
This will grow as the project grows.  Our goal is to build a desktop with Stock Qt and minimize dependencies.   As a result, we are **not** using any KDE Frameworks but this may be subject to change.

* Qt 6.2 (Required)
* Qt 5.15 (Recommended, supported for backward compatiblity).
* OpenRC
* eudev
* elogind
* mDNSResponder
* Samba 4
* QTermWidget

## Components

### Libraries
* libcommdlg/ - Common Dialog Library (About Box, Message Boxes, Open/Save Dialogs, Color/Font Palettes)
* libcompositor/ - the Arion Window Compositor client/server library.
* libshell/ - the Arion Shell Library - the vast majority of the DE's magic lives within this library.
* platformplugin/ - Arion Qt Platform Abstraction plugin
* platformtheme/ - Arion Qt Platform Theme
* shellintegration/ - Qt Wayland shell integration plugin
* style/ - Arion Qt Style

Unless further noted in a sub-directory LICENSE.md - all libraries above are licensed under the GNU Lesser General Public License (LGPL) version 2.

### Core System Binaries:
* compostior/ - Wayland-enabled Desktop Window Compositor
* menuserver/ - Arion DBusMenu/StatusNotifier Server (top panel)
* shellfm/ - Desktop/File Manager
* session/ - Session Manager (startx/startenv replacement)
* settings/ - System Settigns applet host
* sysmon/ - System Monitor (Task Manager)

Unless further noted in a sub-directory LICENSE.md - all applications above are licensed under the GNU General Public License (GPL) version 2.

### System Settings Applets
* applet-datetime/ - Date/Time Configuration
* applet-duck/ - Example Applet
* applet-keyboard/ - Keyboard Configuration
* applet-mouse/ - Mouse Configuration
* applet-network/ - Network (ConnMan) configuration

Unless further noted in a sub-directory LICENSE.md - all applets above are licensed under the GNU General Public License (GPL) version 2.

### Accessories:
* calculator/ - A Calculator
* quicktext/ - A simple text editor
* terminal/ - Terminal Emulator

Unless further noted in a sub-directory LICENSE.md - all applications above are licensed under the GNU General Public License (GPL) version 2.
