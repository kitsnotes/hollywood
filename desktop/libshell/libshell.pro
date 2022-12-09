include(../include/global.pri)

QT += network widgets-private svg dbus

CONFIG(debug, debug|release) {
    TARGET = shelld-$${HOLLYWOOD_APIVERSION}
} else {
    TARGET = shell-$${HOLLYWOOD_APIVERSION}
}

TEMPLATE = lib
DEFINES += LIBSHELL_LIBRARY
DEFINES += USE_DNSSD

INCLUDEPATH += include/
INCLUDEPATH += include/private/
INCLUDEPATH += ../libcompositor/include
QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Shell Library
versionAtLeast(QT_VERSION, 6.0.0) {
    LIBS += -I../libcompositor/include -L../output -lcompositor-$${HOLLYWOOD_APIVERSION}
} else {
    TARGET = shell5-$${HOLLYWOOD_APIVERSION}
    LIBS += -I../libcompositor/include -L../output -lcompositor5-$${HOLLYWOOD_APIVERSION}
    QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Shell Library (Qt5)
}

SOURCES += \
    src/appmodel.cc \
    src/executor.cc \
    src/actionmanager.cc \
    src/columnpreview.cc \
    src/desktopentry.cc \
    src/desktopmodel.cc \
    src/directories.cc \
    src/fileinfo.cc \
    src/fileinfogatherer.cc \
    src/filesystemmodel.cc \
    src/fsitemdelegate.cc \
    src/fsnode.cc \
    src/hwfileiconprovider.cc \
    src/locationbar.cpp \
    src/mdnsbrowser.cc \
    src/mdnsresolver.cc \
    src/mimeapps.cc \
    src/placemodel.cc \
    src/placeview.cc \
    src/progresswidget.cc \
    src/shellhost.cc \
    src/viewoptionsdialog.cc \
    src/iconviewoptions.cc

HEADERS += \
    include/private/fileinfo.h \
    include/private/actionmanager_p.h \
    include/private/appmodel_p.h \
    include/private/columnpreview_p.h \
    include/private/desktopmodel_p.h \
    include/private/fileinfogatherer_p.h \
    include/private/filesystemmodel_p.h \
    include/private/fsitemdelegate_p.h \
    include/private/fsnode.h \
    include/private/iconviewoptions_p.h \
    include/private/locationbar_p.h \
    include/private/locationbarbtn.h \
    include/private/mdnsbrowser.h \
    include/private/mdnsrecord.h \
    include/private/mdnsresolver.h \
    include/private/mimeapps_p.h \
    include/private/placemodel.h \
    include/private/placeview.h \
    include/private/executor_p.h \
    include/private/progresswidget.h \
    include/appmodel.h \
    include/actionmanager.h \
    include/columnpreview.h \
    include/desktopentry.h \
    include/desktopmodel.h \
    include/directories.h \
    include/filesystemmodel.h \
    include/fsitemdelegate.h \
    include/hwfileiconprovider.h \
    include/libshell_int.h \
    include/locationbar.h \
    include/mimeapps.h \
    include/private/shellhost_p.h \
    include/private/viewoptionsdialog_p.h \
    include/shellhost.h \
    include/viewoptionsdialog.h \
    include/iconviewoptions.h \
    include/executor.h

LIBS += -ldns_sd

headers.files = $${HEADERS}
headers.path = $$PREFIX/include/hollywood/
target.path = $$PREFIX/lib
QMAKE_PKGCONFIG_FILE = $${TARGET}
QMAKE_PKGCONFIG_NAME = $${TARGET}
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_PREFIX = $${PREFIX}

INSTALLS += target headers

RESOURCES += \
    res.qrc
