include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=52-keyboard

SOURCES += \
    kbdapplet.cc \
    layoutselector.cc

HEADERS += \
    kbdapplet.h \
    layoutselector.h

DISTFILES += \
    applet-kbd.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
