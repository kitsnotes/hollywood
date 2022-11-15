TARGET = HWKmsSupport
TEMPLATE=lib
DESTDIR=$${OBJECTS_DIR}../../../output/

QT = core-private gui-private core5compat dbus
CONFIG += internal_module
INCLUDEPATH += /usr/include/libdrm $$PWD/hollywood

QT = core-private gui-private

DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += \
    hollywood/private/qkmsdevice_p.h

SOURCES += \
    qkmsdevice.cpp

QMAKE_USE += drm

target.path = /usr/lib
INSTALLS += target
