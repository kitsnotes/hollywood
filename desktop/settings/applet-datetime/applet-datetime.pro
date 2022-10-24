include(../../include/global-applet.pri)

TEMPLATE = lib
CONFIG += link_pkgconfig
TARGET=50-datetime

PKGCONFIG += polkit-qt6-1 polkit-qt6-gui-1
SOURCES += \
    analogclock.cc \
    dtapplet.cc \
    timezone.cc \
    tzimage.cc \
    tzwidget.cc

HEADERS += \
    analogclock.h \
    dtapplet.h \
    timezone.h \
    tzimage.h \
    tzwidget.h

DISTFILES += \
    applet-dt.json \
    org.originull.hwsettings.datetime.policy.in

QMAKE_SUBSTITUTES += org.originull.hwsettings.datetime.policy.in
polkit.path = $$PREFIX/share/polkit-1/actions/
polkit.files = org.originull.hwsettings.datetime.policy
target.path = $$PREFIX/libexec/hollywood/settings
INSTALLS += target polkit

RESOURCES += \
    tzimages.qrc
