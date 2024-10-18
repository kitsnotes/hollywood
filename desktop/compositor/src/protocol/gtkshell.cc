// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "gtkshell.h"
#include <QWaylandSurface>

#define GTK_SHELL_VERSION 5

Q_LOGGING_CATEGORY(hwGtkShell, "compositor.gtkshell")

GtkShell::GtkShell(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<GtkShell>(compositor)
    , QtWaylandServer::gtk_shell1(compositor->display(), GTK_SHELL_VERSION) { }

void GtkShell::gtk_shell1_bind_resource(Resource *res)
{
    send_capabilities(res->handle, 0);
}

void GtkShell::gtk_shell1_get_gtk_surface(Resource *res, uint32_t id, wl_resource *sr)
{
    QWaylandSurface *surface = QWaylandSurface::fromResource(sr);
    QWaylandResource gtkSurfaceResource(wl_resource_create(res->client(), &gtk_surface1_interface,
                                                           wl_resource_get_version(res->handle), id));
    emit surfaceRequested(surface, gtkSurfaceResource);

    GtkSurface *gtkSurface = GtkSurface::fromResource(gtkSurfaceResource.resource());
    if (!gtkSurface)
        gtkSurface = new GtkSurface(this, surface, gtkSurfaceResource);

    emit surfaceCreated(gtkSurface);
}

GtkSurface::GtkSurface(GtkShell *shell, QWaylandSurface *surface,
                       const QWaylandResource &resource, QObject *parent)
    :QObject(parent)
{
    initialize(shell, surface, resource);
}

GtkSurface::~GtkSurface() {}

void GtkSurface::initialize(GtkShell *shell, QWaylandSurface *surface, const QWaylandResource &resource)
{
    m_shell = shell;
    m_surface = surface;
    init(resource.resource());
    emit surfaceChanged();
    emit shellChanged();
}

QWaylandSurface *GtkSurface::surface() const
{
    return const_cast<QWaylandSurface*>(m_surface);
}

GtkShell *GtkSurface::shell() const
{
    return const_cast<GtkShell*>(m_shell);
}

QString GtkSurface::appId() const
{
    return m_appId;
}

QString GtkSurface::appMenuPath() const
{
    return m_appMenuPath;
}

QString GtkSurface::menuBarPath() const
{
    return m_menuBarPath;
}

QString GtkSurface::windowObjectPath() const
{
    return m_windowObjectPath;
}

QString GtkSurface::appObjectPath() const
{
    return m_appObjectPath;
}

QString GtkSurface::uniqueBusName() const
{
    return m_uniqueBusName;
}

GtkSurface *GtkSurface::fromResource(wl_resource *wlres)
{
    Resource *res = Resource::fromResource(wlres);
    if (res)
        return static_cast<GtkSurface *>(res->gtk_surface1_object);
    return nullptr;
}

void GtkSurface::gtk_surface1_destroy_resource(Resource *res)
{
    Q_UNUSED(res);
}

void GtkSurface::gtk_surface1_set_dbus_properties(Resource *res, const QString &application_id, const QString &app_menu_path, const QString &menubar_path, const QString &window_object_path, const QString &application_object_path, const QString &unique_bus_name)
{
    Q_UNUSED(res);
    Q_UNUSED(menubar_path);
    m_appId = application_id;
    m_appMenuPath = app_menu_path;
    m_windowObjectPath = window_object_path;
    m_appObjectPath = application_object_path;
    m_uniqueBusName = unique_bus_name;

    emit appIdChanged(m_appId);
    emit appMenuPathChanged(m_appMenuPath);
    emit menuBarPathChanged(m_menuBarPath);
    emit windowObjectPathChanged(m_windowObjectPath);
    emit appObjectPathChanged(m_appObjectPath);
    emit uniqueBusNameChanged(m_uniqueBusName);
}

void GtkSurface::gtk_surface1_set_modal(Resource *res)
{
    Q_UNUSED(res);
    emit setModal();
}

void GtkSurface::gtk_surface1_unset_modal(Resource *res)
{
    Q_UNUSED(res);
    emit unsetModal();
}
