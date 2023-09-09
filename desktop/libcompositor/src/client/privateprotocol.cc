#include "client/privateprotocol.h"
#include "client/privateprotocol_p.h"

#include "qwayland-originull-privateapi.h"

AIPrivateWaylandProtocol::AIPrivateWaylandProtocol()
    :QWaylandClientExtensionTemplate(1)
    ,QtWayland::org_originull_privateapi()
{

}

static inline struct ::wl_surface *getWlSurface(QWindow *window)
{
    void *surf = QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window);
    return static_cast<struct ::wl_surface *>(surf);
}

OriginullMenuServerClient* AIPrivateWaylandProtocol::createMenuServerResponder()
{
    if(!isActive())
        return nullptr;
    auto obj = provision_menu_server();
    auto mp = new OriginullMenuServerClient(obj);
    return mp;
}

void AIPrivateWaylandProtocol::rotateWallpaper()
{
    rotate_wallpaper();
}

OriginullMenuServerClient::OriginullMenuServerClient(struct ::org_originull_menuserver *menu)
    : QWaylandClientExtensionTemplate(1)
    , QtWayland::org_originull_menuserver(menu) {}


void OriginullMenuServerClient::org_originull_menuserver_appmenu_top_level_window_changed(const QString &service_name, const QString &object_path)
{
    emit menuChanged(service_name, object_path);
}
