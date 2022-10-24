include(../include/global.pri)

QT       += core gui widgets svg opengl
TARGET = hwabout
SOURCES += \
    about.cc

HEADERS += \
    about.h \
    ../sysmon/include/lscpu-arm.h


RESOURCES += \
    res.qrc

QMAKE_SUBSTITUTES += org.originull.about.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.about.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop

DISTFILES += \
    org.originull.sysmon.about.desktop.in
