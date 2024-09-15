include(../include/global.pri)

QT       += core gui widgets waylandclient concurrent waylandclient-private dbus multimedia
CONFIG   += c++11 wayland-scanner
DESTDIR= ../output
TARGET = stage
INCLUDEPATH += ../shared/
INCLUDEPATH += ../libshell/include
INCLUDEPATH += ../libcommdlg/
INCLUDEPATH += ../libcompositor/include/
INCLUDEPATH += ../shellintegration/
INCLUDEPATH += /usr/include/dbusmenu-qt6/

# TODO: prefix with -APIVERSION
CONFIG(debug): LIBS += -ldbusmenu-qt6 -L../output -lshell-$${HOLLYWOOD_APIVERSION} -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION} -lhw-layer-shell
CONFIG(release): LIBS += -ldbusmenu-qt6 -L/usr/lib/qt6/plugins/wayland-shell-integration -lshell-$${HOLLYWOOD_APIVERSION} -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION} -lhw-layer-shell

WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/plasma-window-management.xml


CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    stage.cc \
    taskview/appbutton.cc \
    battery.cc \
    menuserver/menuregistrarimporter.cc \
    menuserver/utils.cc \
    menuserver/menuserver.cc \
    stageclock.cc \
    stagehost.cc \
    statusnotifier/dbustypes.cc \
    statusnotifier/sniasync.cc \
    statusnotifier/statusnotifierbutton.cc \
    statusnotifier/statusnotifieriteminterface.cc \
    statusnotifier/statusnotifierproxy.cc \
    statusnotifier/notifierhost.cc \
    statusnotifier/statusnotifierwatcher.cc \
    surfacemanager.cc \
    taskview/stagetasklist.cc \
    ../shared/upower.cc \
    taskview/windowlist.cc \
    wndmgmt.cc

HEADERS += \
    stage.h \
    taskview/appbutton.h \
    battery.h \
    menuserver/menuregistrarimporter.h \
    menuserver/utils.h \
    menuserver/menuserver.h \
    stageclock.h \
    stagehost.h \
    statusnotifier/dbustypes.h \
    statusnotifier/sniasync.h \
    statusnotifier/statusnotifierbutton.h \
    statusnotifier/statusnotifieriteminterface.h \
    statusnotifier/statusnotifierproxy.h \
    statusnotifier/statusnotifierwatcher.h \
    statusnotifier/notifierhost.h \
    surfacemanager.h \
    taskview/stagetasklist.h \
    ../shared/upower.h \
    taskview/taskview.h \
    taskview/windowlist.h \
    wndmgmt.h

# Default rules for deployment.
unix:!android: target.path = /usr/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
