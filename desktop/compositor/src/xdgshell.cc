// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "xdgshell.h"
#include "xdgshell_p.h"
#include "layershell.h"

#include <QtWaylandCompositor/private/qwaylandutils_p.h>

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandSurfaceRole>
#include <QtWaylandCompositor/QWaylandResource>

#include <QtCore/QObject>

#include <algorithm>

QT_BEGIN_NAMESPACE

HWWaylandXdgShellPrivate::HWWaylandXdgShellPrivate()
{
}

void HWWaylandXdgShellPrivate::ping(QtWaylandServer::xdg_wm_base::Resource *resource, uint32_t serial)
{
    m_pings.insert(serial);
    send_ping(resource->handle, serial);
}

void HWWaylandXdgShellPrivate::registerXdgSurface(HWWaylandXdgSurface *xdgSurface)
{
    m_xdgSurfaces.insert(xdgSurface->surface()->client()->client(), xdgSurface);
}

void HWWaylandXdgShellPrivate::unregisterXdgSurface(HWWaylandXdgSurface *xdgSurface)
{
    auto xdgSurfacePrivate = HWWaylandXdgSurfacePrivate::get(xdgSurface);
    if (!m_xdgSurfaces.remove(xdgSurfacePrivate->resource()->client(), xdgSurface))
        qWarning("%s Unexpected state. Can't find registered xdg surface\n", Q_FUNC_INFO);
}

HWWaylandXdgSurface *HWWaylandXdgShellPrivate::xdgSurfaceFromSurface(QWaylandSurface *surface)
{
    for (HWWaylandXdgSurface *xdgSurface : qAsConst(m_xdgSurfaces)) {
        if (surface == xdgSurface->surface())
            return xdgSurface;
    }
    return nullptr;
}

void HWWaylandXdgShellPrivate::xdg_wm_base_destroy(Resource *resource)
{
    if (!m_xdgSurfaces.values(resource->client()).empty())
        wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_DEFUNCT_SURFACES,
                               "xdg_shell was destroyed before children");

    wl_resource_destroy(resource->handle);
}

void HWWaylandXdgShellPrivate::xdg_wm_base_create_positioner(QtWaylandServer::xdg_wm_base::Resource *resource, uint32_t id)
{
    QWaylandResource positionerResource(wl_resource_create(resource->client(), &xdg_positioner_interface,
                                                           wl_resource_get_version(resource->handle), id));

    new QWaylandXdgPositioner(positionerResource);
}

void HWWaylandXdgShellPrivate::xdg_wm_base_get_xdg_surface(Resource *resource, uint32_t id, wl_resource *surfaceResource)
{
    Q_Q(HWWaylandXdgShell);
    QWaylandSurface *surface = QWaylandSurface::fromResource(surfaceResource);

    if (surface->role() != nullptr) {
        wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_ROLE,
                               "wl_surface@%d, already has role %s\n",
                               wl_resource_get_id(surface->resource()),
                               surface->role()->name().constData());
        return;
    }

    if (surface->hasContent()) {
        //TODO: According to the spec, this is a client error, but there's no appropriate error code
        qWarning() << "get_xdg_surface requested on a xdg_surface with content";
    }

    QWaylandResource xdgSurfaceResource(wl_resource_create(resource->client(), &xdg_surface_interface,
                                                           wl_resource_get_version(resource->handle), id));

    HWWaylandXdgSurface *xdgSurface = new HWWaylandXdgSurface(q, surface, xdgSurfaceResource);

    registerXdgSurface(xdgSurface);
    emit q->xdgSurfaceCreated(xdgSurface);
}

void HWWaylandXdgShellPrivate::xdg_wm_base_pong(Resource *resource, uint32_t serial)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgShell);
    if (m_pings.remove(serial))
        emit q->pong(serial);
    else
        qWarning("Received an unexpected pong!");
}

/*!
 * \qmltype XdgShell
 * \instantiates HWWaylandXdgShell
 * \inqmlmodule QtWayland.Compositor.XdgShell
 * \since 5.12
 * \brief Provides an extension for desktop-style user interfaces.
 *
 * The XdgShell extension provides a way to associate a XdgToplevel or XdgPopup
 * with a regular Wayland surface. Using the XdgToplevel interface, the client
 * can request that the surface is resized, moved, and so on.
 *
 * XdgShell corresponds to the Wayland interface, \c xdg_shell.
 *
 * To provide the functionality of the shell extension in a compositor, create
 * an instance of the XdgShell component and add it to the list of extensions
 * supported by the compositor:
 *
 * \qml
 * import QtWayland.Compositor.XdgShell
 *
 * WaylandCompositor {
 *     XdgShell {
 *         // ...
 *     }
 * }
 * \endqml
 */

/*!
 * \class HWWaylandXdgShell
 * \inmodule QtWaylandCompositor
 * \since 5.12
 * \brief The HWWaylandXdgShell class is an extension for desktop-style user interfaces.
 *
 * The HWWaylandXdgShell extension provides a way to associate a HWWaylandXdgToplevel or
 * HWWaylandXdgPopup with a regular Wayland surface. Using the HWWaylandXdgToplevel interface,
 * the client can request that the surface is resized, moved, and so on.
 *
 * HWWaylandXdgShell corresponds to the Wayland interface, \c xdg_shell.
 */

/*!
 * Constructs a HWWaylandXdgShell object.
 */
HWWaylandXdgShell::HWWaylandXdgShell()
    : QWaylandShellTemplate<HWWaylandXdgShell>(*new HWWaylandXdgShellPrivate())
{
}

/*!
 * Constructs a HWWaylandXdgShell object for the provided \a compositor.
 */
HWWaylandXdgShell::HWWaylandXdgShell(QWaylandCompositor *compositor)
    : QWaylandShellTemplate<HWWaylandXdgShell>(compositor, *new HWWaylandXdgShellPrivate())
{
}

/*!
 * Initializes the shell extension.
 */
void HWWaylandXdgShell::initialize()
{
    Q_D(HWWaylandXdgShell);
    QWaylandShellTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qWarning() << "Failed to find QWaylandCompositor when initializing HWWaylandXdgShell";
        return;
    }
    d->init(compositor->display(), 1);

    handleSeatChanged(compositor->defaultSeat(), nullptr);

    connect(compositor, &QWaylandCompositor::defaultSeatChanged,
            this, &HWWaylandXdgShell::handleSeatChanged);
}

/*!
 * Returns the Wayland interface for the HWWaylandXdgShell.
 */
const struct wl_interface *HWWaylandXdgShell::interface()
{
    return HWWaylandXdgShellPrivate::interface();
}

