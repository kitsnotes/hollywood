include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=54-network
INCLUDEPATH += ../../shared/

CONFIG(debug) {
    INCLUDEPATH += ../../libcmctl
    LIBS += -L../../output -lcmctl-$${HOLLYWOOD_APIVERSION}
}
CONFIG(release) {
    CONFIG += link_pkgconfig
}

SOURCES += \
    netapplet.cc \
    ../../shared/settingsdelegate.cc

HEADERS += \
    netapplet.h \
    ../../shared/settingsdelegate.h

DISTFILES += \
    applet-network.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
