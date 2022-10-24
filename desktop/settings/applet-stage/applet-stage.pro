QT += gui

TEMPLATE = lib
CONFIG += plugin
TARGET=03-stage
CONFIG += c++11
INCLUDEPATH += ../
DESTDIR= ../../output/hollywood-settings

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    stageapplet.cc

HEADERS += \
    stageapplet.h

DISTFILES += applet.json
unix {
    target.path = /usr/libexec/holywood/settings
}
!isEmpty(target.path): INSTALLS += target

