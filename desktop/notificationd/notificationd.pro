include(../include/global.pri)
QT       += core gui widgets dbus multimedia
CONFIG += c++17
TARGET=notificationd

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

target.path = $$PREFIX/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