QByteArray HWWaylandXdgShell::interfaceName()
{
    return HWWaylandXdgShellPrivate::interfaceName();
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgShell::ping(WaylandClient client)
 *
 * Sends a ping event to \a client. If the client replies to the event the
 * \l pong signal will be emitted.
 */

/*!
 * Sends a ping event to \a client. If the client replies to the event the
 * \l pong signal will be emitted.
 */
uint HWWaylandXdgShell::ping(QWaylandClient *client)
{
    Q_D(HWWaylandXdgShell);

    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    Q_ASSERT(compositor);

    uint32_t serial = compositor->nextSerial();

    HWWaylandXdgShellPrivate::Resource *clientResource = d->resourceMap().value(client->client(), nullptr);
    Q_ASSERT(clientResource);

    d->ping(clientResource, serial);
    return serial;
}

void HWWaylandXdgShell::handleSeatChanged(QWaylandSeat *newSeat, QWaylandSeat *oldSeat)
{
    if (oldSeat != nullptr) {
        disconnect(oldSeat, &QWaylandSeat::keyboardFocusChanged,
                   this, &HWWaylandXdgShell::handleFocusChanged);
    }

    if (newSeat != nullptr) {
        connect(newSeat, &QWaylandSeat::keyboardFocusChanged,
                this, &HWWaylandXdgShell::handleFocusChanged);
    }
}

void HWWaylandXdgShell::handleFocusChanged(QWaylandSurface *newSurface, QWaylandSurface *oldSurface)
{
    Q_D(HWWaylandXdgShell);

    HWWaylandXdgSurface *newXdgSurface = d->xdgSurfaceFromSurface(newSurface);
    HWWaylandXdgSurface *oldXdgSurface = d->xdgSurfaceFromSurface(oldSurface);

    if (newXdgSurface)
        HWWaylandXdgSurfacePrivate::get(newXdgSurface)->handleFocusReceived();

    if (oldXdgSurface)
        HWWaylandXdgSurfacePrivate::get(oldXdgSurface)->handleFocusLost();
}

HWWaylandXdgSurfacePrivate::HWWaylandXdgSurfacePrivate()
{
}

void HWWaylandXdgSurfacePrivate::setWindowType(Qt::WindowType windowType)
{
    if (m_windowType == windowType)
        return;

    m_windowType = windowType;

    Q_Q(HWWaylandXdgSurface);
    emit q->windowTypeChanged();
}

void HWWaylandXdgSurfacePrivate::handleFocusLost()
{
    if (m_toplevel)
        HWWaylandXdgToplevelPrivate::get(m_toplevel)->handleFocusLost();
}

void HWWaylandXdgSurfacePrivate::handleFocusReceived()
{
    if (m_toplevel)
        HWWaylandXdgToplevelPrivate::get(m_toplevel)->handleFocusReceived();
}

QRect HWWaylandXdgSurfacePrivate::calculateFallbackWindowGeometry() const
{
    // TODO: The unset window geometry should include subsurfaces as well, so this solution
    // won't work too well on those kinds of clients.
    return QRect(QPoint(), m_surface->destinationSize());
}

void HWWaylandXdgSurfacePrivate::updateFallbackWindowGeometry()
{
    Q_Q(HWWaylandXdgSurface);
    if (!m_unsetWindowGeometry)
        return;

    const QRect unsetGeometry = calculateFallbackWindowGeometry();
    if (unsetGeometry == m_windowGeometry)
        return;

    m_windowGeometry = unsetGeometry;
    emit q->windowGeometryChanged();
}

void HWWaylandXdgSurfacePrivate::xdg_surface_destroy_resource(QtWaylandServer::xdg_surface::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgSurface);
    HWWaylandXdgShellPrivate::get(m_xdgShell)->unregisterXdgSurface(q);
    delete q;
}

void HWWaylandXdgSurfacePrivate::xdg_surface_destroy(QtWaylandServer::xdg_surface::Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

void HWWaylandXdgSurfacePrivate::xdg_surface_get_toplevel(QtWaylandServer::xdg_surface::Resource *resource, uint32_t id)
{
    Q_Q(HWWaylandXdgSurface);

    if (m_toplevel || m_popup) {
        wl_resource_post_error(resource->handle, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,
                               "xdg_surface already has a role object");
        return;
    }

    if (!m_surface->setRole(HWWaylandXdgToplevel::role(), resource->handle, XDG_WM_BASE_ERROR_ROLE))
        return;

    QWaylandResource topLevelResource(wl_resource_create(resource->client(), &xdg_toplevel_interface,
                                                         wl_resource_get_version(resource->handle), id));

    m_toplevel = new HWWaylandXdgToplevel(q, topLevelResource);
    emit q->toplevelCreated();
    emit m_xdgShell->toplevelCreated(m_toplevel, q);
}

void HWWaylandXdgSurfacePrivate::xdg_surface_get_popup(QtWaylandServer::xdg_surface::Resource *resource, uint32_t id, wl_resource *parentResource, wl_resource *positionerResource)
{
    Q_Q(HWWaylandXdgSurface);

    if (m_toplevel || m_popup) {
        wl_resource_post_error(resource->handle, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,
                               "xdg_surface already has a role object");
        return;
    }

    HWWaylandXdgSurface *parent = HWWaylandXdgSurface::fromResource(parentResource);
    WlrLayerSurfaceV1 *ls_parent  = nullptr;
    if(!parent)
    {
        ls_parent = WlrLayerShellV1::fromResource(parentResource);
        if (!ls_parent) {
            //wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT,
            //                       "xdg_surface.get_popup 2 with invalid popup parent");
            //return;
        }
    }

    QWaylandXdgPositioner *positioner = QWaylandXdgPositioner::fromResource(positionerResource);
    if (!positioner) {
        wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_INVALID_POSITIONER,
                               "xdg_surface.get_popup without positioner");
        return;
    }

    if (!positioner->m_data.isComplete()) {
        QWaylandXdgPositionerData p = positioner->m_data;
        wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_INVALID_POSITIONER,
                               "xdg_surface.get_popup with invalid positioner (size: %dx%d, anchorRect: %dx%d)",
                               p.size.width(), p.size.height(), p.anchorRect.width(), p.anchorRect.height());
        return;
    }

    QSize parentSize;
    if(parent)
    {
        parentSize = parent->windowGeometry().size();

        QRect anchorBounds(QPoint(0, 0), parentSize);
        if (!anchorBounds.contains(positioner->m_data.anchorRect)) {
            // TODO: this is a protocol error and should ideally be handled like this:
            //wl_resource_post_error(resource->handle, XDG_WM_BASE_ERROR_INVALID_POSITIONER,
            //                       "xdg_positioner anchor rect extends beyound its parent's window geometry");
            //return;
            // However, our own clients currently do this, so we'll settle for a gentle warning instead.
            qCWarning(qLcWaylandCompositor) << "Ignoring client protocol error: xdg_positioner anchor"
                                            << "rect extends beyond its parent's window geometry";
        }
    }

    if (!m_surface->setRole(HWWaylandXdgPopup::role(), resource->handle, XDG_WM_BASE_ERROR_ROLE))
        return;

    QWaylandResource popupResource(wl_resource_create(resource->client(), &xdg_popup_interface,
                                                      wl_resource_get_version(resource->handle), id));

    m_popup = new HWWaylandXdgPopup(q, parent, positioner, popupResource);
    emit q->popupCreated();
    emit m_xdgShell->popupCreated(m_popup, q);
}

void HWWaylandXdgSurfacePrivate::xdg_surface_ack_configure(QtWaylandServer::xdg_surface::Resource *resource, uint32_t serial)
{
    if (m_toplevel) {
        HWWaylandXdgToplevelPrivate::get(m_toplevel)->handleAckConfigure(serial);
    } else if (m_popup) {
        HWWaylandXdgPopupPrivate::get(m_popup)->handleAckConfigure(serial);
    } else {
        wl_resource_post_error(resource->handle, XDG_SURFACE_ERROR_NOT_CONSTRUCTED,
                               "ack_configure requested on an unconstructed xdg_surface");
    }
}

