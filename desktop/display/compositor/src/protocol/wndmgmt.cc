// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "wndmgmt.h"
#include "surfaceobject.h"
#include "compositor.h"

#include <QWaylandSurface>
#include <QFile>

#define PWM_PROTO_VERSION       15
#define PW_PROTO_VERSION        16

Q_LOGGING_CATEGORY(hwPlasmaWnd, "compositor.plasmawnd")

PlasmaWindowManagement::PlasmaWindowManagement(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::org_kde_plasma_window_management(compositor->display(), PWM_PROTO_VERSION)
    , m_compositor(qobject_cast<Compositor*>(compositor))
{
    qCInfo(hwPlasmaWnd, "Supporting org_kde_plasma_window_management (protocol version %i)", PWM_PROTO_VERSION);
}


void PlasmaWindowManagement::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), PWM_PROTO_VERSION);
}

PlasmaWindowControl* PlasmaWindowManagement::createWindowControl(Surface *parent)
{
    if(parent == nullptr)
        qCDebug(hwPlasmaWnd, "PlasmaWindowManagement::createWindowControl sent null parent");

    auto pm = new PlasmaWindowControl(parent, parent->uuid());
    for(auto res : resourceMap())
        send_window_with_uuid(res->handle, parent->id(), parent->uuid().toString(QUuid::WithoutBraces));

    m_windows.append(pm);
    return pm;
}

void PlasmaWindowManagement::updateZOrder(const QString &uuids)
{
    for(auto res : resourceMap())
        send_stacking_order_uuid_changed(res->handle, uuids);
}

void PlasmaWindowManagement::removeWindow(PlasmaWindowControl *wnd)
{
    m_windows.removeOne(wnd);
}

void PlasmaWindowManagement::org_kde_plasma_window_management_bind_resource(Resource *resource)
{
    // A new shell client has just bound
    // TODO: send_window_with_uuid for all windows
    // sendStackingOrderUuidsChanged
    for(auto wnd : m_compositor->surfaces())
    {
        if(wnd->hasPlasmaControl())
            send_window_with_uuid(resource->handle, wnd->id(), wnd->uuid().toString(QUuid::WithoutBraces));
    }
    send_stacking_order_uuid_changed(resource->handle,
                                     m_compositor->surfaceZOrderByUUID());

    m_shellbound = true;
}

void PlasmaWindowManagement::org_kde_plasma_window_management_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    m_shellbound = false;
}

void PlasmaWindowManagement::org_kde_plasma_window_management_get_window(Resource *resource, uint32_t id, uint32_t internal_window_id)
{
    Q_UNUSED(resource);
    Q_UNUSED(id);
    Q_UNUSED(internal_window_id);
    qCDebug(hwPlasmaWnd, "org_kde_plasma_window_management_get_window called but not responsive: Use org_kde_plasma_window_management_get_window_by_uuid instead.");
}

void PlasmaWindowManagement::org_kde_plasma_window_management_get_window_by_uuid(Resource *resource, uint32_t id, const QString & internal_window_uuid)
{
    for(auto w : m_windows)
    {
        if(w->uuid().toString(QUuid::WithoutBraces) == internal_window_uuid)
        {
            w->add(resource->client(), id, resource->version());
            return;
        }
    }
    qCDebug(hwPlasmaWnd, "PlasmaWindowManagement: requested UUID with no window: %s", internal_window_uuid.toUtf8().data());
    PlasmaWindowControl w(nullptr, QUuid::fromString(internal_window_uuid));
    w.add(resource->client(), resource->version());
}

// --------------------------------------------

PlasmaWindowControl::PlasmaWindowControl(Surface *obj, const QUuid &uuid)
    : QtWaylandServer::org_kde_plasma_window()
    , m_parent(obj)
    , m_uuid(uuid)
{
    qCDebug(hwPlasmaWnd, "%s: allocating kde_plasma_window control", m_uuid.toString(QUuid::WithoutBraces).toUtf8().data());
}

PlasmaWindowControl::~PlasmaWindowControl()
{
    qCDebug(hwPlasmaWnd, "%s: deallocating kde_plasma_window control", m_uuid.toString(QUuid::WithoutBraces).toUtf8().data());
    CompositorApp::instance()->compositor()->windowManager()->removeWindow(this);
    unmap();
}

