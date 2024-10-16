// Hollywood Wayland Compositor
// (C) 2021-2024 Originull Software
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include "xdgshell.h"
#include "xdgshell_p.h"
#include "layershell.h"
#include "xdgdialog.h"

#include <QtWaylandCompositor/private/qwaylandutils_p.h>
#include <QtWaylandCompositor/private/qwaylandxdgshell_p.h>

#include <QtWaylandCompositor/QWaylandCompositor>
#include <QtWaylandCompositor/QWaylandSeat>
#include <QtWaylandCompositor/QWaylandSurface>
#include <QtWaylandCompositor/QWaylandSurfaceRole>
#include <QtWaylandCompositor/QWaylandResource>
#include <QtWaylandCompositor/QWaylandShellSurfaceTemplate>

#include <QtCore/QObject>

#include <algorithm>
#include <type_traits>

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
    for (HWWaylandXdgSurface *xdgSurface : std::as_const(m_xdgSurfaces)) {
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

HWWaylandXdgShell::HWWaylandXdgShell()
    : QWaylandShellTemplate<HWWaylandXdgShell>(*new HWWaylandXdgShellPrivate())
{
}

HWWaylandXdgShell::HWWaylandXdgShell(QWaylandCompositor *compositor)
    : QWaylandShellTemplate<HWWaylandXdgShell>(compositor, *new HWWaylandXdgShellPrivate())
{
}

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

    // Support the dialog extension unconditionally.
    QObject *dialogExtension = new HWWaylandXdgDialogV1Global(compositor);
    dialogExtension->setParent(this);
}

const struct wl_interface *HWWaylandXdgShell::interface()
{
    return HWWaylandXdgShellPrivate::interface();
}

QByteArray HWWaylandXdgShell::interfaceName()
{
    return HWWaylandXdgShellPrivate::interfaceName();
}

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
    q->connect(m_toplevel, &HWWaylandXdgToplevel::modalChanged, q, [q, this](){
        q->setModal(m_toplevel->isModal());
    });
}

void HWWaylandXdgSurfacePrivate::xdg_surface_get_popup(QtWaylandServer::xdg_surface::Resource *resource, uint32_t id, wl_resource *parentResource, wl_resource *positionerResource)
{
    Q_Q(HWWaylandXdgSurface);

    if (m_toplevel || m_popup) {
        wl_resource_post_error(resource->handle, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,
                               "xdg_surface already has a role object");
        return;
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

    HWWaylandXdgSurface *parent = HWWaylandXdgSurface::fromResource(parentResource);
    if(parent)
    {
        QRect anchorBounds(QPoint(0, 0), parent->windowGeometry().size());
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

HWWaylandXdgSurface::HWWaylandXdgSurface()
    : QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>(*new HWWaylandXdgSurfacePrivate)
{
}

HWWaylandXdgSurface::HWWaylandXdgSurface(HWWaylandXdgShell *xdgShell, QWaylandSurface *surface, const QWaylandResource &res)
    : QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>(*new HWWaylandXdgSurfacePrivate)
{
    initialize(xdgShell, surface, res);
}

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

Qt::WindowType HWWaylandXdgSurface::windowType() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_windowType;
}

QRect HWWaylandXdgSurface::windowGeometry() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_windowGeometry;
}

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

HWWaylandXdgShell *HWWaylandXdgSurface::shell() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_xdgShell;
}

QWaylandSurface *HWWaylandXdgSurface::surface() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_surface;
}

HWWaylandXdgToplevel *HWWaylandXdgSurface::toplevel() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_toplevel;
}

HWWaylandXdgPopup *HWWaylandXdgSurface::popup() const
{
    Q_D(const HWWaylandXdgSurface);
    return d->m_popup;
}

const wl_interface *HWWaylandXdgSurface::interface()
{
    return HWWaylandXdgSurfacePrivate::interface();
}

QByteArray HWWaylandXdgSurface::interfaceName()
{
    return HWWaylandXdgSurfacePrivate::interfaceName();
}

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

HWWaylandXdgSurface *HWWaylandXdgToplevel::xdgSurface() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_xdgSurface;
}

HWWaylandXdgToplevel *HWWaylandXdgToplevel::parentToplevel() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_parentToplevel;
}

QString HWWaylandXdgToplevel::title() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_title;
}

QString HWWaylandXdgToplevel::appId() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_appId;
}

QSize HWWaylandXdgToplevel::maxSize() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_maxSize;
}

QSize HWWaylandXdgToplevel::minSize() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_minSize;
}

