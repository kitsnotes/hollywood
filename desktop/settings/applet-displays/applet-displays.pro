include(../../include/global-applet.pri)

TEMPLATE = lib
TARGET=51-displays

SOURCES += \
    displaysapplet.cc \
    screen.cc \
    screenmanager.cc \
    smanwidget.cc

HEADERS += \
    displaysapplet.h \
    screen.h \
    screenmanager.h \
    smanwidget.h

DISTFILES += applet.json
target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
