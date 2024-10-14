// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// This class was derived from Liri
// (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#include "layershell.h"
#include "application.h"
#include "compositor.h"
#include "xdgshell_p.h"

#define LAYERSHELL_VERSION 4

WlrLayerShellV1::WlrLayerShellV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<WlrLayerShellV1>(compositor)
    , QtWaylandServer::zwlr_layer_shell_v1() {}

WlrLayerShellV1::~WlrLayerShellV1()
{

}

void WlrLayerShellV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if(!compositor)
    {
        qDebug() << "WlrLayerShellV1 failed to find compositor, not initializing";
        return;
    }
    init(compositor->display(), LAYERSHELL_VERSION);
}

void WlrLayerShellV1::closeAllSurfaces()
{
    for(auto *s : surfaces)
        s->close();
}

WlrLayerSurfaceV1 *WlrLayerShellV1::fromResource(wl_resource *resource)
{
    for(auto *s : wcApp->compositor()->layerShell()->surfaces)
    {
        if(s->resource()->handle == resource)
            return s;
    }

    return nullptr;
}

void WlrLayerShellV1::zwlr_layer_shell_v1_get_layer_surface(Resource *resource, uint32_t id, wl_resource *surfaceRes, wl_resource *outputRes, uint32_t layer, const QString &nameSpace)
{
    auto surface = QWaylandSurface::fromResource(surfaceRes);
    if(!surface)
    {
        qDebug() << "WlrLayerShell: resource does not exist" << wl_resource_get_id(surfaceRes);
        return;
    }

     if(!surface->setRole(WlrLayerSurfaceV1::role(), resource->handle, error_role))
        return;

     QWaylandOutput *output = nullptr;
     if(outputRes)
         output = QWaylandOutput::fromResource(outputRes);

     auto ls = new WlrLayerSurfaceV1(surface, output, static_cast<WlrLayerShellV1::Layer>(layer), nameSpace);
     ls->init(resource->client(), id, resource->version());
     emit layerSurfaceCreated(ls);
}

// ---------------------------------------------------------------------------

QWaylandSurfaceRole WlrLayerSurfaceV1::s_role("zwlr_layer_surface_v1");

WlrLayerSurfaceV1::WlrLayerSurfaceV1(QWaylandSurface *surface, QWaylandOutput *output,
                                     WlrLayerShellV1::Layer layer, const QString &nameSpace, QObject *parent)
    : QObject(parent)
    , QtWaylandServer::zwlr_layer_surface_v1(),
      m_surface(surface),
      m_output(output),
      m_namespace(nameSpace)
{
    current.layer = layer;
    clientPending.layer = layer;
    connect(m_surface, &QWaylandSurface::redraw, this, [this] {
        if (closed)
            return;

        if (m_surface->hasContent() && !configured) {
            qDebug() << QString("layer_surface@%1 has not yet been configured")
                        .arg(wl_resource_get_id(resource()->handle));
            wl_resource_post_error(resource()->handle,
                                   QtWaylandServer::zwlr_layer_shell_v1::error_already_constructed,
                                   "layer_surface@%u has never been configured",
                                   wl_resource_get_id(resource()->handle));
            return;
        }

        // Set double-buffered properties
        bool hasChanged = false;
        if (current.layer != clientPending.layer) {
            current.layer = clientPending.layer;
            qDebug() << "layerChanged:" << current.layer << clientPending.layer;
            emit layerChanged();
            hasChanged = true;
        }
        if (current.desiredSize != clientPending.desiredSize) {
            current.desiredSize = clientPending.desiredSize;
            qDebug() << "desiredSize:" << clientPending.desiredSize << "current:" << current.desiredSize;
            emit sizeChanged();
            hasChanged = true;
        }
        if (current.anchors != clientPending.anchors) {
            current.anchors = clientPending.anchors;
            emit anchorsChanged();
            hasChanged = true;
        }
        if (current.exclusiveZone != clientPending.exclusiveZone) {
            current.exclusiveZone = clientPending.exclusiveZone;
            emit exclusiveZoneChanged();
            hasChanged = true;
        }
        if (current.margins != clientPending.margins) {
            current.margins = clientPending.margins;
            emit leftMarginChanged();
            emit rightMarginChanged();
            emit topMarginChanged();
            emit bottomMarginChanged();
            hasChanged = true;
        }
        if (current.keyboardInteractivity != clientPending.keyboardInteractivity) {
            current.keyboardInteractivity = clientPending.keyboardInteractivity;
            emit keyboardInteractivityChanged();
            hasChanged = true;
        }
        if (hasChanged)
            emit changed();

        if (!added)
            added = true;

        if (configured && m_surface->hasContent() && !mapped) {
            mapped = true;
            emit mappedChanged();
        }

        if (configured && !m_surface->hasContent() && mapped)
            unmap();
    });
}

quint32 WlrLayerSurfaceV1::sendConfigure(const QSize &size)
{
    if (!size.isValid())
    {
        qDebug() << "Can't configure WaylandWlrLayerSurfaceV1 with invalid size:" << size;
        return 0;
    }

    quint32 serial = m_surface->compositor()->nextSerial();
    pendingConfigures.append(WlrLayerSurfaceV1::ConfigureEvent(size, serial));
    send_configure(serial, size.width(), size.height());
    return serial;
}

