QT = core dbus
CONFIG += c++17 cmdline link_pkgconfig
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
include(../include/global.pri)
CONFIG(debug): DESTDIR=$${OBJECTS_DIR}../output/
PKGCONFIG += apk-qt
TARGET = updated

HEADERS += \
    apkd.h

SOURCES += \
        apkd.cc

DBUS_ADAPTORS += org.originull.updated.xml
DBUS_INTERFACES	+= org.originull.updated.xml

QMAKE_SUBSTITUTES += org.originull.updated.service.in
service.path = $$PREFIX/share/dbus-1/system-services
service.files = org.originull.updated.service

conf.path = $$PREFIX/share/dbus-1/system.d
conf.files = org.originull.updated.conf

target.path = $$PREFIX/libexec/hollywood/

INSTALLS += service conf target

DISTFILES += \
    org.originull.updated.conf \
    org.originull.updated.service.in \
    org.originull.updated.xml

