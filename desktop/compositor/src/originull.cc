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
    init(compositor->display(), 1);
}

void OriginullProtocol::org_originull_privateapi_provision_menu_server(Resource *resource, uint32_t id)
{
    /*if(m_compositor->hasMenuServer())
    {
        wl_resource_post_error(resource->handle, 0, "Menu server already provisioned.");
        return;
    }*/
    OriginullMenuServer* menu = new OriginullMenuServer(resource->client(), id, this, m_compositor);
    m_menu = menu;
    emit menuServerSet(menu);
}

void OriginullProtocol::org_originull_privateapi_rotate_wallpaper(Resource *resource)
{
    Q_UNUSED(resource);
    emit wallpaperRotationRequested();
}

OriginullMenuServer::OriginullMenuServer(struct ::wl_client *client, uint32_t id, OriginullProtocol *proto, Compositor *c)
                   : QObject(nullptr)
                   , QtWaylandServer::org_originull_menuserver(client, id, 1)
                   , m_compositor(c)
{
    Q_UNUSED(proto)
}

void OriginullMenuServer::setTopWindowForMenuServer(Surface *surface)
{
    if(!surface || !m_compositor->hasMenuServer())
        return;

    if(!surface->surface() || !surface->surface()->isInitialized())
        return;

    if(surface->appMenu() == nullptr)
        return;

    auto serviceName = surface->appMenu()->serviceName();
    auto objectPath = surface->appMenu()->objectPath();

    send_appmenu_top_level_window_changed(serviceName, objectPath);
    for (auto r : resourceMap())
        send_appmenu_top_level_window_changed(r->handle, serviceName, objectPath);
}

void OriginullMenuServer::org_originull_menuserver_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource)
    emit menuServerDestroyed();
}
