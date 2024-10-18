include(../../../../../../include/global.pri)
TARGET = eglfs-kms-integration

PLUGIN_TYPE = hwegldeviceintegrations
PLUGIN_CLASS_NAME = QEglFSKmsGbmIntegrationPlugin
TEMPLATE=lib
CONFIG += plugin link_pkgconfig
QT += core-private gui-private fb_support-private dbus opengl_private

DESTDIR=$${OBJECTS_DIR}../../../../../../output/

INCLUDEPATH += $$PWD/../../api $$PWD/../eglfs_kms_support
INCLUDEPATH += $$PWD/../../api/hollywood/private
INCLUDEPATH += $$PWD/../../api/hollywood
INCLUDEPATH += $$PWD/../../../../../platformsupport/edid
INCLUDEPATH += $$PWD/../../../../../platformheaders/
INCLUDEPATH += $$PWD/../../../../../platformsupport/kmsconvenience
INCLUDEPATH += $$PWD/../../../../../libhwlogind
INCLUDEPATH += $$PWD/../../../../../libhwudev
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

HEADERS += $$PWD/qeglfskmsgbmintegration_p.h \
           $$PWD/qeglfskmsgbmdevice_p.h \
           $$PWD/qeglfskmsgbmscreen_p.h \
           $$PWD/qeglfskmsgbmcursor_p.h \
           $$PWD/qeglfskmsgbmwindow_p.h

OTHER_FILES += $$PWD/eglfs_kms.json

LIBS += -L$$OUT_PWD \
    -L$${OBJECTS_DIR}../../../../../../output/ \
    -L$$PWD/../eglfs_kms_support/ \
    -L$$[QT_INSTALL_PLUGINS]/platforms/ \
    -L$$PWD/../../ \
    -L$$PWD/../../../../../platformsupport/kmsconvenience \
    -L$$PWD/../../../../../platformsupport/edid \
    -lhwudev-$${HOLLYWOOD_APIVERSION} -lhwlogind-$${HOLLYWOOD_APIVERSION} \
    -lHWEglFSDeviceIntegration -lQtEglFsKmsSupport -lHWKmsSupport -lHWInputSupport -lHWEdidSupport

target.path = $$[QT_INSTALL_PLUGINS]/hwdeviceintegrations
INSTALLS += target

