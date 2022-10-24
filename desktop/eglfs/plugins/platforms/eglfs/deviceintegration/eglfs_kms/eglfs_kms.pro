include(../../../../../../include/global.pri)
TARGET = qeglfs-kms-integration

PLUGIN_TYPE = hwegldeviceintegrations
PLUGIN_CLASS_NAME = QEglFSKmsGbmIntegrationPlugin
TEMPLATE=lib
CONFIG += plugin link_pkgconfig
QT += core-private gui-private eglfsdeviceintegration-private fb_support-private eglfs_kms_support-private kms_support-private dbus
PKGCONFIG += hwudev-$${HOLLYWOOD_APIVERSION} hwlogind-$${HOLLYWOOD_APIVERSION}

INCLUDEPATH += $$PWD/../../api $$PWD/../eglfs_kms_support
INCLUDEPATH += $$PWD/../../api/hollywood/private
INCLUDEPATH += $$PWD/../../../../../platformsupport/edid
INCLUDEPATH += $$PWD/../../../../../platformheaders/
INCLUDEPATH += $$PWD/../../../../../platformsupport/kmsconvenience
INCLUDEPATH += $$PWD/../../../../../../libhwlogind
INCLUDEPATH += $$PWD/../../../../../../libhwudev
INCLUDEPATH += /usr/include/libdrm
# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11

QMAKE_USE += gbm drm
CONFIG += egl

SOURCES += $$PWD/qeglfskmsgbmmain.cpp \
           $$PWD/qeglfskmsgbmintegration.cpp \
           $$PWD/qeglfskmsgbmdevice.cpp \
           $$PWD/qeglfskmsgbmscreen.cpp \
           $$PWD/qeglfskmsgbmcursor.cpp \
           $$PWD/qeglfskmsgbmwindow.cpp

HEADERS += $$PWD/qeglfskmsgbmintegration.h \
           $$PWD/qeglfskmsgbmdevice.h \
           $$PWD/qeglfskmsgbmscreen.h \
           $$PWD/qeglfskmsgbmcursor.h \
           $$PWD/qeglfskmsgbmwindow.h

OTHER_FILES += $$PWD/eglfs_kms.json

target.path = $$[QT_INSTALL_PLUGINS]/hwdeviceintegrations
INSTALLS += target

