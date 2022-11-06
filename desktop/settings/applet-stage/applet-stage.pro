include(../../include/global-applet.pri)
TEMPLATE = lib
TARGET=03-stage

SOURCES += \
    stageapplet.cc

HEADERS += \
    stageapplet.h

DISTFILES += applet.json
unix {
    target.path = /usr/libexec/holywood/settings
}
!isEmpty(target.path): INSTALLS += target

