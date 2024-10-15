QT       += core gui svg widgets
QT -= quick qml dbus virtualkeyboard
CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
TEMPLATE = app
LIBS += -lcommdlg-1.0
SOURCES += \
    bootstrap.cc \
    menu.cc

HEADERS += \
    bootstrap.h \
    menu.h

RESOURCES += \
    res.qrc


DISTFILES += \
    res/firmware-list.txt
