include(../../include/global.pri)
TEMPLATE = lib
QT += network core5compat

CONFIG += link_pkgconfig
TARGET=56-efiboot

INCLUDEPATH += /usr/include/efivar

PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1
SOURCES += \
    bootentry.cc \
    bootentrylistmodel.cc \
    efivar-lite.c \
    efivar-lite.linux.c \
    startup.cc

HEADERS += \
    bootentry.h \
    bootentrylistmodel.h \
    efiboot.h \
    efivar-lite.common.h \
    efivar-lite/efiboot-loadopt.h \
    efivar-lite/efivar-dp.h \
    efivar-lite/efivar.h \
    startup.h

LIBS += -lefiboot -lefivar
   # QMAKE_SUBSTITUTES += org.originull.hwsettings.datetime.policy.in
    #polkit.path = $$PREFIX/share/polkit-1/actions/
   # polkit.files = org.originull.hwsettings.datetime.policy
    target.path = $$PREFIX/libexec/hollywood/settings
    INSTALLS += target #polkit

DISTFILES += \
    applet-efiboot.json
