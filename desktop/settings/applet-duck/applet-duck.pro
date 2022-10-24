QT += gui

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11
INCLUDEPATH += ../
DESTDIR= ../../output/hollywood-settings

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    duckapplet.cc

HEADERS += \
    duckapplet.h

DISTFILES += applet-duck.json
unix {
    target.path = /usr/libexec/hollwyood/settings
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    duck.qrc
