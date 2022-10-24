include(../include/global.pri)
CONFIG += console
CONFIG -= app_bundle
QT -= gui
QT += network dbus

TARGET = session

INCLUDEPATH += ../include
SOURCES += \
        session.cc

HEADERS += \
    dbus.h \
    session.h

QMAKE_SUBSTITUTES += hollywood.desktop.in
desktop.path = $$PREFIX/share/wayland-sessions
desktop.files = hollywood.desktop
target.path = $$PREFIX/libexec/hollywood/

INSTALLS += target desktop
DISTFILES += \
    hollywood.desktop.in

