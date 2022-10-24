CONFIG += wayland-scanner
QT += gui gui-private waylandclient waylandclient-private

TARGET=hollywood
TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11
INCLUDEPATH += include/

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x051300
WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/appmenu.xml

SOURCES += \
    src/platformplugin.cc

HEADERS += \
    include/platformplugin.h

DISTFILES += platformplugin.json

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/platforms
}
!isEmpty(target.path): INSTALLS += target
