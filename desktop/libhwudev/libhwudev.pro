include(../include/global.pri)
TEMPLATE=lib
QT -= gui widgets
QT += dbus
PKGCONFIG += libudev
TARGET = hwudev-$${HOLLYWOOD_APIVERSION}
CONFIG += link_pkgconfig compile_libtool create_libtool create_pc create_prl no_install_prl
INCLUDEPATH += $$PWD $$PWD/hollywood/

VERSION = $${HOLLYWOOD_APIVERSION}
SEMVER_VERSION = $${HOLLYWOOD_MAJOR_VERSION}.$${HOLLYWOOD_MINOR_VERSION}.$${HOLLYWOOD_PATCH_VERSION}

PUBLIC_HEADERS += hollywood/udev.h \
                hollywood/udevdevice.h \
                hollywood/udevenumerate.h \
                hollywood/udevmonitor.h

PRIVATE_HEADERS += \
    hollywood/private/logindseat_p.h \
    hollywood/qtudevglobal.h \    
    hollywood/private/udev_p.h \
    hollywood/private/udevdevice_p.h \
    hollywood/private/udevenumerate_p.h \
    hollywood/private/udevmonitor_p.h

HEADERS = $$PRIVATE_HEADERS $$PUBLIC_HEADERS

SOURCES += \
    logindseat.cpp \
    udev.cpp \
    udevdevice.cpp \
    udevenumerate.cpp \
    udevmonitor.cpp

QMAKE_PKGCONFIG_DESCRIPTION = Hollywood eudev interface library

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

INSTALLS += target headers pheaders
