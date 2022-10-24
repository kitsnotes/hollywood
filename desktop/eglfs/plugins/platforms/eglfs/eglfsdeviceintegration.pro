TEMPLATE=lib
TARGET = QtEglFSDeviceIntegration
CONFIG += static
QT += \
    core-private gui-private
QT += input_support-private opengl-private fb_support-private dbus
#QT += platformcompositor_support-private
#QT += vulkan_support-private

# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11
DEFINES += QT_BUILD_EGL_DEVICE_LIB

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
