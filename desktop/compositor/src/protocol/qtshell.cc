// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "qtshell.h"
#include <QWaylandSurface>

#define QT_SHELL_VERSION 1

Q_LOGGING_CATEGORY(hwQtShell, "compositor.qtshell")

QtShell::QtShell(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<QtShell>(compositor)
    , QtWaylandServer::zqt_shell_v1(compositor->display(), QT_SHELL_VERSION)
{}

void QtShell::zqt_shell_v1_surface_create(Resource *res, wl_resource *sr, uint32_t id)
{
    QWaylandSurface *surface = QWaylandSurface::fromResource(sr);
    QWaylandResource qtSurfaceResource(wl_resource_create(res->client(), &zqt_shell_surface_v1_interface,
                                                           wl_resource_get_version(res->handle), id));
    emit surfaceRequested(surface, qtSurfaceResource);

    QtSurface *qtsurface = QtSurface::fromResource(qtSurfaceResource.resource());
    if (!qtsurface)
        qtsurface = new QtSurface(this, surface, qtSurfaceResource);

    surface->setRole(QtSurface::role(), res->handle, 0);

    emit surfaceCreated(qtsurface);
}

QtSurface::QtSurface(QtShell *shell, QWaylandSurface *surface, const QWaylandResource &resource, QObject *parent)
    :QObject(parent)
{
    initialize(shell, surface, resource);
}

QtSurface::~QtSurface() {}

void QtSurface::initialize(QtShell *shell, QWaylandSurface *surface, const QWaylandResource &resource)
{
    m_shell = shell;
    m_surface = surface;
    init(resource.resource());
    send_set_capabilities(0);
    emit surfaceChanged();
    emit shellChanged();
}

QWaylandSurface *QtSurface::surface() const
{
    return const_cast<QWaylandSurface*>(m_surface);
}

 QtShell* QtSurface::shell() const
{
    return const_cast<QtShell*>(m_shell);
}

QtSurface *QtSurface::fromResource(wl_resource *wlres)
{
    Resource *res = Resource::fromResource(wlres);
    if (res)
        return static_cast<QtSurface *>(res->zqt_shell_surface_v1_object);
    return nullptr;
}

QWaylandSurfaceRole *QtSurface::role()
{
    return &s_role;
}

void QtSurface::zqt_shell_surface_v1_reposition(Resource *resource, int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    emit reposition(QPoint(x,y));
}

void QtSurface::zqt_shell_surface_v1_request_activate(Resource *resource)
{
    Q_UNUSED(resource);
    emit requestActivate();
}

void QtSurface::zqt_shell_surface_v1_set_size(Resource *resource, int32_t width, int32_t height)
{
    Q_UNUSED(resource);
    emit setSize(QSize(width,height));
}

void QtSurface::zqt_shell_surface_v1_set_minimum_size(Resource *resource, int32_t width, int32_t height)
{
    Q_UNUSED(resource);
    Q_UNUSED(width);
    Q_UNUSED(height);
}

void QtSurface::zqt_shell_surface_v1_set_maximum_size(Resource *resource, int32_t width, int32_t height)
{
    Q_UNUSED(resource);
    Q_UNUSED(width);
    Q_UNUSED(height);
}

void QtSurface::zqt_shell_surface_v1_set_window_title(Resource *resource, const QString &window_title)
{
    Q_UNUSED(resource);
    emit windowTitleChanged(window_title);
}

void QtSurface::zqt_shell_surface_v1_set_window_flags(Resource *resource, uint32_t flags)
{
    Q_UNUSED(resource);
    emit windowFlagsChanged((Qt::WindowFlags) flags);
}

void QtSurface::zqt_shell_surface_v1_start_system_resize(Resource *resource, uint32_t serial, uint32_t edge)
{
    Q_UNUSED(resource);
    // TODO: implement server side resize
}

void QtSurface::zqt_shell_surface_v1_start_system_move(Resource *resource, uint32_t serial)
{
    Q_UNUSED(resource);
    // TODO: implement server side move
}

void QtSurface::zqt_shell_surface_v1_change_window_state(Resource *resource, uint32_t state)
{
    Q_UNUSED(resource);

}

void QtSurface::zqt_shell_surface_v1_raise(Resource *resource)
{
    Q_UNUSED(resource);
    emit requestRaise();
}

void QtSurface::zqt_shell_surface_v1_lower(Resource *resource)
{
    Q_UNUSED(resource);
    emit requestLower();
}

void QtSurface::zqt_shell_surface_v1_ack_configure(Resource *resource, uint32_t serial)
{

}

QWaylandSurfaceRole QtSurface::s_role("zqt_shell_surface_v1");
