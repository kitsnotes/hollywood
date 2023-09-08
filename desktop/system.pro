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
    elevator \
    style \
    session \
    shellfm \
    compositor \
    libshell \
    settings \
    shellintegration \
    stage \
    sysmon \
    sysmon/helper-sysmon \
    terminal \
    calculator \
    popular \
    appstub \
    notificationd

    libshell.depends = libcompositor
    eglfs.depends = libhwudev libhwlogind
    compositor.depends = libcompositor libshell eglfs
    shellfm.depends = libshell libcompositor libcommdlg
    editor.depends = libcommdlg
    calculator.depends = libcommdlg
    platformtheme.depends = libcommdlg libshell
    settings.depends = libshell libcommdlg
    menuserver.depends = libshell libcompositor
    terminal.depends = libcommdlg
    stage.depends = libcompositor libshell
    session.depends = libshell
    sysmon.depends = libshell libcommdlg
    popular.depends = libshell libcommdlg
}

# We only build libraries on Qt5
lessThan(QT_MAJOR_VERSION, 6) {
SUBDIRS = \
    libcommdlg \
    libcompositor \
    libshell \
    platformtheme \
    platformplugin \
    style
}


