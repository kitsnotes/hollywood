TARGET = hweglfs
TEMPLATE = lib
QT += eglfsdeviceintegration-private core-private gui-private

CONFIG += egl plugin

# Avoid X11 header collision, use generic EGL native types
DEFINES += QT_EGL_NO_X11

SOURCES += $$PWD/qeglfsmain.cpp

OTHER_FILES += $$PWD/hollywood-eglfs.json

INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/../../../platformheaders

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = HollywoodEglFSIntegrationPlugin
DISTFILES += hollywood-eglfs.json

target.path = $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
