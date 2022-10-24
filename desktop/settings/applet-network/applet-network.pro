include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=54-network

CONFIG(debug) {
    INCLUDEPATH += ../../libcmctl
    LIBS += -L../../output -lcmctl-$${HOLLYWOOD_APIVERSION}
}
CONFIG(release) {
    CONFIG += link_pkgconfig
}

SOURCES += \
    netapplet.cc

HEADERS += \
    netapplet.h

DISTFILES += \
    applet-network.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
