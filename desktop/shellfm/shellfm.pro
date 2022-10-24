include(../include/global.pri)

QT       += core gui widgets waylandclient
CONFIG   += c++11 wayland-scanner

INCLUDEPATH += ../libshell/include
INCLUDEPATH += include/
INCLUDEPATH += ../libcommdlg/include
INCLUDEPATH += ../libcompositor/include
versionAtLeast(QT_VERSION, 6.0.0) {
    LIBS += -L../libshell -L../output -lshell-$${HOLLYWOOD_APIVERSION}
    LIBS += -I../libcompositor/include -L../output -lcompositor -lcommdlg-$${HOLLYWOOD_APIVERSION}
} else {
    LIBS += -L../libshell -L../output -lshell5-$${HOLLYWOOD_APIVERSION}
    LIBS += -I../libcompositor/include -L../output -lcompositor5 -lcommdlg5-$${HOLLYWOOD_APIVERSION}
}

WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml

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
