include(../include/global.pri)
CONFIG += console
CONFIG -= app_bundle
QT -= gui
QT += network dbus

CONFIG(debug, debug|release) {
    LIBS += -L../output -lshelld-$${HOLLYWOOD_APIVERSION}
} else {
    LIBS += -L../output -lshell-$${HOLLYWOOD_APIVERSION}
}

TARGET = session
INCLUDEPATH += ../libshell/include
INCLUDEPATH += ../include
SOURCES += \
        process.cc \
        session.cc

HEADERS += \
    dbus.h \
    process.h \
    session.h

QMAKE_SUBSTITUTES += hollywood.desktop.in
desktop.path = $$PREFIX/share/wayland-sessions
desktop.files = hollywood.desktop
target.path = $$PREFIX/libexec/hollywood/

INSTALLS += target desktop
DISTFILES += \
    hollywood.desktop.in

