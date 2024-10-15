QT       += core gui widgets help dbus
CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
TARGET = hwhelp
include(../include/global.pri)
CONFIG(debug): DESTDIR=$${OBJECTS_DIR}../output/

include(qlitehtml/src/qlitehtml.pri)

SOURCES += \
    application.cc \
    helpwindow.cc

HEADERS += \
    application.h \
    helpwindow.h

LIBS += -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION}
# Default rules for deployment.
QMAKE_SUBSTITUTES +=org.originull.help.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.help.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop

DISTFILES += \
org.originull.help.desktop.in
