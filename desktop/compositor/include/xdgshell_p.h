// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef XDGSHELL_P_H
#define XDGSHELL_P_H

#include <QtWaylandCompositor/QWaylandCompositorExtension>
#include <QtWaylandCompositor/QWaylandResource>
#include <QtWaylandCompositor/QWaylandShell>
#include <QtWaylandCompositor/QWaylandShellSurface>
//#include <QtWaylandCompositor/qwaylandquickchildren.h>
#include <QtWaylandCompositor/private/qwaylandcompositorextension_p.h>
#include <QtWaylandCompositor/private/qwaylandshell_p.h>
#include <QtWaylandCompositor/private/qwayland-server-xdg-shell.h>
#include <QtWaylandCompositor/private/qwaylandxdgdecorationv1_p.h>
#include <QtCore/QSet>
#include <QtWaylandCompositor/private/qwaylandxdgshell_p.h>
#include <QtCore/QRect>

#include "xdgshell.h"
#include "layershell.h"

struct wl_resource;

QT_BEGIN_NAMESPACE

class HWWaylandXdgShellPrivate
        : public QWaylandShellPrivate
        , public QtWaylandServer::xdg_wm_base
{
    Q_DECLARE_PUBLIC(HWWaylandXdgShell)
public:
    HWWaylandXdgShellPrivate();
    void ping(Resource *resource, uint32_t serial);
    void registerXdgSurface(HWWaylandXdgSurface *xdgSurface);
    void unregisterXdgSurface(HWWaylandXdgSurface *xdgSurface);
    static HWWaylandXdgShellPrivate *get(HWWaylandXdgShell *xdgShell) { return xdgShell->d_func(); }

    QSet<uint32_t> m_pings;
    QMultiMap<struct wl_client *, HWWaylandXdgSurface *> m_xdgSurfaces;

    HWWaylandXdgSurface *xdgSurfaceFromSurface(QWaylandSurface *surface);

protected:
    void xdg_wm_base_destroy(Resource *resource) override;
    void xdg_wm_base_create_positioner(Resource *resource, uint32_t id) override;
    void xdg_wm_base_get_xdg_surface(Resource *resource, uint32_t id,
                                     struct ::wl_resource *surface) override;
    void xdg_wm_base_pong(Resource *resource, uint32_t serial) override;
};

