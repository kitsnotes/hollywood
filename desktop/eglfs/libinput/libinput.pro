TARGET = EglfsLibinputSupport
TEMPLATE = lib

QT = core-private gui-private core5compat dbus
CONFIG += static

DEFINES += QT_NO_CAST_FROM_ASCII

INCLUDEPATH += hollywood/
INCLUDEPATH += ../../libhwlogind
INCLUDEPATH += ../../libhwudev

INCLUDEPATH += ../eglfsxkb

HEADERS += \
    hollywood/libinputgesture.h \
    hollywood/libinputglobal.h \
    hollywood/libinputhandler.h \
    hollywood/private/libinputhandler_p.h \
    hollywood/libinputkeyboard.h \
    hollywood/private/libinputkeyboard_p.h \
    hollywood/libinputpointer.h \
    hollywood/libinputtouch.h \
    hollywood/private/libinputlogging_p.h

SOURCES += \
    libinputgesture.cpp \
    libinputhandler.cpp \
    libinputkeyboard.cpp \
    libinputpointer.cpp \
    libinputtouch.cpp \
    logging.cpp

