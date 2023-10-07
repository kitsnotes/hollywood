#include "client/privateprotocol.h"
#include "client/privateprotocol_p.h"

#include "qwayland-originull-privateapi.h"

HWPrivateWaylandProtocol::HWPrivateWaylandProtocol()
    :QWaylandClientExtensionTemplate(1)
    ,QtWayland::org_originull_privateapi() {}

OriginullMenuServerClient* HWPrivateWaylandProtocol::createMenuServerResponder()
{
    if(!isActive())
        return nullptr;
    auto obj = provision_menu_server();
    auto mp = new OriginullMenuServerClient(obj);
    return mp;
}

void HWPrivateWaylandProtocol::rotateWallpaper()
{
    rotate_wallpaper();
}

void HWPrivateWaylandProtocol::registerDesktopSurface(QWindow *window)
{
    void *surface = QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window);
    register_desktop(static_cast<struct ::wl_surface *>(surface));
}

OriginullMenuServerClient::OriginullMenuServerClient(struct ::org_originull_menuserver *menu)
    : QWaylandClientExtensionTemplate(1)
    , QtWayland::org_originull_menuserver(menu) {}


void OriginullMenuServerClient::org_originull_menuserver_appmenu_top_level_window_changed(const QString &service_name, const QString &object_path)
{
    emit menuChanged(service_name, object_path);
}
