include(../include/global.pri)
QT       += core gui widgets dbus multimedia
CONFIG += c++17
TARGET=notificationd
INCLUDEPATH += ../shellintegration

SOURCES += \
    adaptor.cc \
    daemon.cc \
    notification.cc \
    notifywindow.cc

HEADERS += \
    adaptor.h \
    daemon.h \
    notification.h \
    notifywindow.h

LIBS += -L../output \
        -L/usr/lib/qt6/plugins/wayland-shell-integration \
        -lhw-layer-shell

target.path = $$PREFIX/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
