include(../../include/global.pri)
TARGET = HWInputSupport
TEMPLATE = lib
CONFIG += staticlib

QT = core-private gui-private core5compat dbus
DESTDIR=$${OBJECTS_DIR}../../../output/

LIBS += -L../../output -lhwudev -linput

DEFINES += QT_NO_CAST_FROM_ASCII

INCLUDEPATH += hollywood/
INCLUDEPATH += ../libhwlogind
INCLUDEPATH += ../libhwudev ../libhwudev/hollywood

INCLUDEPATH += ../eglfsxkb
INCLUDEPATH += ../eglfsxkb/hollywood

HEADERS += \
    hollywood/libinputgesture.h \
    hollywood/libinputglobal.h \
    hollywood/libinputhandler.h \
    hollywood/private/libinputhandler_p.h \
    hollywood/libinputkeyboard.h \
    hollywood/private/libinputkeyboard_p.h \
    hollywood/libinputpointer.h \
    hollywood/libinputtouch.h \
    hollywood/private/libinputlogging_p.h \
    hollywood/eglfsxkb.h \
    hollywood/private/outputmapping_p.h

SOURCES += \
    libinputgesture.cpp \
    libinputhandler.cpp \
    libinputkeyboard.cpp \
    libinputpointer.cpp \
    libinputtouch.cpp \
    logging.cpp \
    outputmapping.cpp \
    ../eglfsxkb/eglfsxkb.cpp

target.path = /usr/lib
INSTALLS += target
