include(../../include/global-applet.pri)

QT += core5compat dbus

INCLUDEPATH += ../../include
INCLUDEPATH += ../../libcommdlg

CONFIG(debug): LIBS += -L../../output -lcommdlg-$${HOLLYWOOD_APIVERSION}
CONFIG(release): LIBS += -lcommdlg-$${HOLLYWOOD_APIVERSION}

TEMPLATE = lib
TARGET=08-regionlaguage

SOURCES += \
    regionapplet.cc

HEADERS += \
    regionapplet.h

DISTFILES += applet.json

target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target

RESOURCES +=
