include(../include/global.pri)

TEMPLATE = lib
TARGET = commdlg-$${HOLLYWOOD_APIVERSION}
CONFIG += compile_libtool create_libtool create_pc create_prl no_install_prl
DEFINES += LIBCOMMDLG_LIBRARY
VERSION = $${HOLLYWOOD_APIVERSION}
SEMVER_VERSION = $${HOLLYWOOD_MAJOR_VERSION}.$${HOLLYWOOD_MINOR_VERSION}.$${HOLLYWOOD_PATCH_VERSION}
greaterThan(QT_MAJOR_VERSION, 5): QT += multimedia
INCLUDEPATH += private/
INCLUDEPATH += $$PWD $$PWD/hollywood/ $$PWD/hollywood/private

SOURCES += \
    aboutdialog.cc \
    messagebox.cc \
    preferencedialog.cc

HEADERS += \
    hollywood/aboutdialog.h \
    hollywood/libcommdlg_global.h \
    hollywood/messagebox.h \
    hollywood/private/aboutdialog_p.h \
    hollywood/private/messagebox_p.h \
    hollywood/preferencedialog.h \
    hollywood/private/preferencedialog_p.h

QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Common Dialog Library
!versionAtLeast(QT_VERSION, 6.0.0) {
    TARGET = commdlg5-$${HOLLYWOOD_APIVERSION}
    QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Common Dialog Library (Qt5)
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
