include(../../include/global-applet.pri)
TEMPLATE = lib
QT += dbus
CONFIG += link_pkgconfig
TARGET=60-usersgroups
PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1 accountsservice
LIBS += -lQt6AccountsService

SOURCES += \
    ../../shared/settingsdelegate.cc \
    usersapplet.cc

HEADERS += \
    ../../shared/settingsdelegate.h \
    usersapplet.h

DISTFILES += applet.json
target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
