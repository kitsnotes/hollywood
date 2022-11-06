TEMPLATE = subdirs
DEFINES += BUILD_HOLLYWOOD

include(include/global.pri)

# Projects for both Qt5 and Qt6
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
    settings/applet-stage \
    settings/applet-energy \
    sysmon \
    shellintegration \
    sysmon/helper-sysmon \
    terminal \
    calculator \
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
    shellintegration \
    terminal \
    calculator \
    stage
}
libshell.depends = libcompositor
compositor.depends = libcompositor
shellfm.depends = libshell libcompositor libcommdlg
editor.depends = libcommdlg
calculator.depends = libcommdlg
settings.depends = libshell libcommdlg
menuserver.depends = libshell libcompositor
terminal.depends = libcommdlg
