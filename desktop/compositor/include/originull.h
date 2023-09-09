// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef ORIGINULL_H
#define ORIGINULL_H

#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandCompositor>

#include "qwayland-server-originull-privateapi.h"
#include <QWaylandResource>

class Compositor;
class Surface;
class OriginullMenuServer;
class OriginullProtocol : public QWaylandCompositorExtensionTemplate<OriginullProtocol>,
                          public QtWaylandServer::org_originull_privateapi
{
    Q_OBJECT
public:
    OriginullProtocol(Compositor *compositor);
    void initialize();
signals:
    void menuServerSet(OriginullMenuServer *menu);
    void wallpaperRotationRequested();
protected:
    void org_originull_privateapi_provision_menu_server(Resource *resource, uint32_t id) override;
    void org_originull_privateapi_rotate_wallpaper(Resource *resource) override;
private:
    Compositor *m_compositor = nullptr;
    OriginullMenuServer *m_menu = nullptr;
};

class OriginullMenuServer : public QObject
                          , public QtWaylandServer::org_originull_menuserver
{
    Q_OBJECT
public:
    OriginullMenuServer(struct ::wl_client *client, uint32_t id, OriginullProtocol *proto, Compositor *c);
    void initialize();
    void setTopWindowForMenuServer(Surface* surface);
signals:
    void menuServerDestroyed();
protected:
    void org_originull_menuserver_destroy_resource(Resource *resource) override;
private:
    Compositor *m_compositor;
    Surface *m_parent;
};

#endif // ORIGINULL_H
