// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "appmenu.h"
#include "surfaceobject.h"
#include "compositor.h"

#include <QWaylandSurface>
#include <QDebug>

#define MENU_PROTO_VERSION       1

AppMenuManager::AppMenuManager(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::org_kde_kwin_appmenu_manager(compositor->display(), MENU_PROTO_VERSION)
    , m_compositor(qobject_cast<Compositor*>(compositor)) {}

void AppMenuManager::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), MENU_PROTO_VERSION);
}

void AppMenuManager::org_kde_kwin_appmenu_manager_create(Resource *resource, uint32_t id, wl_resource *surface)
{
    auto qsurface = QWaylandSurface::fromResource(surface);
    Surface *s  = nullptr;
    for(auto i : m_compositor->surfaces())
    {
        if(i->surface() == qsurface)
        {
            s=i;
            break;
        }
    }
    if (!s) {
        wl_resource_post_error(resource->handle, 0, "Invalid surface");
        return;
    }

    wl_resource *appmenu_resource = wl_resource_create(resource->client(),
                &org_kde_kwin_appmenu_interface, resource->version(), id);
    if (!appmenu_resource) {
        wl_client_post_no_memory(resource->client());
        return;
    }

    auto appmenu = new AppMenu(s, appmenu_resource, this);
    m_appmenus.append(appmenu);
    QObject::connect(appmenu, &QObject::destroyed, this,[=]() {
        m_appmenus.removeOne(appmenu);
    });
    emit appMenuCreated(appmenu);
}


AppMenu::AppMenu(Surface *s, wl_resource *resource, AppMenuManager *parent)
    : QObject(parent)
    , QtWaylandServer::org_kde_kwin_appmenu(resource)
    , m_parent(parent)
    , m_surface(s) { }

QString AppMenu::serviceName() const
{
    return m_servicename;
}

QString AppMenu::objectPath() const
{
    return m_objectpath;
}

Surface *AppMenu::surface() const
{
    return m_surface;
}

void AppMenu::org_kde_kwin_appmenu_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    deleteLater();
}

void AppMenu::org_kde_kwin_appmenu_set_address(Resource *resource, const QString &service_name, const QString &object_path)
{
    Q_UNUSED(resource);
    qDebug() << "server received MenuServer Request" << service_name << object_path;
    if (m_servicename == service_name && m_objectpath == object_path)
        return;


    m_servicename = service_name;
    m_objectpath = object_path;
    emit addressChanged(m_servicename, m_objectpath);
}

void AppMenu::org_kde_kwin_appmenu_release(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}
