include(../../include/global-applet.pri)
TEMPLATE = lib
QT += dbus
CONFIG += link_pkgconfig
TARGET=50-audio
PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1 accountsservice

SOURCES += \
    audioapplet.cc

HEADERS += \
    audioapplet.h

DISTFILES += applet.json
target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
