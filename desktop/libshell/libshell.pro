include(../include/global.pri)

QT += network widgets-private svg

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
    src/executor.cc \
    src/actionmanager.cc \
    src/columnpreview.cc \
    src/desktopentry.cc \
    src/desktopmodel.cc \
    src/directories.cc \
    src/fileinfo.cc \
    src/fileinfogatherer.cpp \
    src/filesystemmodel.cpp \
    src/fsitemdelegate.cc \
    src/fsnode.cc \
    src/hwfileiconprovider.cc \
    src/locationbar.cpp \
    src/lslocationbar.cc \
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
    include/actionmanager.h \
    include/columnpreview.h \
    include/desktopentry.h \
    include/desktopmodel.h \
    include/directories.h \
    include/fileinfo.h \
    include/fileinfogatherer_p.h \
    include/filesystemmodel.h \
    include/filesystemmodel_p.h \
    include/fsitemdelegate.h \
    include/fsnode.h \
    include/hwfileiconprovider.h \
    include/libshell_int.h \
    include/locationbar.h \
    include/locationbarbtn.h \
    include/lslocationbar.h \
    include/mdnsbrowser.h \
    include/mdnsrecord.h \
    include/mdnsresolver.h \
    include/mimeapps.h \
    include/placemodel.h \
    include/placeview.h \
    include/private/executor_p.h \
    include/progresswidget.h \
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
