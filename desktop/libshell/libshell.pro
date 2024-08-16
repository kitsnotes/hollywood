include(../include/global.pri)

QT += network widgets-private svg dbus

CONFIG(debug, debug|release) {
    TARGET = shelld-$${HOLLYWOOD_APIVERSION}
} else {
    TARGET = shell-$${HOLLYWOOD_APIVERSION}
}

TEMPLATE = lib
DEFINES += LIBSHELL_LIBRARY
# sDEFINES += USE_DNSSD

INCLUDEPATH += include/
INCLUDEPATH += include/private/
INCLUDEPATH += ../libcompositor/include
INCLUDEPATH += ../libcommdlg
QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Shell Library
versionAtLeast(QT_VERSION, 6.0.0) {
    LIBS += -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION} -lcompositor-$${HOLLYWOOD_APIVERSION}
} else {
    TARGET = shell5-$${HOLLYWOOD_APIVERSION}
    LIBS += -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION} -lcompositor5-$${HOLLYWOOD_APIVERSION}
    QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Shell Library (Qt5)
}

SOURCES += \
    src/core/actionmanager.cc \
    src/core/commonfunctions.cc \
    src/core/disks.cc \
    src/core/fileoperation.cc \
    src/core/opmanager.cc \
    src/core/shellundo.cc \
    src/core/udisks2.cc \
    src/dialogs/getinfodialog.cc \
    src/dialogs/viewoptionsdialog.cc \
    src/filesystem/fileinfo.cc \
    src/filesystem/fileinfogatherer.cc \
    src/filesystem/fsitemdelegate.cc \
    src/filesystem/fsnode.cc \
    src/filesystem/hwfileiconprovider.cc \
    src/filesystem/trashinfogatherer.cc \
    src/filesystem/trashnode.cc \
    src/mdns/mdnsbrowser.cc \
    src/mdns/mdnsresolver.cc \
    src/models/appmodel.cc \
    src/models/desktopmodel.cc \
    src/models/filesystemmodel.cc \
    src/models/placemodel.cc \
    src/models/trashmodel.cc \
    src/widgets/columnpreview.cc \
    src/widgets/iconviewoptions.cc \
    src/widgets/locationbar.cpp \
    src/widgets/placeview.cc \
    src/widgets/progresswidget.cc \
    src/widgets/shellhost.cc \
    src/xdg/directories.cc \
    src/xdg/desktopentry.cc \
    src/xdg/executor.cc  \
    src/xdg/mimeapps.cc

HEADERS += \
    include/commonfunctions.h \
    include/getinfodialog.h \
    include/opmanager.h \
    include/private/commonfunctions_p.h \
    include/private/fileinfo.h \
    include/private/actionmanager_p.h \
    include/private/appmodel_p.h \
    include/private/columnpreview_p.h \
    include/private/desktopmodel_p.h \
    include/private/disks.h \
    include/private/opmanager_p.h \
    include/private/shellundo_p.h \
    include/private/udisks2.h \
    include/private/fileinfogatherer_p.h \
    include/private/fileoperation.h \
    include/private/filesystemmodel_p.h \
    include/private/fsitemdelegate_p.h \
    include/private/fsnode.h \
    include/private/getinfodialog_p.h \
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
    include/private/trashinfogatherer_p.h \
    include/private/trashnode.h \
    include/private/trashmodel_p.h \
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
    include/trashmodel.h \
    include/executor.h

#LIBS += -ldns_sd

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
