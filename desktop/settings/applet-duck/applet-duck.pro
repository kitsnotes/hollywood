include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=11-duck

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    duckapplet.cc

HEADERS += \
    duckapplet.h

DISTFILES += applet-duck.json
unix {
    target.path = /usr/libexec/hollywood/settings
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    duck.qrc
