TARGET = HollywoodEglfsPlatformSupport
TEMPLATE=lib

QT = core-private gui core5compat
CONFIG += static
INCLUDEPATH += hollywood/

DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += \
    eglfsfunctions.h \
    hollywood/eglfsfunctions.h \
    hollywood/hollywood-eglfs.h

SOURCES += \
    eglfsfunctions.cpp


