TARGET = hweglfsplatformsupport
TEMPLATE=lib
CONFIG+=staticlib
DESTDIR=$${OBJECTS_DIR}../../../output/

QT = core-private gui core5compat
CONFIG += lib
INCLUDEPATH += hollywood/

DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += \
    eglfsfunctions.h \
    hollywood/eglfsfunctions.h \
    hollywood/hollywood-eglfs.h

SOURCES += \
    eglfsfunctions.cpp

target.path = /usr/lib
INSTALLS += target
