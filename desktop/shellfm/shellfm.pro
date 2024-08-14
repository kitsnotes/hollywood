include(../include/global.pri)

QT       += core gui widgets waylandclient network dbus multimedia
CONFIG   += c++11 wayland-scanner link_pkgconfig

INCLUDEPATH += ../libshell/include
INCLUDEPATH += include/
INCLUDEPATH += ../libcommdlg
INCLUDEPATH += ../libcompositor/include/
#QMAKE_LFLAGS += -Wl,--allow-shlib-undefined
TARGET = shellfm
DBUS_ADAPTORS += org.freedesktop.FileManager1.xml
LIBS += -lshell-$${HOLLYWOOD_APIVERSION}

LIBS += -L../output -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION}
WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
# PKGCONFIG += hwcompositor
SOURCES += \
    clipboardwindow.cc \
    desktop.cc \
    desktopviewoptions.cc \
    fmapplication.cc \
    filewindow.cc


HEADERS += \
    clipboardwindow.h \
    desktop.h \
    desktopviewoptions.h \
    fileshell.h \
    filewindow.h \
    fmapplication.h


# Default rules for deployment.
QMAKE_SUBSTITUTES +=org.originull.shellfm.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.shellfm.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop

DISTFILES += \
    org.originull.shellfm.desktop.in
