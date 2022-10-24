include(../../include/global-applet.pri)

QT += core5compat

TEMPLATE = lib
TARGET=08-regionlaguage

SOURCES += \
    regionapplet.cc

HEADERS += \
    regionapplet.h

DISTFILES += applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
