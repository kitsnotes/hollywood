include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=01-general
LIBS+= -lQt6GSettings

SOURCES += \
    generalapplet.cc

HEADERS += \
    generalapplet.h

DISTFILES += \
    applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
