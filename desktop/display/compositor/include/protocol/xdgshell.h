// Hollywood Wayland Compositor
// (C) 2021-2024 Originull Software
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QtWaylandCompositor/QWaylandCompositorExtension>
#include <QtWaylandCompositor/QWaylandResource>
#include <QtWaylandCompositor/QWaylandShell>
#include <QtWaylandCompositor/QWaylandShellSurface>
#include <QtWaylandCompositor/QWaylandSurface>
//#include <QtWaylandCompositor/qwaylandquickchildren.h>
#include <QtWaylandCompositor/private/qwaylandcompositorextension_p.h>
#include <QtWaylandCompositor/private/qwaylandshell_p.h>
#include <QtWaylandCompositor/private/qwayland-server-xdg-shell.h>
#include <QtWaylandCompositor/private/qwaylandxdgdecorationv1_p.h>
#include <QtCore/QSet>
#include <QtWaylandCompositor/private/qwaylandxdgshell_p.h>
#include <QtCore/QRect>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwXdgShell)

struct wl_resource;

class QWaylandClient;
class QWaylandOutput;
class QWaylandSeat;
class QWaylandSurface;
class QWaylandSurfaceRole;
class HWWaylandXdgShellPrivate;
class HWWaylandXdgSurface;
class HWWaylandXdgSurfacePrivate;
class HWWaylandXdgToplevel;
class HWWaylandXdgToplevelPrivate;
class HWWaylandXdgPopup;
class HWWaylandXdgPopupPrivate;
class QWaylandXdgPositioner;
class WlrLayerSurfaceV1;

class HWWaylandXdgShell : public QWaylandShellTemplate<HWWaylandXdgShell>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HWWaylandXdgShell)
public:
    explicit HWWaylandXdgShell();
    explicit HWWaylandXdgShell(QWaylandCompositor *compositor);

    void initialize() override;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

public Q_SLOTS:
    uint ping(QWaylandClient *client);

Q_SIGNALS:
    void xdgSurfaceCreated(HWWaylandXdgSurface *xdgSurface);
    void toplevelCreated(HWWaylandXdgToplevel *toplevel, HWWaylandXdgSurface *xdgSurface);
    void popupCreated(HWWaylandXdgPopup *popup, HWWaylandXdgSurface *xdgSurface);
    void pong(uint serial);

private Q_SLOTS:
    void handleSeatChanged(QWaylandSeat *newSeat, QWaylandSeat *oldSeat);
    void handleFocusChanged(QWaylandSurface *newSurface, QWaylandSurface *oldSurface);
};

class  HWWaylandXdgSurface : public QWaylandShellSurfaceTemplate<HWWaylandXdgSurface>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HWWaylandXdgSurface)
#if QT_CONFIG(wayland_compositor_quick)
    Q_WAYLAND_COMPOSITOR_DECLARE_QUICK_CHILDREN(HWWaylandXdgSurface)
#endif
    Q_PROPERTY(HWWaylandXdgShell *shell READ shell NOTIFY shellChanged)
    Q_PROPERTY(QWaylandSurface *surface READ surface NOTIFY surfaceChanged)
    Q_PROPERTY(HWWaylandXdgToplevel *toplevel READ toplevel NOTIFY toplevelCreated)
    Q_PROPERTY(HWWaylandXdgPopup *popup READ popup NOTIFY popupCreated)
    Q_PROPERTY(QRect windowGeometry READ windowGeometry NOTIFY windowGeometryChanged)
    Q_MOC_INCLUDE("qwaylandsurface.h")

public:
    explicit HWWaylandXdgSurface();
    explicit HWWaylandXdgSurface(HWWaylandXdgShell* xdgShell, QWaylandSurface *surface, const QWaylandResource &resource);

    Q_INVOKABLE void initialize(HWWaylandXdgShell* xdgShell, QWaylandSurface *surface, const QWaylandResource &resource);

    Qt::WindowType windowType() const override;

    HWWaylandXdgShell *shell() const;
    QWaylandSurface *surface() const;
    HWWaylandXdgToplevel *toplevel() const;
    HWWaylandXdgPopup *popup() const;
    QRect windowGeometry() const;

    static const struct wl_interface *interface();
    static QByteArray interfaceName();
    static HWWaylandXdgSurface *fromResource(::wl_resource *resource);
    QWaylandQuickShellIntegration *createIntegration(QWaylandQuickShellSurfaceItem *item) override;

Q_SIGNALS:
    void shellChanged();
    void surfaceChanged();
    void toplevelCreated();
    void popupCreated();
    void windowGeometryChanged();

private:
    void initialize() override;

private Q_SLOTS:
    void handleSurfaceSizeChanged();
    void handleBufferScaleChanged();
};

class  HWWaylandXdgToplevel : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HWWaylandXdgToplevel)
    Q_PROPERTY(HWWaylandXdgSurface *xdgSurface READ xdgSurface CONSTANT)
    Q_PROPERTY(HWWaylandXdgToplevel *parentToplevel READ parentToplevel NOTIFY parentToplevelChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString appId READ appId NOTIFY appIdChanged)
    Q_PROPERTY(QSize maxSize READ maxSize NOTIFY maxSizeChanged)
    Q_PROPERTY(QSize minSize READ minSize NOTIFY minSizeChanged)
    Q_PROPERTY(QList<int> states READ statesAsInts NOTIFY statesChanged)
    Q_PROPERTY(bool maximized READ maximized NOTIFY maximizedChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY fullscreenChanged)
    Q_PROPERTY(bool resizing READ resizing NOTIFY resizingChanged)
    Q_PROPERTY(bool activated READ activated NOTIFY activatedChanged)
    Q_PROPERTY(bool modal READ isModal NOTIFY modalChanged FINAL REVISION(6,8))
    Q_PROPERTY(enum DecorationMode decorationMode READ decorationMode NOTIFY decorationModeChanged)

