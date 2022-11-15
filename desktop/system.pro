TEMPLATE = subdirs
DEFINES += BUILD_HOLLYWOOD

include(include/global.pri)

greaterThan(QT_MAJOR_VERSION, 5) {
SUBDIRS = \
    about \
    libcmctl \
    libcommdlg \
    libcompositor \
    libhwlogind \
    libhwudev \
    platformtheme \
    platformplugin \
    editor \
    eglfs \
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
    settings/applet-efiboot \
    settings/applet-general \
    settings/applet-displays \
    settings/applet-network \
    settings/applet-keyboard \
    settings/applet-mouse \
    settings/applet-wallpaper \
    settings/applet-region \
    settings/applet-stage \
    settings/applet-energy \
    shellintegration \
    stage \
    sysmon \
    sysmon/helper-sysmon \
    terminal \
    calculator \
    popular

    libshell.depends = libcompositor
    eglfs.depends = libhwudev libhwlogind
    compositor.depends = libcompositor libshell eglfs
    shellfm.depends = libshell libcompositor libcommdlg
    editor.depends = libcommdlg
    calculator.depends = libcommdlg
    settings.depends = libshell libcommdlg
    menuserver.depends = libshell libcompositor
    terminal.depends = libcommdlg
    stage.depends = libcompositor libshell
}

# We only build libraries on Qt5
lessThan(QT_MAJOR_VERSION, 6) {
SUBDIRS = \
    libcommdlg \
    libcompositor \
    platformtheme \
    platformplugin \
    style \
    libshell
}

