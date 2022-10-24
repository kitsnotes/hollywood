include(../include/global.pri)
TARGET = terminull

versionAtLeast(QT_VERSION, 6.0.0) {
    !contains(QT_CONFIG, no-pkg-config) {
        PKGCONFIG += qtermwidget6
    } else {
        LIBS += -lqtermwidget6
    }
    LIBS += -L../output -lcommdlg-$${HOLLYWOOD_APIVERSION}
    LIBS += -lqtermwidget6
} else {
    LIBS += -L../output -lcommdlg5-$${HOLLYWOOD_APIVERSION}
    LIBS += -lqtermwidget5
}

SOURCES += \
    application.cc \
    preferences.cc \
    profile.cc \
    profiletab.cc \
    tabhost.cc \
    terminal.cc \
    window.cc

HEADERS += \
    application.h \
    preferences.h \
    profile.h \
    profiletab.h \
    tabhost.h \
    terminal.h \
    window.h

INCLUDEPATH += ../libcommdlg/include
QMAKE_SUBSTITUTES += org.originull.terminull.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.terminull.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop
DISTFILES += \
    org.originull.terminull.desktop.in
