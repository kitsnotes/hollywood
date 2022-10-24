include(../../include/global-applet.pri)
TEMPLATE = lib
TARGET=53-mouse
SOURCES += \
    mouseapplet.cc

HEADERS += \
    mouseapplet.h

DISTFILES += \
    applet-mouse.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target
