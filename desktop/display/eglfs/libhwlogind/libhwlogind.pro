include(../../include/global.pri)

QT += core dbus
QT -= gui
TEMPLATE = lib
TARGET = hwlogind
CONFIG += staticlib

PKGCONFIG += libsystemd
INCLUDEPATH += $$PWD/hollywood
DESTDIR=$${OBJECTS_DIR}../../../output/

SOURCES += \
    logind.cpp \
    logindtypes.cpp


VERSION = $${HOLLYWOOD_APIVERSION}
SEMVER_VERSION = $${HOLLYWOOD_MAJOR_VERSION}.$${HOLLYWOOD_MINOR_VERSION}.$${HOLLYWOOD_PATCH_VERSION}

PUBLIC_HEADERS = hollywood/logind.h

PRIVATE_HEADERS += \
    hollywood/private/defaultlogind_p_p.h \
    hollywood/private/logind_p.h \
    hollywood/private/logindtypes_p.h

HEADERS = $$PRIVATE_HEADERS $$PUBLIC_HEADERS

QMAKE_PKGCONFIG_DESCRIPTION = Hollywood elogind interface library

target.path = $$PREFIX/lib
headers.path = $$PREFIX/include/hollywood
pheaders.path = $$PREFIX/include/hollywood/private

headers.files += $${PUBLIC_HEADERS}
pheaders.files += $${PRIVATE_HEADERS}
QMAKE_PKGCONFIG_FILE = $${TARGET}
QMAKE_PKGCONFIG_NAME = $${TARGET}
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_PREFIX = $${PREFIX}
QMAKE_PKGCONFIG_VERSION = $${SEMVER_VERSION}

INSTALLS += target
