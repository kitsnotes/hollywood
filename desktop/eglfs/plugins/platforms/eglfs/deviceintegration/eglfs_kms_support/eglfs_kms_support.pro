include(../../../../../../include/global.pri)
TARGET = QtEglFsKmsSupport
CONFIG += static
TEMPLATE=lib link_pkgconfig
QT += core-private gui-private fb_support-private dbus

DESTDIR=$${OBJECTS_DIR}../../../../../../output/

INCLUDEPATH += $$PWD/hollywood
INCLUDEPATH += $$PWD/../../api/hollywood/private
INCLUDEPATH += $$PWD/../../api/
INCLUDEPATH += $$PWD/../../api/hollywood
INCLUDEPATH += $$PWD/../../../../../platformheaders/
INCLUDEPATH += $$PWD/../../../../../platformsupport/edid
INCLUDEPATH += $$PWD/../../../../../platformsupport/kmsconvenience
INCLUDEPATH += $$PWD/../../../../../../libhwlogind
INCLUDEPATH += /usr/include/libdrm
# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11

CONFIG += egl

SOURCES += $$PWD/qeglfskmsintegration.cpp \
           $$PWD/qeglfskmsdevice.cpp \
           $$PWD/qeglfskmsscreen.cpp \
           qeglfskmseventreader.cpp

HEADERS += $$PWD/hollywood/qeglfskmsintegration.h \
           $$PWD/hollywood/qeglfskmsdevice.h \
           $$PWD/hollywood/qeglfskmsscreen.h \
           $$PWD/hollywood/qeglfskmshelpers.h \
           hollywood/qeglfskmseventreader.h
