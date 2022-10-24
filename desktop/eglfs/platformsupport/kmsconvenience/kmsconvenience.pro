TARGET = QtKmsSupport
TEMPLATE=lib

INCLUDEPATH += /usr/include/libdrm $$PWD/hollywood

QT = core-private gui-private
CONFIG += static 

DEFINES += QT_NO_CAST_FROM_ASCII

HEADERS += \
    hollywood/private/qkmsdevice_p.h

SOURCES += \
    qkmsdevice.cpp

QMAKE_USE += drm
