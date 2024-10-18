include(../include/global.pri)

QT += gui gui-private waylandclient waylandclient-private
CONFIG += plugin wayland-scanner c++11
TARGET = hw-layer-shell
TEMPLATE = lib
INCLUDEPATH += $$PWD/hollywood
INCLUDEPATH += $$PWD/hollywood/private

QMAKE_USE+=wayland-client

WAYLANDCLIENTSOURCES += ../display/compositor/protocols/xdg-shell.xml
WAYLANDCLIENTSOURCES += ../display/compositor/protocols/wlr-layer-shell-unstable-v1.xml
WAYLANDCLIENTSOURCES += ../display/compositor/protocols/xdg-decoration-unstable-v1.xml
WAYLANDCLIENTSOURCES += ../display/compositor/protocols/xdg-activation-v1.xml

SOURCES += \
    src/layershellsurface.cc \
    src/layershellwindow.cc \
    src/logging.cc \
    src/qwaylandlayershell.cc \
    src/qwaylandlayershellintegration.cc \
    src/qwaylandlayershellintegrationplugin.cc \
    src/qwaylandlayersurface.cc \
    src/qwaylandxdgactivationv1.cc \
    src/qwaylandxdgdecorationv1.cc \
    src/qwaylandxdgshell.cc

PUBLIC_HEADERS += \
        hollywood/layershellinterface.h \
        hollywood/layershellqt_export.h \
        hollywood/layershellwindow.h

PRIVATE_HEADERS += \
    hollywood/private/qwaylandlayershell_p.h \
    hollywood/private/qwaylandlayershellintegration_p.h \
    hollywood/private/qwaylandlayersurface_p.h \
    hollywood/private/qwaylandxdgactivationv1_p.h \
    hollywood/private/qwaylandxdgshell_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

OTHER_FILES += \
    layer-shell.json

DISTFILES += \
    layer-shell.json


headers.files = $${PUBLIC_HEADERS}
headers.path = /usr/include/hollywood
pheaders.files = $${PRIVATE_HEADERS}
pheaders.path = /usr/include/hollywood/private


target.path = $$[QT_INSTALL_PLUGINS]/wayland-shell-integration
INSTALLS += target headers pheaders
