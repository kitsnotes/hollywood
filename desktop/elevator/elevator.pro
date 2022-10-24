include(../include/global.pri)

CONFIG += link_pkgconfig
TARGET=elevator

PKGCONFIG += glib-2.0 polkit-agent-1
versionAtLeast(QT_VERSION, 6.0.0) {
    PKGCONFIG += polkit-qt6-1 polkit-qt6-agent-1 polkit-qt6-gui-1
} else {
    PKGCONFIG += polkit-qt5-1 polkit-qt5-agent-1 polkit-qt5-gui-1
}

SOURCES += \
    agent.cc \
    gui.cc \
    main.cc

HEADERS += \
    agent.h \
    gui.h

QMAKE_SUBSTITUTES += org.originull.elevator.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.elevator.desktop
target.path = $$PREFIX/libexec/hollywood/
INSTALLS += target desktop

