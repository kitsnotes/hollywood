// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SURFACEOBJECT_H
#define SURFACEOBJECT_H

#include <QObject>
#include <QWaylandCompositor>
#include <QWaylandSurface>
#include <QWaylandView>
#include <QWaylandWlShellSurface>
#include "xdgshell.h"
#include <QTimer>
#include <QOpenGLTextureBlitter>
#include <QWaylandSeat>
#include <QUuid>
#include <QImage>

#include "layershell.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwSurface)

class QWaylandWlShell;
class QWaylandWlShellSurface;
class HWWaylandXdgShell;
class QOpenGLTexture;
class SurfaceView;
class AppMenu;
class PlasmaWindowControl;
class OriginullMenuServer;
class GtkSurface;
class QtSurface;
class Output;
/* This class represents a holder for a Wayland Surface object
 * and all of the items that a surface may contain, including,
 * protocols like xdg_shell, wl_shell, the GTK flavors of the month,
 * et cetera.
 *
 * This also holds our views.  Note that there may be multiple
 * views per surface.  This would happen in the case of multi-monitor
 * setups, etc...
 * */

class Surface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF surfacePosition READ surfacePosition WRITE setPosition)
    Q_PROPERTY(QSize animatedSurfaceSize READ surfaceSize WRITE setAnimatedSurfaceSize)
public:
    enum SurfaceType {
        Unknown = 0,
        TopLevel,
        Transient,
        Popup,
        TopLevelTool,
        MenuServer,
        LayerShell,
        Desktop
    };
    Q_ENUM(SurfaceType)

    explicit Surface(QWaylandSurface *surface);
    ~Surface();

    QWaylandSurface* surface() const;
    // This is only used for popup windows
    Surface* parentSurfaceObject() const;
    QPointF parentPosition() const;
    QPointF mapToSurface(const QPointF &pos) const;
    QList<Surface*> childSurfaceObjects() const;
    QList<Surface*> childXdgSurfaceObjects() const;

    QString appId() const { return m_appid; }
    QString themedIcon() const { return m_icontheme; }

    bool isInitialized() const { return m_surfaceInit; }
    bool isSubsurface() const { return m_subsurface; }
    void setSubsurface(const bool subsurface) { m_subsurface = subsurface; }
    bool surfaceReadyToRender() const;
    bool serverDecorated() const;
    QPointF renderPosition() const;
    QPointF surfacePosition() const;
    QPointF decorationPosition() const;
    void setPosition(const QPointF &pos);
    uint shadowSize() const;
    bool inhibitsIdle() const;

    QSize surfaceSize() const;
    QSize decoratedSize() const;
    QSize renderSize() const;

    QRectF windowRect() const;
    QRectF decoratedRect() const;
    QRectF closeButtonRect() const;
    QRectF maximizeButtonRect() const;
    QRectF minimizeButtonRect() const;
    QRectF titleBarRect() const;
    QUuid uuid() const;
    uint id() const;
    bool isCursor() const;
    bool isMinimized() const;
    bool isMaximized() const;
    bool isFullscreen() const;

    bool canMaximize() const;
    bool canMinimize() const;
    bool canClose() const;
    void setMaximized();
    void unsetMaximized();
    void setMinimized();
    bool activated() const;
    QString windowTitle() const;
    SurfaceView* viewForOutput(Output* o);
    SurfaceView* primaryView() const;
    SurfaceType surfaceType() const;
    WlrLayerSurfaceV1::Anchors anchors();
    // Protocol Extensions
    bool isSpecialShellObject() const;
    bool hasWlShell() const;
    bool hasPlasmaControl() const;
    bool isXdgPopup() const;
    bool isGtkSurface() const;
    bool isQtSurface() const;
    bool isLayerSurface() const;
    QWaylandWlShellSurface* wlShellSurface() const;
    HWWaylandXdgSurface* xdgSurface() const;
    HWWaylandXdgToplevel* xdgTopLevel() const;
    PlasmaWindowControl* plasmaControl() const;
    WlrLayerSurfaceV1* layerSurface() const;
    AppMenu* appMenu() const;
    GtkSurface* gtkSurface() const;
    QtSurface* qtSurface() const;
    OriginullMenuServer* menuServer() const;
    void startMove();
    void startResize();
    void endResize();
    void endMove();
    QImage* decorationImage();
    Surface *topLevelSurface() const { return m_parentTopLevelSurface; }
    Surface *xdgTopLevelParent() const { return m_parentTopLevelSurface; }
    void removeXdgTopLevelChild(Surface *s);
    QIcon icon() const { return m_icon; }
    bool isFullscreenShell() const { return m_fullscreenShell; }
    void setTwilight(bool twilight) { m_twilight = twilight; }
    bool isShellDesktop() const { return m_isShellDesktop; }
public slots:
    void activate();
    void deactivate();
    void toggleMinimize();
    void toggleMaximize();
    void toggleActive();
    void sendClose();
    void recalculateLayerShellAnchorPosition();
    void onLayerShellSizeChanged();
    void invalidateCachedDecoration();
