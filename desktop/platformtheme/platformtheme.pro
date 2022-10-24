include(../include/global.pri)

QT += core-private gui dbus-private
QT += dbus waylandclient waylandclient-private widgets-private

TARGET=hollywood
TEMPLATE = lib
CONFIG += plugin wayland-scanner
INCLUDEPATH += include/
DEFINES -=QT_NO_SIGNALS_SLOTS_KEYWORDS

WAYLANDCLIENTSOURCES += ../compositor/protocols/appmenu.xml

SOURCES += \
    src/platformtheme.cc \
    src/qdbusmenuadaptor.cc \
    src/qdbusmenubar.cc \
    src/qdbusmenusupport.cc \
    src/qdbusmenutypes.cc \
    src/qdbusplatformmenu.cc \
    src/wayland.cc

HEADERS += \
    include/platformtheme.h \
    include/qdbusmenuadaptor_p.h \
    include/qdbusmenubar_p.h \
    include/qdbusmenuregistrarproxy_p.h \
    include/qdbusmenutypes_p.h \
    include/qdbusplatformmenu_p.h \
    include/wayland.h

DISTFILES += platformtheme.json

target.path = $$[QT_INSTALL_PLUGINS]/platformthemes
INSTALLS += target
