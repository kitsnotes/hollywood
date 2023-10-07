include(../include/global.pri)
TEMPLATE = lib
QT += gui gui-private waylandclient waylandclient-private waylandcompositor waylandcompositor-private
CONFIG += wayland-scanner link_pkgconfig compile_libtool create_libtool \
    create_pc create_prl no_install_prl
TARGET = compositor-$${HOLLYWOOD_APIVERSION}

VERSION = $${HOLLYWOOD_APIVERSION}
SEMVER_VERSION = $${HOLLYWOOD_MAJOR_VERSION}.$${HOLLYWOOD_MINOR_VERSION}.$${HOLLYWOOD_PATCH_VERSION}

INCLUDEPATH += ./include

DEFINES += LIBCOMPOSITOR_LIBRARY

SOURCES += \
    src/client/privateprotocol.cc \
    src/client/wlrlayersurfacev1.cpp

HEADERS += \
    include/client/privateprotocol.h \
    include/client/utils_p.h \
    include/client/wlrlayersurfacev1.h \
    include/client/wlrlayersurfacev1_p.h \
    include/libcompositor_global.h


WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDSERVERSOURCES += \
    ../compositor/protocols/wlr-layer-shell-unstable-v1.xml \
    ../compositor/protocols/appmenu.xml

!contains(QT_CONFIG, no-pkg-config) {
    PKGCONFIG += wayland-client
} else {
    LIBS += -lwayland-client
}

QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Compositor Interface Library
!versionAtLeast(QT_VERSION, 6.0.0) {
    TARGET = compositor5-$${HOLLYWOOD_APIVERSION}
    QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Compositor Interface Library (Qt5)
}
headers.files = $${HEADERS}
headers.path = $$PREFIX/include/hollywood/
target.path = $$PREFIX/lib
QMAKE_PKGCONFIG_FILE = $${TARGET}
QMAKE_PKGCONFIG_NAME = $${TARGET}
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_PREFIX = $${PREFIX}
QMAKE_PKGCONFIG_VERSION = $${SEMVER_VERSION}

INSTALLS += target headers
