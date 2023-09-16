include(vars.pri)

isEmpty(PREFIX): PREFIX=/usr
QT += core gui widgets dbus
CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

INCLUDEPATH += $${PWD}
INCLUDEPATH += ../../shared/

HEADERS +=     ../../shared/shellfunc.h
CONFIG += plugin
CONFIG(debug): DESTDIR=$${OBJECTS_DIR}../../output

