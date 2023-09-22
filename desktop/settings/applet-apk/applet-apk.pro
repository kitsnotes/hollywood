include(../../include/global-applet.pri)
TEMPLATE = lib
QT += dbus
CONFIG += link_pkgconfig
TARGET=49-sys-update
INCLUDEPATH += /usr/include/ApkQt/
PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1 apk-qt

SOURCES += \
    apkapplet.cc

HEADERS += \
    apkapplet.h

DISTFILES += applet.json
target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