QList<HWWaylandXdgToplevel::State> HWWaylandXdgToplevel::states() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states;
}

bool HWWaylandXdgToplevel::maximized() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::MaximizedState);
}

bool HWWaylandXdgToplevel::fullscreen() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::FullscreenState);
}

bool HWWaylandXdgToplevel::resizing() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::ResizingState);
}

bool HWWaylandXdgToplevel::activated() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_lastAckedConfigure.states.contains(HWWaylandXdgToplevel::State::ActivatedState);
}

bool HWWaylandXdgToplevel::isModal() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_modal;
}

void HWWaylandXdgToplevel::setModal(bool newModal)
{
    Q_D(HWWaylandXdgToplevel);
    if (d->m_modal == newModal)
        return;
    d->m_modal = newModal;
    emit modalChanged();
}

HWWaylandXdgToplevel::DecorationMode HWWaylandXdgToplevel::decorationMode() const
{
    Q_D(const HWWaylandXdgToplevel);
    return d->m_decoration ? (HWWaylandXdgToplevel::DecorationMode)d->m_decoration->configuredMode() : DecorationMode::ClientSideDecoration;
}

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

uint HWWaylandXdgToplevel::sendConfigure(const QSize &size, const QList<int> &states)
{
    QList<State> s;
    for (auto state : states)
        s << State(state);
    return sendConfigure(size, s);
}

void HWWaylandXdgToplevel::sendClose()
{
    Q_D(HWWaylandXdgToplevel);
    d->send_close();
}

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
uint HWWaylandXdgToplevel::sendUnmaximized(const QSize &size)
{
    Q_D(HWWaylandXdgToplevel);
    HWWaylandXdgToplevelPrivate::ConfigureEvent conf = d->lastSentConfigure();

    conf.states.removeOne(HWWaylandXdgToplevel::State::MaximizedState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::FullscreenState);
    conf.states.removeOne(HWWaylandXdgToplevel::State::ResizingState);

    return sendConfigure(size, conf.states);

}

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

QWaylandSurfaceRole *HWWaylandXdgToplevel::role()
{
    return &HWWaylandXdgToplevelPrivate::s_role;
}

HWWaylandXdgToplevel *HWWaylandXdgToplevel::fromResource(wl_resource *resource)
{
    if (auto p = QtWayland::fromResource<HWWaylandXdgToplevelPrivate *>(resource))
        return p->q_func();
    return nullptr;
}

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

HWWaylandXdgPopup::HWWaylandXdgPopup(HWWaylandXdgSurface *xdgSurface, HWWaylandXdgSurface *parentXdgSurface,
                                   QWaylandXdgPositioner *positioner, QWaylandResource &resource)
    : QObject(*new HWWaylandXdgPopupPrivate(xdgSurface, parentXdgSurface, positioner, resource))
{
}

HWWaylandXdgPopup* HWWaylandXdgPopup::fromResource(wl_resource *resource)
{
    if (auto p = QtWayland::fromResource<HWWaylandXdgPopupPrivate *>(resource))
        return p->q_func();
    return nullptr;
}

HWWaylandXdgSurface *HWWaylandXdgPopup::xdgSurface() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_xdgSurface;
}

HWWaylandXdgSurface *HWWaylandXdgPopup::parentXdgSurface() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_parentXdgSurface;
}

QRect HWWaylandXdgPopup::configuredGeometry() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_geometry;
}

QRect HWWaylandXdgPopup::anchorRect() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.anchorRect;
}

Qt::Edges HWWaylandXdgPopup::anchorEdges() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.anchorEdges;
}

Qt::Edges HWWaylandXdgPopup::gravityEdges() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.gravityEdges;
}

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

QPoint HWWaylandXdgPopup::offset() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.offset;
}

QSize HWWaylandXdgPopup::positionerSize() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.size;
}

QPoint HWWaylandXdgPopup::unconstrainedPosition() const
{
    Q_D(const HWWaylandXdgPopup);
    return d->m_positionerData.unconstrainedPosition();
}

uint HWWaylandXdgPopup::sendConfigure(const QRect &geometry)
{
    Q_D(HWWaylandXdgPopup);
    return d->sendConfigure(geometry);
}

void HWWaylandXdgPopup::sendPopupDone()
{
    Q_D(HWWaylandXdgPopup);
    d->send_popup_done();
}

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

void HWWaylandXdgPopupPrivate::setParentLayerSurface(WlrLayerSurfaceV1 *surface)
{
    m_parentXdgSurface = nullptr;
    m_layerParent = surface;
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

