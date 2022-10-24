include(../include/global.pri)
QT += network dbus
TEMPLATE = lib
TARGET = cmctl-$${HOLLYWOOD_APIVERSION}
CONFIG += compile_libtool create_libtool creaate_pc create_prl no_install_prl

DBUS_ADAPTORS 	+= org.originull.netagent.xml
DBUS_INTERFACES	+= org.originull.netagent.xml

HEADERS += \
    agent.h \
    config_dialog.h \
    model.h \
    connman.h \
    libcmctl_global.h \
    service.h \
    technology.h \
    wifimenu.h \
    private/agent_p.h \
    private/config_dialog_p.h \
    private/connman_p.h \
    private/service_p.h \
    private/technology_p.h \
    private/wifi_dialog_p.h \
    private/wifimenu_p.h

FORMS += \
    ui/agent.ui \
    ui/captive_portal_launcher.ui \
    ui/peditor.ui \
    ui/wifi_dialog.ui

SOURCES += \
    agent.cc \
    config_dialog.cc \
    connman.cc \
    model.cc \
    service.cc \
    technology.cc \
    wifi_dialog.cc \
    wifimenu.cc

DISTFILES += \
    org.originull.netagent.xml

headers.files = $${HEADERS}
headers.path = $$PREFIX/include/hollywood/
target.path = $$PREFIX/lib
QMAKE_PKGCONFIG_FILE = $${TARGET}
QMAKE_PKGCONFIG_NAME = $${TARGET}
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESCRIPTION = Hollywood Connman Control Library
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMKE_PKGCONFIG_PREFIX = $${PREFIX}

INSTALLS += target headers
