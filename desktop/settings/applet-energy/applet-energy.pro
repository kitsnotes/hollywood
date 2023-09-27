include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=56-energy

SOURCES += \
    energyapplet.cc \
    ../../shared/labeledslider.cc \
    ../../shared/upower.cc \
    meter.cc

HEADERS += \
    energyapplet.h \
    ../../shared/labeledslider.h \
    ../../shared/upower.h \
    meter.h

DISTFILES += \
    applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
