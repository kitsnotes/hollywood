include(../include/global.pri)

QT       += core gui widgets waylandclient
CONFIG   += c++11 wayland-scanner link_pkgconfig

INCLUDEPATH += ../libshell/include
INCLUDEPATH += include/
INCLUDEPATH += ../libcommdlg/include
INCLUDEPATH += ../libcompositor/include

CONFIG(debug, debug|release) {
    LIBS += -L../output -lshelld-$${HOLLYWOOD_APIVERSION}
} else {
    LIBS += -lshell-$${HOLLYWOOD_APIVERSION}
}

LIBS += -L../output -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION}
WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
# PKGCONFIG += hwcompositor
SOURCES += \
    clipboardwindow.cc \
    desktop.cc \
    fmapplication.cc \
    filewindow.cc


HEADERS += \
    clipboardwindow.h \
    desktop.h \
    fileshell.h \
    filewindow.h \
    fmapplication.h


unix {
    target.path = /usr/bin
}
!isEmpty(target.path): INSTALLS += target
