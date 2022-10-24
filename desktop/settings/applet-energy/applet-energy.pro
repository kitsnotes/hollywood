QT += gui

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11
INCLUDEPATH += ../
DESTDIR= ../../output/hollywood-settings
TARGET=55-energy

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    energyapplet.cc

HEADERS += \
    energyapplet.h

DISTFILES += applet.json
unix {
    target.path = /usr/libexec/holywood/settings
}
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
