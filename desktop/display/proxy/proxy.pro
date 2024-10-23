include(../../include/global.pri)

QT-=gui
QT+=dbus waylandclient
TARGET=wlproxy
SOURCES=wlproxy.cc wl-socket.c
HEADERS=wl-socket.h \
    wlproxy.h

contains( DEFINES, BUILD_HOLLYWOOD )
{
    TARGET = wlproxy
    DESTDIR=$${OBJECTS_DIR}../../output/
    INCLUDEPATH+=../libshell/include/
    target.path = $$PREFIX/libexec/hollywood/
}

INSTALLS += target