void HWWaylandXdgSurfacePrivate::xdg_surface_set_window_geometry(QtWaylandServer::xdg_surface::Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{
    Q_Q(HWWaylandXdgSurface);

    if (!q->surface()->role()) {
        wl_resource_post_error(resource->handle, XDG_SURFACE_ERROR_NOT_CONSTRUCTED,
                               "set_window_geometry requested on an unconstructed xdg_surface");
        return;
    }

    if (width <= 0 || height <= 0) {
        // The protocol spec says "setting an invalid size will raise an error". But doesn't tell
        // which error to raise, and there's no fitting error in the xdg_surface_error enum.
        // So until this is fixed, just output a warning and return.
        qWarning() << "Invalid (non-positive) dimensions received in set_window_geometry";
        return;
    }

    m_unsetWindowGeometry = false;

    QRect geometry(x, y, width, height);

    if (m_windowGeometry == geometry)
        return;

    m_windowGeometry = geometry;
    emit q->windowGeometryChanged();
}

/*!
 * \qmltype XdgSurface
 * \instantiates HWWaylandXdgSurface
 * \inqmlmodule QtWayland.Compositor.XdgShell
 * \since 5.12
 * \brief XdgSurface provides desktop-style compositor-specific features to an xdg surface.
 *
 * This type is part of the \l{XdgShell} extension and provides a way to
 * extend the functionality of an existing \l{WaylandSurface} with features
 * specific to desktop-style compositors, such as resizing and moving the
 * surface.
 *
 * It corresponds to the Wayland interface \c xdg_surface.
 */

/*!
 * \class HWWaylandXdgSurface
 * \inmodule QtWaylandCompositor
 * \since 5.12
 * \brief The HWWaylandXdgSurface class provides desktop-style compositor-specific features to an xdg surface.
 *
 * This class is part of the HWWaylandXdgShell extension and provides a way to
 * extend the functionality of an existing QWaylandSurface with features
 * specific to desktop-style compositors, such as resizing and moving the
 * surface.
 *
 * It corresponds to the Wayland interface \c xdg_surface.
 */

/*!
 * Constructs a HWWaylandXdgSurface.
 */
HWWaylandXdgSurface::HWWaylandXdgSurface()
    : QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>(*new HWWaylandXdgSurfacePrivate)
{
}

/*!
 * Constructs a HWWaylandXdgSurface for \a surface and initializes it with the
 * given \a xdgShell, \a surface, and resource \a res.
 */
HWWaylandXdgSurface::HWWaylandXdgSurface(HWWaylandXdgShell *xdgShell, QWaylandSurface *surface, const QWaylandResource &res)
    : QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>(*new HWWaylandXdgSurfacePrivate)
{
    initialize(xdgShell, surface, res);
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgSurface::initialize(object xdgShell, object surface, object client, int id)
 *
 * Initializes the XdgSurface, associating it with the given \a xdgShell, \a surface,
 * \a client, and \a id.
 */

/*!
 * Initializes the HWWaylandXdgSurface, associating it with the given \a xdgShell, \a surface
 * and \a resource.
 */
void HWWaylandXdgSurface::initialize(HWWaylandXdgShell *xdgShell, QWaylandSurface *surface, const QWaylandResource &resource)
{
    Q_D(HWWaylandXdgSurface);
    d->m_xdgShell = xdgShell;
    d->m_surface = surface;
    d->init(resource.resource());
    setExtensionContainer(surface);
    d->m_windowGeometry = d->calculateFallbackWindowGeometry();
    connect(surface, &QWaylandSurface::destinationSizeChanged, this, &HWWaylandXdgSurface::handleSurfaceSizeChanged);
    connect(surface, &QWaylandSurface::bufferScaleChanged, this, &HWWaylandXdgSurface::handleBufferScaleChanged);
    emit shellChanged();
    emit surfaceChanged();
    QWaylandCompositorExtension::initialize();
}

/*!
 * \qmlproperty enum QtWaylandCompositor::XdgSurface::windowType
 *
 * This property holds the window type of the XdgSurface.
 */
Qt::WindowType HWWaylandXdgSurface::windowType() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_windowType;
}

/*!
 * \qmlproperty rect QtWaylandCompositor::XdgSurface::windowGeometry
 *
 * This property holds the window geometry of the HWWaylandXdgSurface. The window
 * geometry describes the window's visible bounds from the user's perspective.
 * The geometry includes title bars and borders if drawn by the client, but
 * excludes drop shadows. It is meant to be used for aligning and tiling
 * windows.
 */

/*!
 * \property HWWaylandXdgSurface::windowGeometry
 *
 * This property holds the window geometry of the HWWaylandXdgSurface. The window
 * geometry describes the window's visible bounds from the user's perspective.
 * The geometry includes title bars and borders if drawn by the client, but
 * excludes drop shadows. It is meant to be used for aligning and tiling
 * windows.
 */
QRect HWWaylandXdgSurface::windowGeometry() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_windowGeometry;
}

/*!
 * \internal
 */
void HWWaylandXdgSurface::initialize()
{
    QWaylandCompositorExtension::initialize();
}

void HWWaylandXdgSurface::handleSurfaceSizeChanged()
{
    Q_D(HWWaylandXdgSurface);
    d->updateFallbackWindowGeometry();
}

void HWWaylandXdgSurface::handleBufferScaleChanged()
{
    Q_D(HWWaylandXdgSurface);
    d->updateFallbackWindowGeometry();
}

/*!
 * \qmlproperty XdgShell QtWaylandCompositor::XdgSurface::shell
 *
 * This property holds the shell associated with this XdgSurface.
 */

/*!
 * \property HWWaylandXdgSurface::shell
 *
 * This property holds the shell associated with this HWWaylandXdgSurface.
 */
HWWaylandXdgShell *HWWaylandXdgSurface::shell() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_xdgShell;
}

/*!
 * \qmlproperty WaylandSurface QtWaylandCompositor::XdgSurface::surface
 *
 * This property holds the surface associated with this XdgSurface.
 */

/*!
 * \property HWWaylandXdgSurface::surface
 *
 * This property holds the surface associated with this HWWaylandXdgSurface.
 */
QWaylandSurface *HWWaylandXdgSurface::surface() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_surface;
}

/*!
 * \qmlproperty XdgToplevel QtWaylandCompositor::XdgSurface::toplevel
 *
 * This property holds the properties and methods that are specific to the
 * toplevel XdgSurface.
 *
 * \sa popup, XdgShell::toplevelCreated
 */

/*!
 * \property HWWaylandXdgSurface::toplevel
 *
 * This property holds the properties and methods that are specific to the
 * toplevel HWWaylandXdgSurface.
 *
 * \sa HWWaylandXdgSurface::popup, HWWaylandXdgShell::toplevelCreated
 */
HWWaylandXdgToplevel *HWWaylandXdgSurface::toplevel() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_toplevel;
}

/*!
 * \qmlproperty XdgPopup QtWaylandCompositor::XdgSurface::popup
 *
 * This property holds the properties and methods that are specific to the
 * popup XdgSurface.
 *
 * \sa toplevel, XdgShell::popupCreated
 */

/*!
 * \property HWWaylandXdgSurface::popup
 *
 * This property holds the properties and methods that are specific to the
 * popup HWWaylandXdgSurface.
 *
 * \sa HWWaylandXdgSurface::toplevel, HWWaylandXdgShell::popupCreated
 */
HWWaylandXdgPopup *HWWaylandXdgSurface::popup() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_popup;
}

/*!
 * Returns the Wayland interface for the HWWaylandXdgSurface.
 */
const wl_interface *HWWaylandXdgSurface::interface()
{
    return HWWaylandXdgSurfacePrivate::interface();
}

/*!
 * \internal
 */
QByteArray HWWaylandXdgSurface::interfaceName()
{
    return HWWaylandXdgSurfacePrivate::interfaceName();
}

/*!
 * Returns the HWWaylandXdgSurface corresponding to the \a resource.
 */
HWWaylandXdgSurface *HWWaylandXdgSurface::fromResource(wl_resource *resource)
{
    if (auto p = QtWayland::fromResource<HWWaylandXdgSurfacePrivate *>(resource))
        return p->q_func();
    return nullptr;
}

QWaylandQuickShellIntegration *HWWaylandXdgSurface::createIntegration(QWaylandQuickShellSurfaceItem *item)
{
    Q_UNUSED(item)
    return nullptr;
}


/*!
 * \qmltype XdgToplevel
 * \instantiates HWWaylandXdgToplevel
 * \inqmlmodule QtWayland.Compositor.XdgShell
 * \since 5.12
 * \brief XdgToplevel represents the toplevel window specific parts of an xdg surface.
 *
 * This type is part of the \l{XdgShell} extension and provides a way to
 * extend the functionality of an XdgSurface with features
 * specific to desktop-style windows.
 *
 * It corresponds to the Wayland interface \c xdg_toplevel.
 */

