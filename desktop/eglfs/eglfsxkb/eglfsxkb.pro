TARGET = EglfsXkbSupport
TEMPLATE = lib

QT = core-private core5compat
CONFIG += static link_pkgconfig
PKGCONFIG += xkbcommon

DEFINES += QT_NO_CAST_FROM_ASCII
INCLUDEPATH += hollywood

HEADERS += hollywood/eglfsxkb.h
SOURCES += eglfsxkb.cpp