quint32 WlrLayerSurfaceV1::sendConfigure(int width, int height)
{
    return sendConfigure(QSize(width,height));
}

void WlrLayerSurfaceV1::close()
{
    closed = true;
    unmap();
    send_closed();
}

QWaylandSurface *WlrLayerSurfaceV1::surface() const
{
    return m_surface;
}

QWaylandOutput *WlrLayerSurfaceV1::output() const
{
    return m_output;
}

WlrLayerShellV1::Layer WlrLayerSurfaceV1::layer() const
{
    qDebug() << current.layer;
    return current.layer;
}

QString WlrLayerSurfaceV1::nameSpace() const
{
    return m_namespace;
}

QSize WlrLayerSurfaceV1::size() const
{
    return current.desiredSize;
}

quint32 WlrLayerSurfaceV1::width() const
{
    return current.desiredSize.width();
}

quint32 WlrLayerSurfaceV1::height() const
{
    return current.desiredSize.height();
}

WlrLayerSurfaceV1::Anchors WlrLayerSurfaceV1::anchors() const
{
    return current.anchors;
}

int WlrLayerSurfaceV1::exclusiveZone() const
{
    return current.exclusiveZone;
}

QMargins WlrLayerSurfaceV1::margins() const
{
    return current.margins;
}

qint32 WlrLayerSurfaceV1::leftMargin() const
{
    return current.margins.left();
}

qint32 WlrLayerSurfaceV1::topMargin() const
{
    return current.margins.top();
}

qint32 WlrLayerSurfaceV1::rightMargin() const
{
    return current.margins.right();
}

qint32 WlrLayerSurfaceV1::bottomMargin() const
{
    return current.margins.bottom();
}

WlrLayerSurfaceV1::KeyboardInteractivity WlrLayerSurfaceV1::keyboardInteractivity() const
{
    return current.keyboardInteractivity;
}

bool WlrLayerSurfaceV1::isMapped() const
{
    return mapped;
}

bool WlrLayerSurfaceV1::isConfigured() const
{
    return configured;
}

void WlrLayerSurfaceV1::unmap()
{
    pendingConfigures.clear();
    if(mapped)
    {
        mapped = false;
        emit mappedChanged();
    }
    if(configured)
    {
        configured = false;
        emit configuredChanged();
    }
}

QWaylandSurfaceRole *WlrLayerSurfaceV1::role()
{
    return &s_role;
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_size(Resource *resource, uint32_t width, uint32_t height)
{
    Q_UNUSED(resource);
    clientPending.desiredSize = QSize(width, height);
    m_surface->redraw();
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_anchor(Resource *resource, uint32_t anchor)
{
    Q_UNUSED(resource)

    const uint32_t maxAnchor = anchor_top | anchor_bottom | anchor_left | anchor_right;
    if (anchor > maxAnchor) {
        wl_resource_post_error(resource->handle, error_invalid_anchor,
                               "invalid anchor %d", anchor);
        return;
    }

    clientPending.anchors = static_cast<WlrLayerSurfaceV1::Anchors>(anchor);
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_exclusive_zone(Resource *resource, int32_t zone)
{
    Q_UNUSED(resource);
    clientPending.exclusiveZone = zone;
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_margin(Resource *resource, int32_t top, int32_t right, int32_t bottom, int32_t left)
{
    Q_UNUSED(resource);
    clientPending.margins = QMargins(left, top, right, bottom);
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_keyboard_interactivity(Resource *resource, uint32_t keyboard_interactivity)
{
    Q_UNUSED(resource);
    clientPending.keyboardInteractivity = static_cast<WlrLayerSurfaceV1::KeyboardInteractivity>(keyboard_interactivity);
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_get_popup(Resource *resource, wl_resource *popup)
{
    Q_UNUSED(resource);

    HWWaylandXdgPopup *xdgPopup = HWWaylandXdgPopup::fromResource(popup);
    if (xdgPopup) {
        auto *xdgPopupPrivate = HWWaylandXdgPopupPrivate::get(xdgPopup);
        xdgPopupPrivate->setParentLayerSurface(this);
        emit xdgPopupParentChanged(xdgPopup);
    }
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_ack_configure(Resource *resource, uint32_t serial)
{
    Q_UNUSED(resource);
    ConfigureEvent config;
    bool found = false;

    Q_FOREVER {
        if (pendingConfigures.isEmpty())
        {
            qDebug() << "Received an unexpected ack_configure!";
            return;
        }

        config = pendingConfigures.takeFirst();
        if (config.serial == serial)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        wl_resource_post_error(resource->handle, error_invalid_surface_state,
                               "wrong configure serial: %u", serial);
        return;
    }

    lastAckedConfigure = config;

    if (!configured)
    {
        configured = true;
        emit configuredChanged();
    }
}

void WlrLayerSurfaceV1::zwlr_layer_surface_v1_set_layer(Resource *resource, uint32_t layer)
{
    Q_UNUSED(resource);
    if (layer > QtWaylandServer::zwlr_layer_shell_v1::layer_overlay) {
        wl_resource_post_error(resource->handle,
                               QtWaylandServer::zwlr_layer_shell_v1::error_invalid_layer,
                               "invalid layer %d", layer);
        return;
    }

    qDebug() << layer;
    clientPending.layer = static_cast<WlrLayerShellV1::Layer>(layer);
}
