QT       += core gui widgets waylandclient concurrent
CONFIG   += c++11 wayland-scanner
DESTDIR= ../output

INCLUDEPATH += ../include/
INCLUDEPATH += ../libshell/include
INCLUDEPATH += ../libcommdlg/include
INCLUDEPATH += ../libcompositor/include
LIBS += -L../libshell -L../output -lshell
LIBS += -I../libcompositor/include -L../output -lcompositor -lcommdlg

WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/plasma-window-management.xml


CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    app.cc \
    progman.cc \
    window.cc \
    wndmgmt.cc

HEADERS += \
    app.h \
    progman.h \
    window.h \
    wndmgmt.h

# Default rules for deployment.
unix:!android: target.path = /usr/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
