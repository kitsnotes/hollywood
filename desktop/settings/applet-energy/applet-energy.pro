include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=55-energy

SOURCES += \
    energyapplet.cc

HEADERS += \
    energyapplet.h

DISTFILES += \
    applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