class HWWaylandXdgSurfacePrivate
        : public QWaylandCompositorExtensionPrivate
        , public QtWaylandServer::xdg_surface
{
    Q_DECLARE_PUBLIC(HWWaylandXdgSurface)
public:
    HWWaylandXdgSurfacePrivate();
    void setWindowType(Qt::WindowType windowType);
    void handleFocusLost();
    void handleFocusReceived();
    static HWWaylandXdgSurfacePrivate *get(HWWaylandXdgSurface *xdgSurface) { return xdgSurface->d_func(); }

    QRect calculateFallbackWindowGeometry() const;
    void updateFallbackWindowGeometry();

private:
    HWWaylandXdgShell *m_xdgShell = nullptr;
    QWaylandSurface *m_surface = nullptr;

    HWWaylandXdgToplevel *m_toplevel = nullptr;
    HWWaylandXdgPopup *m_popup = nullptr;
    QRect m_windowGeometry;
    bool m_unsetWindowGeometry = true;
    QMargins m_windowMargins;
    Qt::WindowType m_windowType = Qt::WindowType::Window;

    void xdg_surface_destroy_resource(Resource *resource) override;
    void xdg_surface_destroy(Resource *resource) override;
    void xdg_surface_get_toplevel(Resource *resource, uint32_t id) override;
    void xdg_surface_get_popup(Resource *resource, uint32_t id, struct ::wl_resource *parent, struct ::wl_resource *positioner) override;
    void xdg_surface_ack_configure(Resource *resource, uint32_t serial) override;
    void xdg_surface_set_window_geometry(Resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
};

class HWWaylandXdgToplevelPrivate : public QObjectPrivate, public QtWaylandServer::xdg_toplevel
{
    Q_DECLARE_PUBLIC(HWWaylandXdgToplevel)
public:
    struct ConfigureEvent {
        ConfigureEvent() = default;
        ConfigureEvent(const QList<HWWaylandXdgToplevel::State>
                       &incomingStates,
                       const QSize &incomingSize, uint incomingSerial)
        : states(incomingStates), size(incomingSize), serial(incomingSerial)
        { }
        QList<HWWaylandXdgToplevel::State> states;
        QSize size = {0, 0};
        uint serial = 0;
    };

    HWWaylandXdgToplevelPrivate(HWWaylandXdgSurface *xdgSurface, const QWaylandResource& resource);
    ConfigureEvent lastSentConfigure() const { return m_pendingConfigures.empty() ? m_lastAckedConfigure : m_pendingConfigures.last(); }
    void handleAckConfigure(uint serial); //TODO: move?
    void handleFocusLost();
    void handleFocusReceived();

    static HWWaylandXdgToplevelPrivate *get(HWWaylandXdgToplevel *toplevel) { return toplevel->d_func(); }
    static Qt::Edges convertToEdges(resize_edge edge);

protected:

    void xdg_toplevel_destroy_resource(Resource *resource) override;

    void xdg_toplevel_destroy(Resource *resource) override;
    void xdg_toplevel_set_parent(Resource *resource, struct ::wl_resource *parent) override;
    void xdg_toplevel_set_title(Resource *resource, const QString &title) override;
    void xdg_toplevel_set_app_id(Resource *resource, const QString &app_id) override;
    void xdg_toplevel_show_window_menu(Resource *resource, struct ::wl_resource *seat, uint32_t serial, int32_t x, int32_t y) override;
    void xdg_toplevel_move(Resource *resource, struct ::wl_resource *seatResource, uint32_t serial) override;
    void xdg_toplevel_resize(Resource *resource, struct ::wl_resource *seat, uint32_t serial, uint32_t edges) override;
    void xdg_toplevel_set_max_size(Resource *resource, int32_t width, int32_t height) override;
    void xdg_toplevel_set_min_size(Resource *resource, int32_t width, int32_t height) override;
    void xdg_toplevel_set_maximized(Resource *resource) override;
    void xdg_toplevel_unset_maximized(Resource *resource) override;
    void xdg_toplevel_set_fullscreen(Resource *resource, struct ::wl_resource *output) override;
    void xdg_toplevel_unset_fullscreen(Resource *resource) override;
    void xdg_toplevel_set_minimized(Resource *resource) override;

public:
    HWWaylandXdgSurface *m_xdgSurface = nullptr;
    HWWaylandXdgToplevel *m_parentToplevel = nullptr;
    QList<ConfigureEvent> m_pendingConfigures;
    ConfigureEvent m_lastAckedConfigure;
    QString m_title;
    QString m_appId;
    QSize m_maxSize;
    QSize m_minSize = {0, 0};
    QWaylandXdgToplevelDecorationV1 *m_decoration = nullptr;

    static QWaylandSurfaceRole s_role;
};

class HWWaylandXdgPopupPrivate : public QObjectPrivate, public QtWaylandServer::xdg_popup
{
    Q_DECLARE_PUBLIC(HWWaylandXdgPopup)
public:
    struct ConfigureEvent {
        QRect geometry;
        uint serial;
    };

    HWWaylandXdgPopupPrivate(HWWaylandXdgSurface *xdgSurface, HWWaylandXdgSurface *parentXdgSurface,
                            QWaylandXdgPositioner *positioner, const QWaylandResource& resource);

    void handleAckConfigure(uint serial);

    static HWWaylandXdgPopupPrivate *get(HWWaylandXdgPopup *popup) { return popup->d_func(); }

    static QWaylandSurfaceRole s_role;

private:
    friend class WlrLayerSurfaceV1;
    uint sendConfigure(const QRect &geometry);
    void setParentLayerSurface(WlrLayerSurfaceV1 *surface);

protected:
    void xdg_popup_destroy(Resource *resource) override;
    void xdg_popup_grab(Resource *resource, struct ::wl_resource *seat, uint32_t serial) override;

private:
    HWWaylandXdgSurface *m_xdgSurface = nullptr;
    HWWaylandXdgSurface *m_parentXdgSurface = nullptr;
    WlrLayerSurfaceV1 *m_layerParent = nullptr;
    QWaylandXdgPositionerData m_positionerData;
    QRect m_geometry;
    QList<ConfigureEvent> m_pendingConfigures;
};

#endif // XDGSHELL_P_H
