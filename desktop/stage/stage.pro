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

# TODO: prefix with -APIVERSION
CONFIG(debug): LIBS += -L../output -lshell-$${HOLLYWOOD_APIVERSION} -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION} -lhw-layer-shell
CONFIG(release): LIBS += -L/usr/lib/qt6/plugins/wayland-shell-integration -lshell-$${HOLLYWOOD_APIVERSION} -lcompositor-$${HOLLYWOOD_APIVERSION} -lcommdlg-$${HOLLYWOOD_APIVERSION} -lhw-layer-shell

WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/plasma-window-management.xml


CONFIG += c++11
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    app.cc \
    battery.cc \
    dbusmenu/dbusmenu_interface.cc \
    dbusmenu/dbusmenuimporter.cc \
    dbusmenu/dbusmenushortcut.cc \
    dbusmenu/dbusmenutypes.cc \
    dbusmenu/menuimporter.cc \
    dbusmenu/utils.cc \
    menuserver.cc \
    notifierhost.cc \
    stageclock.cc \
    stagehost.cc \
    statusnotifier/dbustypes.cpp \
    statusnotifier/sniasync.cpp \
    statusnotifier/statusnotifierbutton.cpp \
    statusnotifier/statusnotifieriteminterface.cpp \
    statusnotifier/statusnotifierproxy.cpp \
    statusnotifier/statusnotifierwatcher.cpp \
    surfacemanager.cc \
    taskbutton.cc \
    ../shared/upower.cc \
    wndmgmt.cc

HEADERS += \
    app.h \
    battery.h \
    dbusmenu/dbusmenu_interface.h \
    dbusmenu/dbusmenuimporter.h \
    dbusmenu/dbusmenuimporter_p.h \
    dbusmenu/dbusmenushortcut_p.h \
    dbusmenu/dbusmenutypes_p.h \
    dbusmenu/menuimporter.h \
    dbusmenu/utils.h \
    menuserver.h \
    notifierhost.h \
    stageclock.h \
    stagehost.h \
    statusnotifier/dbustypes.h \
    statusnotifier/sniasync.h \
    statusnotifier/statusnotifierbutton.h \
    statusnotifier/statusnotifieriteminterface.h \
    statusnotifier/statusnotifierproxy.h \
    statusnotifier/statusnotifierwatcher.h \
    surfacemanager.h \
    taskbutton.h \
    ../shared/upower.h \
    wndmgmt.h

# Default rules for deployment.
unix:!android: target.path = /usr/libexec/hollywood
!isEmpty(target.path): INSTALLS += target
