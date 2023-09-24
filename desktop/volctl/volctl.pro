include(../include/global.pri)
QT       += core gui widgets
CONFIG += c++17 link_pkgconfig
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
PKGCONFIG += glib-2.0 libpulse libpulse-mainloop-glib
INCLUDEPATH += /usr/include/glib-2.0 /usr/include/glib-2.0/include
INCLUDEPATH += ../libpavu
INCLUDEPATH += ../libpavu

SOURCES += \
    main.cpp

LIBS += -L../output -lpavu-$${HOLLYWOOD_APIVERSION}

QMAKE_SUBSTITUTES +=org.originull.volctl.desktop.in
desktop.path = $$PREFIX/share/applications
desktop.files = org.originull.volctl.desktop
target.path = $$PREFIX/bin

INSTALLS += target desktop
