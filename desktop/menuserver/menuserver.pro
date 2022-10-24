include(../include/global.pri)


QT       += dbus waylandclient widgets waylandclient-private gui-private concurrent
CONFIG += c++11
CONFIG += wayland-scanner
WAYLANDCLIENTSOURCES += ../compositor/protocols/originull-privateapi.xml
WAYLANDCLIENTSOURCES += ../compositor/protocols/plasma-window-management.xml
# DBUS_INTERFACES += com.canonical.AppMenu.registrar.xml

TARGET = menuserver
INCLUDEPATH += include/
INCLUDEPATH += ../include/
INCLUDEPATH += ../libcompositor/include
LIBS += -I../libcompositor/include -L../output -lcompositor-$${HOLLYWOOD_APIVERSION}

SOURCES += \
    src/dbusmenuimporter.cpp \
    src/dbusmenushortcut.cc \
    src/dbusmenutypes.cc \
    src/menuimporter.cc \
    src/application.cc \
    src/menuserver.cc \
    src/utils.cc

HEADERS += \
    include/dbusmenu_interface.h \
    include/dbusmenuimporter.h \
    include/dbusmenuimporter_p.h \
    include/dbusmenushortcut_p.h \
    include/dbusmenutypes_p.h \
    include/menuimporter.h \
    include/utils.h \
    include/application.h \
    include/menuserver.h

QMAKE_SUBSTITUTES += org.originull.menuserver.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.menuserver.desktop
target.path = $$PREFIX/usr/libexec/hollywood/

INSTALLS += target desktop

RESOURCES += \
    org.originull.menuserver.desktop.in \
    res/resource.qrc
