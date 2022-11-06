TARGET = HWInputSupport
TEMPLATE = lib

QT = core-private gui-private core5compat dbus
CONFIG += internal_module

DEFINES += QT_NO_CAST_FROM_ASCII

INCLUDEPATH += hollywood/
INCLUDEPATH += ../../libhwlogind
INCLUDEPATH += ../../libhwudev

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
    hollywood/eglfsxkb.h

SOURCES += \
    libinputgesture.cpp \
    libinputhandler.cpp \
    libinputkeyboard.cpp \
    libinputpointer.cpp \
    libinputtouch.cpp \
    logging.cpp \
    ../eglfsxkb/eglfsxkb.cpp

target.path = /usr/lib
INSTALLS += target
