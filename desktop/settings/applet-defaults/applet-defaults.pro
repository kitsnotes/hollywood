include(../../include/global-applet.pri)

TEMPLATE = lib
CONFIG += link_pkgconfig
TARGET=10-defaults

CONFIG(debug, debug|release) {
    LIBS += -L../../output -lshelld-$${HOLLYWOOD_APIVERSION}
} else {
    LIBS += -L../../output -lshell-$${HOLLYWOOD_APIVERSION}
}

INCLUDEPATH += ../../libshell/include

SOURCES += \
    defaults.cc \
    mimetypemodel.cc

HEADERS += \
    defaults.h \
    mimetypemodel.h

DISTFILES += \
    applet.json

target.path = /usr/libexec/hollywood/settings
!isEmpty(target.path): INSTALLS += target
INSTALLS += target

RESOURCES +=