protected:
    // only callable by Compostior
    void setAnimatedSurfaceSize(QSize size);
    void setParentSurfaceObject(Surface *parent);
    void createXdgShellSurface(HWWaylandXdgSurface *surface);
    void createXdgTopLevelSurface(HWWaylandXdgToplevel *topLevel);
    void createXdgPopupSurface(HWWaylandXdgPopup *popup);
    void createLayerShellSurface(WlrLayerSurfaceV1 *surface);
    void createGtkSurface(GtkSurface *gtk);
    void createQtSurface(QtSurface *qt);
    void createMenuServer(OriginullMenuServer *menu);
    void addChildSurfaceObject(Surface* child);
    void addXdgChildSurfaceObject(Surface* child);
    void recycleChildSurfaceObject(Surface* child);
    void adjustPostionX(float x);
    void adjustPostionY(float y);
    void renderDecoration();
    void updatePlasmaStateFlags() const;
    void setLayerShellParent(Surface *surface);
    void handleLayerShellPopupPositioning();
private slots:
    void onChildAdded(QWaylandSurface *child);
    void onSurfaceSourceGeometryChanged();
    void onDestinationSizeChanged();
    void onBufferScaleChanged();
    void onXdgStartResize(QWaylandSeat *seat, Qt::Edges edges);
    void onXdgSetMaximized();
    void onXdgUnsetMaximized();
    void onXdgSetMinimized();
    void unsetMinimized();
    void onXdgSetFullscreen(QWaylandOutput *output);
    void onXdgUnsetFullscreen();
    void onXdgTitleChanged();
    void onXdgParentTopLevelChanged();
    void onXdgAppIdChanged();
    void onXdgWindowGeometryChanged();
    void completeXdgConfigure();
    void surfaceDestroyed();
    void viewSurfaceDestroyed();
    void reconfigureLayerSurface();
    void onLayerChanged();
    void onLayerShellXdgPopupParentChanged(HWWaylandXdgPopup *popup);
    void onAnchorsChanged();
    void onExclusiveZoneChanged();

    void decorationModeChanged();
    void onQtWindowTitleChanged(const QString&);
    void onQtShellActivationRequest();
    void onQtShellReposition(const QPoint &pos);
    void onQtShellSetSize(const QSize &size);
    void onQtWindowFlagsChanged(const Qt::WindowFlags &f);

signals:
    void primaryViewChanged();
    void geometryChanged();
private:
    friend class Compositor;
    friend class SurfaceView;
    friend class OutputWindow;
    SurfaceView* createViewForOutput(Output *o);
    void setAppMenu(AppMenu *m);
    void createPlasmaWindowControl();
    void recalculateRenderPosition();
    QUuid m_uuid;
    uint m_id;

    bool m_surfaceInit = false;
    // cursor surface?
    bool m_cursor = false;

    bool m_active = false;

    bool m_subsurface = false;

    bool m_canMaximize = true;
    bool m_canMinimize = true;

    bool m_showMinMaxBtn = true;
    bool m_canClose = true;

    bool m_minimized = false;
    bool m_maximized = false;
    bool m_fullscreen = false;

    // tracking for animation
    bool m_maximized_complete = true;

    bool m_ssd = false;

    bool m_isShellDesktop = false;

    bool m_moving = false;
    bool m_resize_animation = false;
    QPointF m_resize_animation_start_point;

    SurfaceType m_surfaceType;

    bool m_fullscreenShell = false;

    QRectF m_viewport;

    QSize m_ls_size;
    QSize m_qt_size;

    QSize m_animation_minmax_target_size;

    Qt::WindowFlags m_qt_wndflags;
    uint m_qt_moveserial;
    uint m_qt_sizeserial;

    QString m_windowTitle;

    // the point at where the actual wayland surface is visible
    QPointF m_surfacePosition = QPointF(0,0);
    QSize m_resize_animation_size;
    QSize m_resize_animation_start_size;
    // Store our previous restore state size
    QSize m_prior_normal_size;

    // used for restoring window
    QPointF m_priorNormalPos;

    QString m_appid;
    QString m_icontheme;

    QWaylandSurface *m_surface = nullptr;
    Surface *m_parentSurface = nullptr;
    Surface *m_parentTopLevelSurface = nullptr;
    QList<Surface*> m_children;
    QList<Surface*> m_xdgChildren;

    QMap<Output*, SurfaceView*> m_viewList;

    QWaylandWlShellSurface *m_wlShellSurface = nullptr;
    HWWaylandXdgToplevel *m_xdgTopLevel = nullptr;
    HWWaylandXdgSurface *m_xdgSurface = nullptr;
    HWWaylandXdgPopup *m_xdgPopup = nullptr;
    WlrLayerSurfaceV1 *m_layerSurface = nullptr;
    PlasmaWindowControl *m_wndctl = nullptr;
    AppMenu *m_appMenu = nullptr;
    OriginullMenuServer *m_ms = nullptr;
    GtkSurface *m_gtk = nullptr;
    QtSurface *m_qt = nullptr;

    OriginullMenuServer *m_menuServer = nullptr;
    QImage *m_ssdimg = nullptr;

    Output *m_ls_output = nullptr;

    QIcon m_icon = QIcon();

    bool m_twilight = false;

    QTimer* m_loadTimer = nullptr;
};

#endif // SURFACEOBJECT_H
