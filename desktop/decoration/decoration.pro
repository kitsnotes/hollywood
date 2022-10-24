QT += gui gui-private waylandclient waylandclient-private

TARGET=hollywood
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11

SOURCES += \
    decoration.cc

HEADERS += \
    decoration.h

OTHER_FILES += \
    hollywood.json

DISTFILES += \
    hollywood.json

unix {
    target.path = $$[QT_INSTALL_PLUGINS]/wayland-decoration-client
}
!isEmpty(target.path): INSTALLS += target