/*!
 * \class HWWaylandXdgToplevel
 * \inmodule QtWaylandCompositor
 * \since 5.12
 * \brief The HWWaylandXdgToplevel class represents the toplevel window specific parts of an xdg surface.
 *
 * This class is part of the HWWaylandXdgShell extension and provides a way to
 * extend the functionality of an HWWaylandXdgSurface with features
 * specific to desktop-style windows.
 *
 * It corresponds to the Wayland interface \c xdg_toplevel.
 */

/*!
 * Constructs a HWWaylandXdgToplevel for the given \a xdgSurface and \a resource.
 */
HWWaylandXdgToplevel::HWWaylandXdgToplevel(HWWaylandXdgSurface *xdgSurface, QWaylandResource &resource)
    : QObject(*new HWWaylandXdgToplevelPrivate(xdgSurface, resource))
{
    QList<HWWaylandXdgToplevel::State> states;
    sendConfigure({0, 0}, states);
}

HWWaylandXdgToplevel::~HWWaylandXdgToplevel()
{
    Q_D(HWWaylandXdgToplevel);
    // Usually, the decoration is destroyed by the client (according to the protocol),
    // but if the client misbehaves, or is shut down, we need to clean up here.
    if (Q_UNLIKELY(d->m_decoration))
        wl_resource_destroy(d->m_decoration->resource()->handle);
    Q_ASSERT(!d->m_decoration);
}

/*!
 * \qmlproperty XdgSurface QtWaylandCompositor::XdgToplevel::xdgSurface
 *
 * This property holds the XdgSurface for this XdgToplevel.
 */

/*!
 * \property HWWaylandXdgToplevel::xdgSurface
 *
 * This property holds the HWWaylandXdgSurface for this HWWaylandXdgToplevel.
 */
HWWaylandXdgSurface *HWWaylandXdgToplevel::xdgSurface() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_xdgSurface;
}

/*!
 * \qmlproperty XdgToplevel QtWaylandCompositor::XdgToplevel::parentToplevel
 *
 * This property holds the XdgToplevel parent of this XdgToplevel.
 */

/*!
 * \property HWWaylandXdgToplevel::parentToplevel
 *
 * This property holds the XdgToplevel parent of this XdgToplevel.
 *
 */
HWWaylandXdgToplevel *HWWaylandXdgToplevel::parentToplevel() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_parentToplevel;
}

/*!
 * \qmlproperty string QtWaylandCompositor::XdgToplevel::title
 *
 * This property holds the title of the XdgToplevel.
 */

/*!
 * \property HWWaylandXdgToplevel::title
 *
 * This property holds the title of the HWWaylandXdgToplevel.
 */
QString HWWaylandXdgToplevel::title() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_title;
}

/*!
 * \qmlproperty string QtWaylandCompositor::XdgToplevel::appId
 *
 * This property holds the app id of the XdgToplevel.
 */

/*!
 * \property HWWaylandXdgToplevel::appId
 *
 * This property holds the app id of the HWWaylandXdgToplevel.
 */
QString HWWaylandXdgToplevel::appId() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_appId;
}

/*!
 * \qmlproperty size QtWaylandCompositor::XdgToplevel::maxSize
 *
 * This property holds the maximum size of the XdgToplevel as requested by the client.
 *
 * The compositor is free to ignore this value and request a larger size.
 */

/*!
 * \property HWWaylandXdgToplevel::maxSize
 *
 * This property holds the maximum size of the HWWaylandXdgToplevel.
 *
 * The compositor is free to ignore this value and request a larger size.
 */
QSize HWWaylandXdgToplevel::maxSize() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_maxSize;
}

/*!
 * \qmlproperty size QtWaylandCompositor::XdgToplevel::minSize
 *
 * This property holds the minimum size of the XdgToplevel as requested by the client.
 *
 * The compositor is free to ignore this value and request a smaller size.
 */

/*!
 * \property HWWaylandXdgToplevel::minSize
 *
 * This property holds the minimum size of the HWWaylandXdgToplevel.
 *
 * The compositor is free to ignore this value and request a smaller size.
 */
QSize HWWaylandXdgToplevel::minSize() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_minSize;
}

/*!
 * \property HWWaylandXdgToplevel::states
 *
 * This property holds the last states the client acknowledged for this QWaylandToplevel.
 */
QList<HWWaylandXdgToplevel::State> HWWaylandXdgToplevel::states() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states;
}

/*!
 * \qmlproperty bool QtWaylandCompositor::XdgToplevel::maximized
 *
 * This property holds whether the client has acknowledged that it should be maximized.
 */

/*!
 * \property HWWaylandXdgToplevel::maximized
 *
 * This property holds whether the client has acknowledged that it should be maximized.
 */
bool HWWaylandXdgToplevel::maximized() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::MaximizedState);
}

/*!
 * \qmlproperty bool QtWaylandCompositor::XdgToplevel::fullscreen
 *
 * This property holds whether the client has acknowledged that it should be fullscreen.
 */

/*!
 * \property HWWaylandXdgToplevel::fullscreen
 *
 * This property holds whether the client has acknowledged that it should be fullscreen.
 */
bool HWWaylandXdgToplevel::fullscreen() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::FullscreenState);
}

/*!
 * \qmlproperty bool QtWaylandCompositor::XdgToplevel::resizing
 *
 * This property holds whether the client has acknowledged that it is being resized.
 */

/*!
 * \property HWWaylandXdgToplevel::resizing
 *
 * This property holds whether the client has acknowledged that it is being resized.
 */
bool HWWaylandXdgToplevel::resizing() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::ResizingState);
}

/*!
 * \qmlproperty bool QtWaylandCompositor::XdgToplevel::activated
 *
 * This property holds whether toplevel is drawing itself as having input focus.
 */

/*!
 * \property HWWaylandXdgToplevel::activated
 *
 * This property holds whether toplevel is drawing itself as having input focus.
 */
bool HWWaylandXdgToplevel::activated() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::ActivatedState);
}

/*!
 * \enum HWWaylandXdgToplevel::DecorationMode
 *
 * This enum type is used to specify the window decoration mode for toplevel windows.
 *
 * \value ServerSideDecoration The compositor should draw window decorations.
 * \value ClientSideDecoration The client should draw window decorations.
 */

/*!
 * \qmlproperty enumeration QtWaylandCompositor::XdgToplevel::decorationMode
 *
 * This property holds the current window decoration mode for this toplevel.
 *
 * The possible values are:
 * \value XdgToplevel.ServerSideDecoration The compositor should draw window decorations.
 * \value XdgToplevel.ClientSideDecoration The client should draw window decorations.
 *
 * \sa XdgDecorationManagerV1
 */

/*!
 * \property HWWaylandXdgToplevel::decorationMode
 *
 * This property holds the current window decoration mode for this toplevel.
 *
 * \sa HWWaylandXdgDecorationManagerV1
 */
HWWaylandXdgToplevel::DecorationMode HWWaylandXdgToplevel::decorationMode() const
{
    Q_D(const HWWaylandXdgToplevel);
    if(d->m_decoration)
        return (HWWaylandXdgToplevel::DecorationMode)d->m_decoration->configuredMode();

    return ClientSideDecoration;
}

/*!
 * \qmlmethod size QtWaylandCompositor::XdgToplevel::sizeForResize(size size, point delta, uint edges)
 *
 * Convenience for computing the new size given the current \a size, a \a delta, and
 * the \a edges active in the drag.
 */

/*!
 * Convenience for computing the new size given the current \a size, a \a delta, and
 * the \a edges active in the drag.
 */
