include(../../../../include/global.pri)
TEMPLATE=lib
TARGET = HWEglFSDeviceIntegration
CONFIG += internal_module link_pkgconfig
MODULE = eglfsdeviceintegration
QT += \
    core-private gui-private
QT += input_support-private opengl-private fb_support-private dbus
#QT += platformcompositor_support-private
#QT += vulkan_support-private

DESTDIR=$${OBJECTS_DIR}../../../../output/
# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11
DEFINES += QT_BUILD_EGL_DEVICE_LIB
#PKGCONFIG += hwlogind-1.0 hwudev-1.0
include($$PWD/api/api.pri)

!isEmpty(EGLFS_PLATFORM_HOOKS_SOURCES) {
    HEADERS += $$EGLFS_PLATFORM_HOOKS_HEADERS
    SOURCES += $$EGLFS_PLATFORM_HOOKS_SOURCES
    LIBS    += $$EGLFS_PLATFORM_HOOKS_LIBS
    DEFINES += EGLFS_PLATFORM_HOOKS
}

!isEmpty(EGLFS_DEVICE_INTEGRATION) {
    DEFINES += EGLFS_PREFERRED_PLUGIN=$$EGLFS_DEVICE_INTEGRATION
}

CONFIG += egl

RESOURCES += $$PWD/cursor.qrc

INCLUDEPATH += $$PWD/../../../platformheaders/hollywood/

HEADERS += $$PWD/../../../platformheaders/hollywood/eglfsfunctions.h
SOURCES += $$PWD/../../../platformheaders/eglfsfunctions.cpp

LIBS += -L$$OUT_PWD \
     -L$${OBJECTS_DIR}../../../../output/ \
     -lhwudev-$${HOLLYWOOD_APIVERSION} -lhwlogind-$${HOLLYWOOD_APIVERSION} \
     -lHWKmsSupport -lHWInputSupport -lHWEdidSupport
target.path = /usr/lib
INSTALLS += target
