QT       += core gui widgets waylandclient concurrent waylandclient-private
CONFIG   += c++11 wayland-scanner
DESTDIR= ../output
TARGET = stage
INCLUDEPATH += ../include/
INCLUDEPATH += ../libshell/include
INCLUDEPATH += ../libcommdlg/
INCLUDEPATH += ../libcompositor/
INCLUDEPATH += ../shellintegration/

LIBS += -L../output -lshell -lcompositor -lcommdlg -lhw-layer-shell

WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/plasma-window-management.xml


CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    app.cc \
    progman.cc \
    stagehost.cc \
    taskbutton.cc \
    wndmgmt.cc

HEADERS += \
    app.h \
    progman.h \
    stagehost.h \
    taskbutton.h \
    wndmgmt.h

# Default rules for deployment.
unix:!android: target.path = /usr/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
