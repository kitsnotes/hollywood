include(../include/global.pri)

QT       += core gui widgets charts

INCLUDEPATH += include/
INCLUDEPATH += ../libshell/include
INCLUDEPATH += ../libcommdlg/include
INCLUDEPATH += /usr/include/ApkQt/
CONFIG += link_pkgconfig
TARGET = hwsysmon

PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1 apk-qt
SOURCES += \
    src/application.cc \
    src/chartobject.cc \
    src/cpuoverview.cc \
    src/gaugeobject.cc \
    src/infodialog.cc \
    src/logviewer.cc \
    src/nicedialog.cc \
    src/openrc.cc \
    src/openrcmodel.cc \
    src/overview.cc \
    src/procfsmodel.cc \
    src/ramoverview.cc \
    src/diskoverview.cc \
    src/statpoller.cc \
    src/window.cc

HEADERS += \
    include/application.h \
    include/gaugeobject.h \
    include/infodialog.h \
    include/logviewer.h \
    include/chartobject.h \
    include/cpuoverview.h \
    include/lscpu-arm.h \
    include/nicedialog.h \
    include/openrc.h \
    include/openrcmodel.h \
    include/overview.h \
    include/procfsmodel.h \
    include/ramoverview.h \
    include/diskoverview.h \
    include/statpoller.h \
    include/window.h

LIBS += -L../libshell -L../output -lshell-$${HOLLYWOOD_APIVERSION}
LIBS += -L../libcommdlg -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION}

QMAKE_SUBSTITUTES += org.originull.sysmon.desktop.in \
    org.originull.sysmon.policy.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.sysmon.desktop
target.path = $$PREFIX/bin

polkit.path = $$PREFIX/share/polkit-1/actions/
polkit.files = org.originull.sysmon.policy

INSTALLS += target desktop polkit

DISTFILES += \
    org.originull.sysmon.desktop.in \
    org.originull.sysmon.policy.in
