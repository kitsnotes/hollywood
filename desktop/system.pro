TEMPLATE = subdirs
DEFINES += BUILD_HOLLYWOOD

include(include/global.pri)

greaterThan(QT_MAJOR_VERSION, 5) {
SUBDIRS = \
    about \
    apkd \
    libcmctl \
    libcommdlg \
    libpavu \
    platformtheme \
    editor \
    display \
    elevator \
    help \
    style \
    session \
    shellfm \
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
    notificationd \
    volctl \
    quickweb

    libshell.depends = libcommdlg
    shellfm.depends = libshell libcommdlg
    display.depends = libshell
    editor.depends = libcommdlg
    calculator.depends = libcommdlg
    platformtheme.depends = libcommdlg libshell
    settings.depends = libshell libcommdlg libpavu
    menuserver.depends = libshell
    terminal.depends = libcommdlg
    stage.depends = libshell libpavu
    session.depends = libshell
    sysmon.depends = libshell libcommdlg
    popular.depends = libshell libcommdlg
    volctl.depends = libpavu
    quickweb.depends = libcommdlg
}

# We only build libraries on Qt5
lessThan(QT_MAJOR_VERSION, 6) {
SUBDIRS = \
    libcommdlg \
    libshell \
    platformtheme \
    style
}


