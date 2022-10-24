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

OriginullMenuServerClient* AIPrivateWaylandProtocol::setMenuServerForWindow(QWindow *window)
{
    if(!isActive())
        return nullptr;
    if (window->handle())
    {
        qDebug() << "set MenuServer";
        wl_surface *nativeSurface = getWlSurface(window);
        auto wlms = QtWayland::org_originull_privateapi::provision_menu_server(nativeSurface);
        auto mp = new OriginullMenuServerClient(wlms);
        return mp;
    }

    return nullptr;
}

void AIPrivateWaylandProtocol::setDesktopForWindow(QWindow *window)
{
    if(!isActive())
        return;
    if (window->handle())
    {
        wl_surface *nativeSurface = getWlSurface(window);
        QtWayland::org_originull_privateapi::provision_desktop_surface(nativeSurface);
    }
}

OriginullMenuServerClient::OriginullMenuServerClient(struct ::org_originull_menuserver *window)
    : QWaylandClientExtensionTemplate(1)
    , QtWayland::org_originull_menuserver(window)
{

}

void OriginullMenuServerClient::org_originull_menuserver_appmenu_top_level_window_changed(wl_surface *surface, const QString &service_name, const QString &object_path)
{
    Q_UNUSED(surface);
    qDebug() << "menuChanged" << service_name << object_path;
    emit menuChanged(service_name, object_path);
}