void PlasmaWindowControl::org_kde_plasma_window_bind_resource(Resource *resource)
{
    if(!m_parent)
    {
        qCDebug(hwPlasmaWnd, "org_kde_plasma_window_bind_resource: invalid parent, bailing");
        return;
    }

    send_initial_state(resource->handle);
    send_title_changed(resource->handle, m_title);
    send_pid_changed(resource->handle, m_parent->surface()->client()->processId());
    send_app_id_changed(resource->handle, m_parent->appId());
    send_themed_icon_name_changed(resource->handle, m_parent->themedIcon());
}

QUuid PlasmaWindowControl::uuid() const
{
    return m_uuid;
}

void PlasmaWindowControl::destroy()
{
    unmap();
}

void PlasmaWindowControl::setActive(bool set)
{
    if(m_mapped)
        setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_ACTIVE, set);
}

void PlasmaWindowControl::setMinimized(bool set)
{
    setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZED, set);
}

void PlasmaWindowControl::setMaximized(bool set)
{
    setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZED, set);
}

void PlasmaWindowControl::setMinimizable(bool set)
{
    setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZABLE, set);
}

void PlasmaWindowControl::setMaximizable(bool set)
{
    setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZABLE, set);
}

void PlasmaWindowControl::setCloseable(bool set)
{
    setState(ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_CLOSEABLE, set);
}

void PlasmaWindowControl::setTitle(const QString &title)
{
    m_title = title;
    for (auto r : resourceMap())
    {
        if(r)
            send_title_changed(r->handle, title);
    }
    emit titleChanged(title);
}

void PlasmaWindowControl::setThemedIcon(const QString &icon)
{
    m_themedicon = icon;
    for (auto r : resourceMap())
        send_themed_icon_name_changed(r->handle, icon);
    emit themedIconChanged(icon);
}

void PlasmaWindowControl::setIcon(const QIcon &icon)
{
    m_icon = icon;
    for (auto r : resourceMap())
        send_icon_changed(r->handle);
}

void PlasmaWindowControl::org_kde_plasma_window_set_state(Resource *resource, uint32_t flags, uint32_t state)
{
    Q_UNUSED(resource);
    emit setStateRequest(flags, state);
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_ACTIVE) {
        emit activeRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_ACTIVE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZED) {
        emit minimizedRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZED);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZED) {
        emit maximizedRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZED);
    }
    /*if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_FULLSCREEN) {
        emit fullscreenRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_FULLSCREEN);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_DEMANDS_ATTENTION) {
        emit demandsAttentionRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_DEMANDS_ATTENTION);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_CLOSEABLE) {
        emit closeableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_CLOSEABLE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZABLE) {
        emit minimizeableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MINIMIZABLE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZABLE) {
        emit maximizeableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MAXIMIZABLE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_FULLSCREENABLE) {
        emit fullscreenableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_FULLSCREENABLE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_SKIPTASKBAR) {
        emit skipTaskbarRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_SKIPTASKBAR);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MOVABLE) {
        emit movableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_MOVABLE);
    }
    if (flags & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_RESIZABLE) {
        emit resizableRequested(state & ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_RESIZABLE);
    }*/
}

void PlasmaWindowControl::org_kde_plasma_window_set_virtual_desktop(Resource *resource, uint32_t number)
{
    Q_UNUSED(resource);
    Q_UNUSED(number);
    qCDebug(hwPlasmaWnd, "org_kde_plasma_window_set_virtual_desktop not currently implemented but it eventually will be");
}

void PlasmaWindowControl::org_kde_plasma_window_get_icon(Resource *resource, int32_t fd)
{
    Q_UNUSED(resource);
    QFile file;
    file.open(fd, QFile::WriteOnly);
    QDataStream in(&file);
    in >> m_icon;
    file.close();
}

void PlasmaWindowControl::org_kde_plasma_window_close(Resource *resource)
{
    Q_UNUSED(resource);
    emit closeRequested();
}

void PlasmaWindowControl::org_kde_plasma_window_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

void PlasmaWindowControl::unmap()
{
    if(!m_mapped)
        return;

    m_mapped = false;
    for (auto r : resourceMap())
        send_unmapped(r->handle);
}

void PlasmaWindowControl::setState(org_kde_plasma_window_management_state flag, bool set)
{
    quint32 ns = m_state;
    if(set)
        ns |= flag;
    else
        ns &= ~flag;
    if(ns == m_state)
        return;

    m_state = ns;
    for (auto r : resourceMap())
    {
        if(r)
            send_state_changed(r->handle, ns);
    }
}