QSize HWWaylandXdgToplevel::sizeForResize(const QSizeF &size, const QPointF &delta, Qt::Edges edges) const
{
    qreal width = size.width();
    qreal height = size.height();
    if (edges & Qt::LeftEdge)
        width -= delta.x();
    else if (edges & Qt::RightEdge)
        width += delta.x();

    if (edges & Qt::TopEdge)
        height -= delta.y();
    else if (edges & Qt::BottomEdge)
        height += delta.y();

    QSize newSize = QSize(width, height)
            .expandedTo(minSize())
            .expandedTo({1, 1}); // We don't want to send a size of (0,0) as that means that the client decides

    if (maxSize().isValid())
        newSize = newSize.boundedTo(maxSize());

    return newSize;
}

/*!
 * Sends a configure event to the client. Parameter \a size contains the pixel size
 * of the surface. A size of zero means the client is free to decide the size.
 * Known \a states are enumerated in HWWaylandXdgToplevel::State.
 */
uint HWWaylandXdgToplevel::sendConfigure(const QSize &size, const QList<HWWaylandXdgToplevel::State> &states)
{
    if (!size.isValid()) {
        qWarning() << "Can't configure xdg_toplevel with an invalid size" << size;
        return 0;
    }
    Q_D(HWWaylandXdgToplevel);
    auto statesBytes = QByteArray::fromRawData(reinterpret_cast<const char *>(states.data()),
                                               states.size() * static_cast<int>(sizeof(State)));
    uint32_t serial = d->m_xdgSurface->surface()->compositor()->nextSerial();
    d->m_pendingConfigures.append(HWWaylandXdgToplevelPrivate::ConfigureEvent{states, size, serial});
    d->send_configure(size.width(), size.height(), statesBytes);
    HWWaylandXdgSurfacePrivate::get(d->m_xdgSurface)->send_configure(serial);
    return serial;
}

/*!
 * \qmlmethod int QtWaylandCompositor::XdgToplevel::sendConfigure(size size, list<int> states)
 *
 * Sends a configure event to the client. \a size contains the pixel size of the surface.
 * A size of zero means the client is free to decide the size.
 * Known \a states are enumerated in XdgToplevel::State.
 */
