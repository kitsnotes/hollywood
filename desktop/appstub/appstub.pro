include(../include/global.pri)

QT       += core gui widgets
TARGET = appstub
SOURCES += \
    appstub.cc

HEADERS +=

QMAKE_SUBSTITUTES += org.originull.appstub.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.appstub.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop

DISTFILES += \
    org.originull.appstub.desktop.in
