// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "originull.h"
#include "surfaceobject.h"
#include "appmenu.h"
#include "compositor.h"

#include <QWaylandSurface>

OriginullProtocol::OriginullProtocol(Compositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::org_originull_privateapi(compositor->display(), 1)
    , m_compositor(compositor)
{
}
void OriginullProtocol::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void OriginullProtocol::org_originull_privateapi_provision_menu_server(QtWaylandServer::org_originull_privateapi::Resource *resource, uint32_t id, wl_resource *wl_surface)
{
    Q_UNUSED(id);
    auto surface = QWaylandSurface::fromResource(wl_surface);
    Surface *s  = nullptr;
    for(auto i : m_compositor->surfaces())
    {
        if(i->surface() == surface)
        {
            s=i;
            break;
        }
    }
    if (!s) {
        wl_resource_post_error(resource->handle, 0, "Invalid surface");
        return;
    }

    wl_resource *msr = wl_resource_create(resource->client(),
                &org_originull_menuserver_interface, resource->version(), id);
    if (!msr) {
        wl_client_post_no_memory(resource->client());
        return;
    }

    /* QObject::connect(appmenu, &QObject::destroyed, this,[=]() {

    }); */
    Q_EMIT menuServerSet(surface);
}

void OriginullProtocol::org_originull_privateapi_provision_desktop_surface(QtWaylandServer::org_originull_privateapi::Resource *resource, wl_resource *wl_surface)
{
    Q_UNUSED(resource);
    auto surface = QWaylandSurface::fromResource(wl_surface);
    Q_EMIT desktopSet(surface);
}

OriginullMenuServer::OriginullMenuServer(Compositor *c, Surface *s, wl_resource *resource)
                   : QObject(nullptr)
                   , QtWaylandServer::org_originull_menuserver(resource)
                   , m_compositor(c)
                   , m_parent(s) {}

void OriginullMenuServer::setTopWindowForMenuServer(Surface *surface)
{
    if(surface->appMenu() == nullptr)
        return;

    if(!surface->surface()->isInitialized())
        return;

    if(!m_compositor->hasMenuServer())
        return;

    qDebug() << "setTopWindowForMenuServer";
    auto wlSurface = surface->surface()->resource();
    auto serviceName = surface->appMenu()->serviceName();
    auto objectPath = surface->appMenu()->objectPath();

    QtWaylandServer::org_originull_menuserver::
            send_appmenu_top_level_window_changed(wlSurface, serviceName, objectPath);
}
