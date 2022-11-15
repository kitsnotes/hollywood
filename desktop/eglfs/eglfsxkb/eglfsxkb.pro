include(../../include/global.pri)
TARGET = HWEglfsXkbSupport
TEMPLATE = lib

QT = core-private core5compat
CONFIG += link_pkgconfig
PKGCONFIG += xkbcommon

DEFINES += QT_NO_CAST_FROM_ASCII
INCLUDEPATH += hollywood

HEADERS += hollywood/eglfsxkb.h
SOURCES += eglfsxkb.cpp

target.path = /usr/lib
INSTALLS += target
