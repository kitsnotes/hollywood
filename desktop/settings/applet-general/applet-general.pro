include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=01-general

SOURCES += \
    generalapplet.cc

HEADERS += \
    generalapplet.h

DISTFILES += \
    applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