public:
    enum State : uint {
        MaximizedState  = 1,
        FullscreenState = 2,
        ResizingState   = 3,
        ActivatedState  = 4
    };
    Q_ENUM(State)

    enum DecorationMode {
        ClientSideDecoration = 1,
        ServerSideDecoration = 2,
    };
    Q_ENUM(DecorationMode)

    explicit HWWaylandXdgToplevel(HWWaylandXdgSurface *xdgSurface, QWaylandResource &resource);
    ~HWWaylandXdgToplevel() override;

    HWWaylandXdgSurface *xdgSurface() const;
    HWWaylandXdgToplevel *parentToplevel() const;

    QString title() const;
    QString appId() const;
    QSize maxSize() const;
    QSize minSize() const;
    QList<HWWaylandXdgToplevel::State> states() const;
    bool maximized() const;
    bool fullscreen() const;
    bool resizing() const;
    bool activated() const;
    DecorationMode decorationMode() const;

    Q_INVOKABLE QSize sizeForResize(const QSizeF &size, const QPointF &delta, Qt::Edges edges) const;
    uint sendConfigure(const QSize &size, const QList<State> &states);
    Q_INVOKABLE uint sendConfigure(const QSize &size, const QList<int> &states);
    Q_INVOKABLE void sendClose();
    Q_INVOKABLE uint sendMaximized(const QSize &size);
    Q_INVOKABLE uint sendUnmaximized(const QSize &size = QSize(0, 0));
    Q_INVOKABLE uint sendFullscreen(const QSize &size);
    Q_INVOKABLE uint sendResizing(const QSize &maxSize);

    static QWaylandSurfaceRole *role();
    static HWWaylandXdgToplevel *fromResource(::wl_resource *resource);

    bool isModal() const;

Q_SIGNALS:
    void parentToplevelChanged();
    void titleChanged();
    void appIdChanged();
    void maxSizeChanged();
    void minSizeChanged();
    void startMove(QWaylandSeat *seat);
    void startResize(QWaylandSeat *seat, Qt::Edges edges);
    void statesChanged();
    void maximizedChanged();
    void fullscreenChanged();
    void resizingChanged();
    void activatedChanged();

    void showWindowMenu(QWaylandSeat *seat, const QPoint &localSurfacePosition);
    void setMaximized();
    void unsetMaximized();
    void setFullscreen(QWaylandOutput *output);
    void unsetFullscreen();
    void setMinimized();

    void decorationModeChanged();

    Q_REVISION(6, 8) void modalChanged();

private:
    QList<int> statesAsInts() const;
    void setModal(bool newModal);
    friend class HWWaylandXdgDialogV1;
};

class  HWWaylandXdgPopup : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HWWaylandXdgPopup)
    Q_PROPERTY(HWWaylandXdgSurface *xdgSurface READ xdgSurface CONSTANT)
    Q_PROPERTY(HWWaylandXdgSurface *parentXdgSurface READ parentXdgSurface CONSTANT)
    Q_PROPERTY(QRect configuredGeometry READ configuredGeometry NOTIFY configuredGeometryChanged)

    // Positioner properties
    Q_PROPERTY(QRect anchorRect READ anchorRect CONSTANT)
    Q_PROPERTY(Qt::Edges anchorEdges READ anchorEdges CONSTANT)
    Q_PROPERTY(Qt::Edges gravityEdges READ gravityEdges CONSTANT)
    Q_PROPERTY(Qt::Orientations slideConstraints READ slideConstraints CONSTANT)
    Q_PROPERTY(Qt::Orientations flipConstraints READ flipConstraints CONSTANT)
    Q_PROPERTY(Qt::Orientations resizeConstraints READ resizeConstraints CONSTANT)
    Q_PROPERTY(QPoint offset READ offset CONSTANT)
    Q_PROPERTY(QSize positionerSize READ positionerSize CONSTANT)
    Q_PROPERTY(QPoint unconstrainedPosition READ unconstrainedPosition CONSTANT)
public:
    HWWaylandXdgSurface *xdgSurface() const;
    HWWaylandXdgSurface *parentXdgSurface() const;
    QRect configuredGeometry() const;

    // Positioner properties
    QRect anchorRect() const;
    Qt::Edges anchorEdges() const ;
    Qt::Edges gravityEdges() const ;
    Qt::Orientations slideConstraints() const;
    Qt::Orientations flipConstraints() const;
    Qt::Orientations resizeConstraints() const;
    QPoint offset() const;
    QSize positionerSize() const;
    QPoint unconstrainedPosition() const;

    Q_INVOKABLE uint sendConfigure(const QRect &geometry);
    Q_REVISION(1, 14) Q_INVOKABLE void sendPopupDone();

    static QWaylandSurfaceRole *role();
    static HWWaylandXdgPopup *fromResource(struct ::wl_resource *resource);
Q_SIGNALS:
    void configuredGeometryChanged();

private:
    explicit HWWaylandXdgPopup(HWWaylandXdgSurface *xdgSurface, HWWaylandXdgSurface *parentXdgSurface,
                              QWaylandXdgPositioner *positioner, QWaylandResource &resource);
    friend class HWWaylandXdgSurfacePrivate;
};
