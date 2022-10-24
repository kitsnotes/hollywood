TEMPLATE = subdirs
DEFINES += BUILD_HOLLYWOOD

include(include/global.pri)

# Projects for both Qt5 and Qt6
SUBDIRS = \
    about \
    libcmctl \
    libcommdlg \
    libcompositor \
    platformtheme \
    platformplugin \
    editor \
    settings/applet-efiboot \
    settings/helper-datetime \
    style \
    libshell \
    menuserver \
    session \
    shellfm \
    compositor \
    elevator \
    settings \
    settings/applet-apk \
    settings/applet-datetime \
    settings/applet-general \
    settings/applet-displays \
    settings/applet-network \
    settings/applet-keyboard \
    settings/applet-mouse \
    settings/applet-wallpaper \
    settings/applet-region \
    sysmon \
    sysmon/helper-sysmon \
    terminal \
    calculator \
    shellintegration \
    stage

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS -= about \
    libcmctl \
    editor \
    settings/applet-efiboot \
    settings/helper-datetime \
    menuserver \
    session \
    shellfm \
    compositor \
    elevator \
    settings \
    settings/applet-apk \
    settings/applet-datetime \
    settings/applet-general \
    settings/applet-displays \
    settings/applet-network \
    settings/applet-keyboard \
    settings/applet-mouse \
    settings/applet-wallpaper \
    settings/applet-region \
    sysmon \
    sysmon/helper-sysmon \
    terminal \
    calculator \
    stage
}

eglfs.depends = libhwudev libhwlogind
libshell.depends = libcompositor
compositor.depends = libcompositor
shellfm.depends = libshell libcompositor libcommdlg
editor.depends = libcommdlg
calculator.depends = libcommdlg
settings.depends = libshell libcommdlg
menuserver.depends = libshell libcompositor
terminal.depends = libcommdlg
