include(../../../../../include/global.pri)
TEMPLATE=lib
CONFIG += internal_module link_pkgconfig

TARGET = hweglfs
TEMPLATE = lib
QT += core-private gui-private

CONFIG += egl plugin
DESTDIR=$${OBJECTS_DIR}../../../../../output/

# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11

SOURCES += $$PWD/qeglfsmain.cpp

OTHER_FILES += $$PWD/hollywood-eglfs.json

INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/../../../platformheaders

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = HWEglFSIntegrationPlugin
DISTFILES += hollywood-eglfs.json

target.path = $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target

LIBS += -L../../../../../output/ -lhwudev -lhwlogind -lHWEglFSDeviceIntegration -lHWInputSupport -L$$OUT_PWD
