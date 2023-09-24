include(../include/global.pri)
QT += gui widgets

TEMPLATE = lib
DEFINES += LIBPAVU_LIBRARY
TARGET = libpavu-$${HOLLYWOOD_APIVERSION}
CONFIG += c++17 link_pkgconfig
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
PKGCONFIG += glib-2.0 libpulse libpulse-mainloop-glib
INCLUDEPATH += $$PWD $$PWD/hollywood/

SOURCES += \
    cardwidget.cc \
    channel.cc \
    devicewidget.cc \
    elidinglabel.cc \
    mainwindow.cc \
    minimalstreamwidget.cc \
    pulsecontrol.cc \
    rolewidget.cc \
    sinkinputwidget.cc \
    sinkwidget.cc \
    sourceoutputwidget.cc \
    sourcewidget.cc \
    streamwidget.cc

HEADERS += \
    cardwidget.h \
    channel.h \
    devicewidget.h \
    elidinglabel.h \
    libpavu_global.h \
    mainwindow.h \
    minimalstreamwidget.h \
    pulsecontrol.h \
    rolewidget.h \
    sinkinputwidget.h \
    sinkwidget.h \
    sourceoutputwidget.h \
    sourcewidget.h \
    streamwidget.h

# Default rules for deployment.
target.path = $$PREFIX/lib
INSTALLS += target

FORMS += \
    cardwidget.ui \
    channelwidget.ui \
    devicewidget.ui \
    mainwindow.ui \
    streamwidget.ui
