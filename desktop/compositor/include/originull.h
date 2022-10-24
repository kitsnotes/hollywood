// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef ORIGINULL_H
#define ORIGINULL_H

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include "qwayland-server-originull-privateapi.h"

class Compositor;
class Surface;
class OriginullProtocol : public QWaylandCompositorExtensionTemplate<OriginullProtocol>,
                          public QtWaylandServer::org_originull_privateapi
{
    Q_OBJECT
public:
    OriginullProtocol(Compositor *compositor);
    void initialize();
    //void setTopWindowForMenuServer(Surface* surface);
signals:
    void menuServerSet(QWaylandSurface *surface);
    void desktopSet(QWaylandSurface *surface);
protected:
    void org_originull_privateapi_provision_desktop_surface(QtWaylandServer::org_originull_privateapi::Resource *resource, wl_resource *wl_surface);
    void org_originull_privateapi_provision_menu_server(QtWaylandServer::org_originull_privateapi::Resource *resource, uint32_t id, wl_resource *wl_surface);
private:
    Compositor *m_compositor;
};

class OriginullMenuServer : public QObject
                          , public QtWaylandServer::org_originull_menuserver
{
    Q_OBJECT
public:
    OriginullMenuServer(Compositor *c, Surface *s, wl_resource *resource);
    void initialize();
    void setTopWindowForMenuServer(Surface* surface);
private:
    Compositor *m_compositor;
    Surface *m_parent;
};

#endif // ORIGINULL_H
