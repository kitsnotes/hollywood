// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// This class was derived from Liri
// (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef LAYERSHELL_H
#define LAYERSHELL_H

#include <QMargins>
#include <QLoggingCategory>
#include <QWaylandCompositorExtension>
#include <QWaylandOutput>
#include <QWaylandSurface>
#include <QtWaylandCompositor/QWaylandResource>

#include "qwayland-server-wlr-layer-shell-unstable-v1.h"

#include "xdgshell.h"

class WlrLayerSurfaceV1;
class WlrLayerShellV1
        : public QWaylandCompositorExtensionTemplate<WlrLayerShellV1>
        , public QtWaylandServer::zwlr_layer_shell_v1
{
    Q_OBJECT
public:
    WlrLayerShellV1(QWaylandCompositor *compositor);
    ~WlrLayerShellV1();
    enum Layer {
        BackgroundLayer = 0,
        BottomLayer = 1,
        TopLayer = 2,
        OverlayLayer = 3
    };
    Q_ENUM(Layer)

    void initialize() override;

    void closeAllSurfaces();
    static WlrLayerSurfaceV1* fromResource(wl_resource *resource);
signals:
    void layerSurfaceCreated(WlrLayerSurfaceV1 *layerSurface);
private:
    QMap<QWaylandSurface *, WlrLayerSurfaceV1 *> surfaces;

protected:

    void zwlr_layer_shell_v1_get_layer_surface(Resource *resource, uint32_t id,
                                               struct ::wl_resource *surfaceRes,
                                               struct ::wl_resource *outputRes,
                                               uint32_t layer,
                                               const QString &nameSpace) override;
};

class WlrLayerSurfaceV1
        : public QObject
        , public QtWaylandServer::zwlr_layer_surface_v1
{
    Q_OBJECT
private:
    explicit WlrLayerSurfaceV1(QWaylandSurface *surface,
                                      QWaylandOutput *output,
                                      WlrLayerShellV1::Layer layer,
                                      const QString &nameSpace,
                                      QObject *parent = nullptr);
    friend class WlrLayerShellV1;
public:
    enum Anchor {
        TopAnchor = 1,
        BottomAnchor = 2,
        LeftAnchor = 4,
        RightAnchor = 8
    };
    Q_ENUM(Anchor)
    Q_DECLARE_FLAGS(Anchors, Anchor)
    Q_FLAG(Anchors)

    enum KeyboardInteractivity {
        NoKeyboardInteractivity = 0,
        ExclusiveKeyboardInteractivity = 1,
        OnDemandKeyboardInteractivity = 2
    };
    Q_ENUM(KeyboardInteractivity)

    ~WlrLayerSurfaceV1() = default;
    quint32 sendConfigure(const QSize &size);
    quint32 sendConfigure(int width, int height);
    void close();

    QWaylandSurface *surface() const;
    QWaylandOutput *output() const;
    WlrLayerShellV1::Layer layer() const;
    QString nameSpace() const;

    QSize size() const;
    quint32 width() const;
    quint32 height() const;
    Anchors anchors() const;
    int exclusiveZone() const;
    QMargins margins() const;
    qint32 leftMargin() const;
    qint32 topMargin() const;
    qint32 rightMargin() const;
    qint32 bottomMargin() const;
    KeyboardInteractivity keyboardInteractivity() const;
    bool isMapped() const;
    bool isConfigured() const;

    struct State {
        State() = default;

        WlrLayerShellV1::Layer layer = WlrLayerShellV1::BackgroundLayer;
        QSize desiredSize = QSize(0, 0);
        WlrLayerSurfaceV1::Anchors anchors;
        int exclusiveZone = 0;
        QMargins margins;
        KeyboardInteractivity keyboardInteractivity = WlrLayerSurfaceV1::NoKeyboardInteractivity;
    };

    struct ConfigureEvent {
        ConfigureEvent() = default;
        ConfigureEvent(const QSize &incomingSize, quint32 incomingSerial)
            : serial(incomingSerial)
            , size(incomingSize)
        { }

        quint32 serial = 0;
        QSize size = QSize(0, 0);
    };

    ConfigureEvent lastSentConfigure() const;

    void unmap();

private:
    QWaylandSurface *m_surface = nullptr;
    QWaylandOutput *m_output = nullptr;
    QString m_namespace;

    bool added = false;
    bool configured = false;
    bool mapped = false;
    bool closed = false;

    State clientPending, serverPending, current;

    QVector<ConfigureEvent> pendingConfigures;
    ConfigureEvent lastAckedConfigure;
    static QWaylandSurfaceRole *role();

    static QWaylandSurfaceRole s_role;
signals:
    void layerChanged();
    void sizeChanged();
    void anchorsChanged();
    void exclusiveZoneChanged();
    void leftMarginChanged();
    void topMarginChanged();
    void rightMarginChanged();
    void bottomMarginChanged();
    void keyboardInteractivityChanged();
    void changed();
    void mappedChanged();
    void configuredChanged();
    void xdgPopupParentChanged(HWWaylandXdgPopup *popup);

protected:
    void zwlr_layer_surface_v1_set_size(Resource *resource, uint32_t width,
                                        uint32_t height) override;
    void zwlr_layer_surface_v1_set_anchor(Resource *resource,
                                          uint32_t anchor) override;
    void zwlr_layer_surface_v1_set_exclusive_zone(Resource *resource, int32_t zone) override;
    void zwlr_layer_surface_v1_set_margin(Resource *resource, int32_t top, int32_t right, int32_t bottom, int32_t left) override;
    void zwlr_layer_surface_v1_set_keyboard_interactivity(Resource *resource, uint32_t keyboard_interactivity) override;
    void zwlr_layer_surface_v1_get_popup(Resource *resource, struct ::wl_resource *popup) override;
    void zwlr_layer_surface_v1_ack_configure(Resource *resource, uint32_t serial) override;
    void zwlr_layer_surface_v1_set_layer(Resource *resource, uint32_t layer) override;
};

#endif // LAYERSHELL_H