uint HWWaylandXdgToplevel::sendConfigure(const QSize &size, const QList<int> &states)
{
    QList<State> s;
    for (auto state : states)
        s << State(state);
    return sendConfigure(size, s);
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgToplevel::sendClose()
 *
 * Sends a close event to the client. The client may choose to ignore the event.
 */

/*!
 * Sends a close event to the client. The client may choose to ignore the event.
 */
void HWWaylandXdgToplevel::sendClose()
{
    Q_D(HWWaylandXdgToplevel);
    d->send_close();
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgToplevel::sendMaximized(size size)
 *
 * Convenience for sending a configure event with the maximized state set, and
 * fullscreen and resizing removed. The activated state is left in its current state.
 *
 * \a size is the new size of the window.
 */

/*!
 * Convenience for sending a configure event with the maximized state set, and
 * fullscreen and resizing removed. The activated state is left in its current state.
 *
 * \a size is the new size of the window.
 */
uint HWWaylandXdgToplevel::sendMaximized(const QSize &size)
{
    Q_D(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent conf = d->lastSentConfigure();

    if (!conf.states.contains(HWWaylandXdgToplevel::State::MaximizedState))
        conf.states.append(HWWaylandXdgToplevel::State::MaximizedState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::FullscreenState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::ResizingState);

    return sendConfigure(size, conf.states);
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgToplevel::sendUnmaximized(size size)
 *
 * Convenience for sending a configure event with the maximized, fullscreen and
 * resizing states removed, and fullscreen and resizing removed. The activated
 * state is left in its current state.
 *
 * \a size is the new size of the window. If \a size is zero, the client decides the size.
 */

/*!
 * Convenience for sending a configure event with the maximized, fullscreen and
 * resizing states removed, and fullscreen and resizing removed. The activated
 * state is left in its current state.
 *
 * \a size is the new size of the window. If \a size is zero, the client decides the size.
 */
uint HWWaylandXdgToplevel::sendUnmaximized(const QSize &size)
{
    Q_D(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent conf = d->lastSentConfigure();

    conf.states.removeOne(HWWaylandXdgToplevel::State::MaximizedState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::FullscreenState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::ResizingState);

    return sendConfigure(size, conf.states);

}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgToplevel::sendFullscreen(size size)
 *
 * Convenience for sending a configure event with the fullscreen state set, and
 * maximized and resizing removed. The activated state is left in its current state.
 *
 * \sa sendUnmaximized
 *
 * \a size is the new size of the window.
 */

/*!
 * Convenience for sending a configure event with the fullscreen state set, and
 * maximized and resizing removed. The activated state is left in its current state.
 *
 * \sa sendUnmaximized
 *
 * \a size is the new size of the window.
 */
uint HWWaylandXdgToplevel::sendFullscreen(const QSize &size)
{
    Q_D(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent conf = d->lastSentConfigure();

    if (!conf.states.contains(HWWaylandXdgToplevel::State::FullscreenState))
        conf.states.append(HWWaylandXdgToplevel::State::FullscreenState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::MaximizedState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::ResizingState);

    return sendConfigure(size, conf.states);
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgToplevel::sendResizing(size maxSize)
 *
 * Convenience for sending a configure event with the resizing state set, and
 * maximized and fullscreen removed. The activated state is left in its current state.
 *
 * \a maxSize is the new size of the window.
 */

/*!
 * Convenience for sending a configure event with the resizing state set, and
 * maximized and fullscreen removed. The activated state is left in its current state.
 *
 * \a maxSize is the new size of the window.
 */
uint HWWaylandXdgToplevel::sendResizing(const QSize &maxSize)
{
    Q_D(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent conf = d->lastSentConfigure();

    if (!conf.states.contains(HWWaylandXdgToplevel::State::ResizingState))
        conf.states.append(HWWaylandXdgToplevel::State::ResizingState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::MaximizedState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::FullscreenState);

    return sendConfigure(maxSize, conf.states);
}

/*!
 * Returns the surface role for the QWaylandToplevel.
 */
QWaylandSurfaceRole *HWWaylandXdgToplevel::role()
{
    return &HWWaylandXdgToplevelPrivate::s_role;
}

/*!
 * Returns the HWWaylandXdgToplevel corresponding to the \a resource.
 */
HWWaylandXdgToplevel *HWWaylandXdgToplevel::fromResource(wl_resource *resource)
{
    if (auto p = QtWayland::fromResource<HWWaylandXdgToplevelPrivate *>(resource))
        return p->q_func();
    return nullptr;
}

/*!
 * \qmlsignal QtWaylandCompositor::XdgShell::xdgSurfaceCreated(XdgSurface xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_surface.
 * Note that \a xdgSurface is not mapped, i.e. according to the \c xdg-shell
 * protocol it should not be displayed, until it has received a role object.
 *
 * \sa toplevelCreated(), popupCreated()
 */

/*!
 * \fn void HWWaylandXdgShell::xdgSurfaceCreated(HWWaylandXdgSurface *xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_surface.
 * Note that \a xdgSurface is not mapped, i.e. according to the \c xdg-shell
 * protocol it should not be displayed, until it has received a role object.
 *
 * \sa toplevelCreated(), popupCreated()
 */

/*!
 * \qmlsignal QtWaylandCompositor::XdgShell::toplevelCreated(XdgToplevel toplevel, XdgSurface xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_toplevel.
 * A common use case is to let the handler of this signal instantiate a ShellSurfaceItem or
 * WaylandQuickItem for displaying \a toplevel in a QtQuick scene.
 *
 * \a xdgSurface is the XdgSurface \a toplevel is the role object for.
 */

/*!
 * \fn void HWWaylandXdgShell::toplevelCreated(HWWaylandXdgToplevel *toplevel, HWWaylandXdgSurface *xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_toplevel.
 * A common use case is to let the handler of this signal instantiate a QWaylandShellSurfaceItem or
 * QWaylandQuickItem for displaying \a toplevel in a QtQuick scene.
 *
 * \a xdgSurface is the XdgSurface \a toplevel is the role object for.
 */

/*!
 * \qmlsignal QtWaylandCompositor::XdgShell::popupCreated(XdgPopup popup, XdgSurface xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_popup.
 * A common use case is to let the handler of this signal instantiate a ShellSurfaceItem or
 * WaylandQuickItem for displaying \a popup in a QtQuick scene.
 *
 * \a xdgSurface is the XdgSurface \a popup is the role object for.
 */

/*!
 * \fn void HWWaylandXdgShell::popupCreated(HWWaylandXdgPopup *popup, HWWaylandXdgSurface *xdgSurface)
 *
 * This signal is emitted when the client has created a \c xdg_popup.
 * A common use case is to let the handler of this signal instantiate a QWaylandShellSurfaceItem or
 * QWaylandQuickItem for displaying \a popup in a QtQuick scene.
 *
 * \a xdgSurface is the XdgSurface \a popup is the role object for.
 */

/*!
 * \qmlsignal QtWaylandCompositor::XdgShell::pong(int serial)
 *
 * This signal is emitted when the client has responded to a ping event with serial, \a serial.
 *
 * \sa ping()
 */

/*!
 * \fn void HWWaylandXdgShell::pong(uint serial)
 *
 * This signal is emitted when the client has responded to a ping event with serial, \a serial.
 *
 * \sa HWWaylandXdgShell::ping()
 */

QList<int> HWWaylandXdgToplevel::statesAsInts() const
{
   QList<int> list;
   const auto s = states();
   list.reserve(s.size());
   for (auto state : s) {
       list << static_cast<int>(state);
   }
   return list;
}

QWaylandSurfaceRole HWWaylandXdgToplevelPrivate::s_role("xdg_toplevel");

HWWaylandXdgToplevelPrivate::HWWaylandXdgToplevelPrivate(HWWaylandXdgSurface *xdgSurface, const QWaylandResource &resource)
    : m_xdgSurface(xdgSurface)
{
    init(resource.resource());
}

void HWWaylandXdgToplevelPrivate::handleAckConfigure(uint serial)
{
    Q_Q(HWWaylandXdgToplevel);
    ConfigureEvent config;
    Q_FOREVER {
        if (m_pendingConfigures.empty()) {
            qWarning("Toplevel received an unexpected ack_configure!");
            return;
        }

        // This won't work unless there always is a toplevel.configure for each xdgsurface.configure
        config = m_pendingConfigures.takeFirst();

        if (config.serial == serial)
            break;
    }

    QList<uint> changedStates;
    std::set_symmetric_difference(
                m_lastAckedConfigure.states.begin(), m_lastAckedConfigure.states.end(),
                config.states.begin(), config.states.end(),
                std::back_inserter(changedStates));

    m_lastAckedConfigure = config;

    for (uint state : changedStates) {
        switch (state) {
        case state_maximized:
            emit q->maximizedChanged();
            break;
        case state_fullscreen:
            emit q->fullscreenChanged();
            break;
        case state_resizing:
            emit q->resizingChanged();
            break;
        case state_activated:
            emit q->activatedChanged();
            break;
        }
    }

    if (!changedStates.empty())
        emit q->statesChanged();
}

void HWWaylandXdgToplevelPrivate::handleFocusLost()
{
    Q_Q(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent current = lastSentConfigure();
    current.states.removeOne(HWWaylandXdgToplevel::State::ActivatedState);
    q->sendConfigure(current.size, current.states);
}

void HWWaylandXdgToplevelPrivate::handleFocusReceived()
{
    Q_Q(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent current = lastSentConfigure();
    if (!current.states.contains(HWWaylandXdgToplevel::State::ActivatedState)) {
        current.states.push_back(HWWaylandXdgToplevel::State::ActivatedState);
        q->sendConfigure(current.size, current.states);
    }
}

Qt::Edges HWWaylandXdgToplevelPrivate::convertToEdges(resize_edge edge)
{
    return Qt::Edges(((edge & 0b1100) >> 1) | ((edge & 0b0010) << 2) | (edge & 0b0001));
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_destroy_resource(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    delete q;
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_destroy(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    if (Q_UNLIKELY(m_decoration))
        qWarning() << "Client error: xdg_toplevel destroyed before its decoration object";

    wl_resource_destroy(resource->handle);
    //TODO: Should the xdg surface be desroyed as well? Or is it allowed to recreate a new toplevel for it?
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_parent(QtWaylandServer::xdg_toplevel::Resource *resource, wl_resource *parent)
{
    Q_UNUSED(resource);
    HWWaylandXdgToplevel *parentToplevel = HWWaylandXdgToplevel::fromResource(parent);

    Q_Q(HWWaylandXdgToplevel);

    if (m_parentToplevel != parentToplevel) {
        m_parentToplevel = parentToplevel;
        emit q->parentToplevelChanged();
    }

    if (m_parentToplevel && m_xdgSurface->windowType() != Qt::WindowType::SubWindow) {
        // There's a parent now, which means the surface is transient
        HWWaylandXdgSurfacePrivate::get(m_xdgSurface)->setWindowType(Qt::WindowType::SubWindow);
    } else if (!m_parentToplevel && m_xdgSurface->windowType() != Qt::WindowType::Window) {
        // When the surface has no parent it is toplevel
        HWWaylandXdgSurfacePrivate::get(m_xdgSurface)->setWindowType(Qt::WindowType::Window);
    }
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_title(QtWaylandServer::xdg_toplevel::Resource *resource, const QString &title)
{
    Q_UNUSED(resource);
    if (title == m_title)
        return;
    Q_Q(HWWaylandXdgToplevel);
    m_title = title;
    emit q->titleChanged();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_app_id(QtWaylandServer::xdg_toplevel::Resource *resource, const QString &app_id)
{
    Q_UNUSED(resource);
    if (app_id == m_appId)
        return;
    Q_Q(HWWaylandXdgToplevel);
    m_appId = app_id;
    emit q->appIdChanged();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_show_window_menu(QtWaylandServer::xdg_toplevel::Resource *resource, wl_resource *seatResource, uint32_t serial, int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    Q_UNUSED(serial);
    QPoint position(x, y);
    auto seat = QWaylandSeat::fromSeatResource(seatResource);
    Q_Q(HWWaylandXdgToplevel);
    emit q->showWindowMenu(seat, position);
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_move(Resource *resource, wl_resource *seatResource, uint32_t serial)
{
    Q_UNUSED(resource);
    Q_UNUSED(serial);
    Q_Q(HWWaylandXdgToplevel);
    QWaylandSeat *seat = QWaylandSeat::fromSeatResource(seatResource);
    emit q->startMove(seat);
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_resize(QtWaylandServer::xdg_toplevel::Resource *resource, wl_resource *seatResource, uint32_t serial, uint32_t edges)
{
    Q_UNUSED(resource);
    Q_UNUSED(serial);
    Q_Q(HWWaylandXdgToplevel);
    QWaylandSeat *seat = QWaylandSeat::fromSeatResource(seatResource);
    emit q->startResize(seat, convertToEdges(resize_edge(edges)));
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_max_size(QtWaylandServer::xdg_toplevel::Resource *resource, int32_t width, int32_t height)
{
    Q_UNUSED(resource);

    QSize maxSize(width, height);
    if (width == 0 && height == 0)
        maxSize = QSize(); // Wayland size of zero means unspecified which best translates to invalid

    if (m_maxSize == maxSize)
        return;

    if (width < 0 || height < 0) {
        // The spec says raise a protocol error, but there's no matching error defined
        qWarning() << "Received a xdg_toplevel.set_max_size request with a negative size";
        return;
    }

    if (m_minSize.isValid() && maxSize.isValid() &&
            (maxSize.width() < m_minSize.width() || maxSize.height() < m_minSize.height())) {
        // The spec says raise a protocol error, but there's no matching error defined
        qWarning() << "Received a xdg_toplevel.set_max_size request with a size smaller than the minimium size";
        return;
    }

    m_maxSize = maxSize;

    Q_Q(HWWaylandXdgToplevel);
    emit q->maxSizeChanged();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_min_size(QtWaylandServer::xdg_toplevel::Resource *resource, int32_t width, int32_t height)
{
    Q_UNUSED(resource);

    QSize minSize(width, height);
    if (width == 0 && height == 0)
        minSize = QSize(); // Wayland size of zero means unspecified

    if (m_minSize == minSize)
        return;

    if (width < 0 || height < 0) {
        // The spec says raise a protocol error, but there's no matching error defined
        qWarning() << "Received a xdg_toplevel.set_min_size request with a negative size";
        return;
    }

    if (m_maxSize.isValid() && minSize.isValid() &&
            (minSize.width() > m_maxSize.width() || minSize.height() > m_maxSize.height())) {
        // The spec says raise a protocol error, but there's no matching error defined
        qWarning() << "Received a xdg_toplevel.set_min_size request with a size larger than the maximum size";
        return;
    }

    m_minSize = minSize;

    Q_Q(HWWaylandXdgToplevel);
    emit q->minSizeChanged();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_maximized(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    emit q->setMaximized();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_unset_maximized(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    emit q->unsetMaximized();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_fullscreen(QtWaylandServer::xdg_toplevel::Resource *resource, wl_resource *output_res)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    QWaylandOutput *output = output_res ? QWaylandOutput::fromResource(output_res) : nullptr;
    emit q->setFullscreen(output);
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_unset_fullscreen(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    emit q->unsetFullscreen();
}

void HWWaylandXdgToplevelPrivate::xdg_toplevel_set_minimized(QtWaylandServer::xdg_toplevel::Resource *resource)
{
    Q_UNUSED(resource);
    Q_Q(HWWaylandXdgToplevel);
    emit q->setMinimized();
}

/*!
 * \qmltype XdgPopup
 * \instantiates HWWaylandXdgPopup
 * \inqmlmodule QtWayland.Compositor.XdgShell
 * \since 5.12
 * \brief XdgPopup represents the popup specific parts of and xdg surface.
 *
 * This type is part of the \l{XdgShell} extension and provides a way to extend
 * extend the functionality of an \l{XdgSurface} with features
 * specific to desktop-style menus for an xdg surface.
 *
 * It corresponds to the Wayland interface \c xdg_popup.
 */

/*!
 * \class HWWaylandXdgPopup
 * \inmodule QtWaylandCompositor
 * \since 5.12
 * \brief The HWWaylandXdgPopup class represents the popup specific parts of an xdg surface.
 *
 * This class is part of the HWWaylandXdgShell extension and provides a way to
 * extend the functionality of a HWWaylandXdgSurface with features
 * specific to desktop-style menus for an xdg surface.
 *
 * It corresponds to the Wayland interface \c xdg_popup.
 */

/*!
 * Constructs a HWWaylandXdgPopup.
 */
HWWaylandXdgPopup::HWWaylandXdgPopup(HWWaylandXdgSurface *xdgSurface, HWWaylandXdgSurface *parentXdgSurface,
                                   QWaylandXdgPositioner *positioner, QWaylandResource &resource)
    : QObject(*new HWWaylandXdgPopupPrivate(xdgSurface, parentXdgSurface, positioner, resource))
{
}

/*!
 * \qmlproperty XdgSurface QtWaylandCompositor::XdgPopup::xdgSurface
 *
 * This property holds the XdgSurface associated with this XdgPopup.
 */

/*!
 * \property HWWaylandXdgPopup::xdgSurface
 *
 * This property holds the HWWaylandXdgSurface associated with this HWWaylandXdgPopup.
 */
HWWaylandXdgSurface *HWWaylandXdgPopup::xdgSurface() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_xdgSurface;
}

/*!
 * \qmlproperty XdgSurface QtWaylandCompositor::XdgPopup::parentXdgSurface
 *
 * This property holds the XdgSurface associated with the parent of this XdgPopup.
 */

/*!
 * \property HWWaylandXdgPopup::parentXdgSurface
 *
 * This property holds the HWWaylandXdgSurface associated with the parent of this
 * HWWaylandXdgPopup.
 */
HWWaylandXdgSurface *HWWaylandXdgPopup::parentXdgSurface() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_parentXdgSurface;
}

/*!
 * \qmlproperty rect QtWaylandCompositor::XdgPopup::configuredGeometry
 *
 * The window geometry the popup received in the configure event. Relative to the
 * upper left corner of the parent surface.
 */

/*!
 * \property HWWaylandXdgPopup::configuredGeometry
 *
 * The window geometry the popup received in the configure event. Relative to the
 * upper left corner of the parent surface.
 */
QRect HWWaylandXdgPopup::configuredGeometry() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_geometry;
}

/*!
 * \qmlproperty rect QtWaylandCompositor::XdgPopup::anchorRect
 *
 * The anchor rectangle relative to the parent window geometry that the child
 * surface should be placed relative to.
 */

/*!
 * \property HWWaylandXdgPopup::anchorRect
 *
 * Returns the anchor rectangle relative to the parent window geometry that the child
 * surface should be placed relative to.
 */
QRect HWWaylandXdgPopup::anchorRect() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.anchorRect;
}

/*!
 * \qmlproperty enumeration QtWaylandCompositor::XdgPopup::anchorEdges
 *
 * This property holds the set of edges on the anchor rect that the child surface should be placed
 * relative to. If no edges are specified in a direction, the anchor point should be
 * centered between the edges.
 *
 * The possible values are:
 * \value Qt.TopEdge The top edge of the rectangle.
 * \value Qt.LeftEdge The left edge of the rectangle.
 * \value Qt.RightEdge The right edge of the rectangle.
 * \value Qt.BottomEdge The bottom edge of the rectangle.
 */

/*!
 * \property HWWaylandXdgPopup::anchorEdges
 *
 * Returns the set of edges on the anchor rect that the child surface should be placed
 * relative to. If no edges are specified in a direction, the anchor point should be
 * centered between the edges.
 */
Qt::Edges HWWaylandXdgPopup::anchorEdges() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.anchorEdges;
}

/*!
 * \qmlproperty rect QtWaylandCompositor::XdgPopup::gravityEdges
 *
 * Specifies in what direction the surface should be positioned, relative to the anchor
 * point.
 *
 * The possible values are:
 * \value Qt.TopEdge The surface should slide towards the top of the screen.
 * \value Qt.LeftEdge The surface should slide towards the left of the screen.
 * \value Qt.RightEdge The surface should slide towards the right of the screen.
 * \value Qt.BottomEdge The surface should slide towards the bottom of the screen.
 */

/*!
 * \property HWWaylandXdgPopup::gravityEdges
 *
 * Specifies in what direction the surface should be positioned, relative to the anchor
 * point.
 */
Qt::Edges HWWaylandXdgPopup::gravityEdges() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.gravityEdges;
}

/*!
 * \qmlproperty enumeration QtWaylandCompositor::XdgPopup::slideConstraints
 *
 * This property holds the orientations in which the child should slide to fit within the screen.
 *
 * Possible values:
 * \value Qt.Horizontal Horizontal
 * \value Qt.Vertical Vertical
 */

/*!
 * \property HWWaylandXdgPopup::slideConstraints
 *
 * This property holds the orientations in which the child should slide to fit within the screen.
 */
Qt::Orientations HWWaylandXdgPopup::slideConstraints() const
{
    Q_D(const HWWaylandXdgPopup);
    const uint flags = d->m_positionerData.constraintAdjustments;

    Qt::Orientations constraints = {};

    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_X)
        constraints |= Qt::Horizontal;
    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_SLIDE_Y)
        constraints |= Qt::Vertical;

    return constraints;
}

/*!
 * \qmlproperty enumeration QtWaylandCompositor::XdgPopup::flipConstraints
 *
 * This property holds the orientations in which the child should flip to fit within the screen.
 *
 * Possible values:
 * \value Qt.Horizontal Horizontal
 * \value Qt.Vertical Vertical
 */

/*!
 * \property HWWaylandXdgPopup::flipConstraints
 *
 * This property holds the orientations in which the child should flip to fit within the screen.
 */
Qt::Orientations HWWaylandXdgPopup::flipConstraints() const
{
    Q_D(const HWWaylandXdgPopup);
    const uint flags = d->m_positionerData.constraintAdjustments;

    Qt::Orientations constraints = {};

    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_X)
        constraints |= Qt::Horizontal;
    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_FLIP_Y)
        constraints |= Qt::Vertical;

    return constraints;
}

/*!
 * \qmlproperty enumeration QtWaylandCompositor::XdgPopup::resizeConstraints
 *
 * This property holds the orientations in which the child should resize to fit within the screen.
 *
 * Possible values:
 * \value Qt.Horizontal Horizontal
 * \value Qt.Vertical Vertical
 */

/*!
 * \property HWWaylandXdgPopup::resizeConstraints
 *
 * This property holds the orientations in which the child should resize to fit within the screen.
 */
Qt::Orientations HWWaylandXdgPopup::resizeConstraints() const
{
    Q_D(const HWWaylandXdgPopup);
    const uint flags = d->m_positionerData.constraintAdjustments;

    Qt::Orientations constraints = {};

    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_X)
        constraints |= Qt::Horizontal;
    if (flags & XDG_POSITIONER_CONSTRAINT_ADJUSTMENT_RESIZE_Y)
        constraints |= Qt::Vertical;

    return constraints;
}

/*!
 * \qmlproperty point QtWaylandCompositor::XdgPopup::offset
 *
 * The position relative to the position of the anchor on the anchor rectangle and
 * the anchor on the surface.
 */

/*!
 * \property HWWaylandXdgPopup::offset
 *
 * Returns the surface position relative to the position of the anchor on the anchor
 * rectangle and the anchor on the surface.
 */
QPoint HWWaylandXdgPopup::offset() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.offset;
}

/*!
 * \qmlproperty size QtWaylandCompositor::XdgPopup::positionerSize
 *
 * The size requested for the window geometry by the positioner object.
 */

/*!
 * \property HWWaylandXdgPopup::positionerSize
 *
 * Returns the size requested for the window geometry by the positioner object.
 */
QSize HWWaylandXdgPopup::positionerSize() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.size;
}

/*!
 * \qmlproperty point QtWaylandCompositor::XdgPopup::unconstrainedPosition
 *
 * The position of the surface relative to the parent window geometry if the surface
 * is not constrained. I.e. when not moved to fit inside the screen or similar.
 */

/*!
 * \property HWWaylandXdgPopup::unconstrainedPosition
 *
 * The position of the surface relative to the parent window geometry if the surface
 * is not constrained. I.e. when not moved to fit inside the screen or similar.
 */
QPoint HWWaylandXdgPopup::unconstrainedPosition() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.unconstrainedPosition();
}

/*!
 * \qmlmethod int QtWaylandCompositor::XdgPopup::sendConfigure(rect geometry)
 *
 * Sends a configure event to the client. \a geometry contains the window geometry
 * relative to the upper left corner of the window geometry of the parent surface.
 *
 * This implicitly sends a configure event to the corresponding XdgSurface as well.
 */

/*!
 * Sends a configure event to the client. \a geometry contains the window geometry
 * relative to the upper left corner of the window geometry of the parent surface.
 *
 * This implicitly sends a configure event to the corresponding HWWaylandXdgSurface
 * as well.
 */
uint HWWaylandXdgPopup::sendConfigure(const QRect &geometry)
{
    Q_D(HWWaylandXdgPopup);
    return d->sendConfigure(geometry);
}

/*!
 * \qmlmethod void QtWaylandCompositor::XdgPopup::sendPopupDone()
 * \since 5.14
 *
 * Dismiss the popup. According to the \c xdg-shell protocol this should make the
 * client destroy the popup.
 */

/*!
 * \since 5.14
 *
 * Dismiss the popup. According to the \c xdg-shell protocol this should make the
 * client destroy the popup.
 */
void HWWaylandXdgPopup::sendPopupDone()
{
    Q_D(HWWaylandXdgPopup);
    d->send_popup_done();
}

/*!
 * Returns the surface role for the QWaylandPopup.
 */
QWaylandSurfaceRole *HWWaylandXdgPopup::role()
{
    return &HWWaylandXdgPopupPrivate::s_role;
}

HWWaylandXdgPopupPrivate::HWWaylandXdgPopupPrivate(HWWaylandXdgSurface *xdgSurface, HWWaylandXdgSurface *parentXdgSurface,
                                                 QWaylandXdgPositioner *positioner, const QWaylandResource &resource)
    : m_xdgSurface(xdgSurface)
    , m_parentXdgSurface(parentXdgSurface)
    , m_positionerData(positioner->m_data)
{
    Q_ASSERT(m_positionerData.isComplete());
    init(resource.resource());

    HWWaylandXdgSurfacePrivate::get(m_xdgSurface)->setWindowType(Qt::WindowType::Popup);

    //TODO: Need an API for sending a different initial configure
    sendConfigure(QRect(m_positionerData.unconstrainedPosition(), m_positionerData.size));
}

void HWWaylandXdgPopupPrivate::handleAckConfigure(uint serial)
{
    Q_Q(HWWaylandXdgPopup);
    ConfigureEvent config;
    Q_FOREVER {
        if (m_pendingConfigures.empty()) {
            qWarning("Popup received an unexpected ack_configure!");
            return;
        }

        // This won't work unless there always is a popup.configure for each xdgsurface.configure
        config = m_pendingConfigures.takeFirst();

        if (config.serial == serial)
            break;
    }

    if (m_geometry == config.geometry)
        return;

    m_geometry = config.geometry;
    emit q->configuredGeometryChanged();
}

uint HWWaylandXdgPopupPrivate::sendConfigure(const QRect &geometry)
{
    uint32_t serial = m_xdgSurface->surface()->compositor()->nextSerial();
    m_pendingConfigures.append(HWWaylandXdgPopupPrivate::ConfigureEvent{geometry, serial});
    send_configure(geometry.x(), geometry.y(), geometry.width(), geometry.height());
    HWWaylandXdgSurfacePrivate::get(m_xdgSurface)->send_configure(serial);
    return serial;
}

void HWWaylandXdgPopupPrivate::xdg_popup_destroy(QtWaylandServer::xdg_popup::Resource *resource)
{
    Q_UNUSED(resource);
    qWarning() << Q_FUNC_INFO << "Not implemented"; //TODO
}

void HWWaylandXdgPopupPrivate::xdg_popup_grab(QtWaylandServer::xdg_popup::Resource *resource, wl_resource *seat, uint32_t serial)
{
    Q_UNUSED(resource);
    Q_UNUSED(serial);
    Q_UNUSED(seat);
    qWarning() << Q_FUNC_INFO << "Not implemented"; //TODO
    //switch keyboard focus
    //eventually send configure with activated.
}

QWaylandSurfaceRole HWWaylandXdgPopupPrivate::s_role("xdg_popup");


QT_END_NAMESPACE
