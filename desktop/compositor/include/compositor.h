// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QWaylandCompositor>
#include <QWaylandSurface>
#include <QWaylandView>
#include <QWaylandWlShellSurface>
#include "xdgshell.h"
#include <QWaylandXdgDecorationManagerV1>
#include <QTimer>
#include <QOpenGLTextureBlitter>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QTouchEvent>

#include <QtWaylandCompositor/qwaylandseat.h>
#include <QtWaylandCompositor/qwaylanddrag.h>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QOpenGLContext>

#define BUTTON_SPACING 5
#define BUTTON_WIDTH 18
#define BUTTONS_RIGHT_MARGIN 8

class FullscreenShell;
class QWaylandWlShell;
class QWaylandWlShellSurface;
class QWaylandXdgShell;
class OriginullMenuServer;
class QOpenGLTexture;
class SurfaceView;
class Surface;
class OutputWindow;
class Output;
class OriginullProtocol;
class AppMenuManager;
class AppMenu;
class PlasmaWindowManagement;
class WlrLayerShellV1;
class WlrLayerSurfaceV1;
class GtkShell;
class GtkSurface;
class QtShell;
class QtSurface;
class ShortcutManager;
class Compositor : public QWaylandCompositor
{
    Q_OBJECT
public:
    Compositor(bool is_sddm);
    ~Compositor() override;
    void create() override;

    void startRender();
    void endRender();
    QColor accentColor() const;
    QColor primaryBackgroundColor() const;
    Output* primaryOutput() const;
    bool legacyRender() const;
    bool useAnimations() const;

    void resetIdle();

    QList<Surface*> surfaceObjects() const { return m_surfaces; }
    QVector<Surface*> surfaceByZOrder() { return m_zorder; }
    QList<Surface*> desktopSurfaces() const { return m_desktops; }
    QList<SurfaceView*> views() const;
    QList<Surface*> surfaces() { return m_surfaces; }

    void raise(Surface *surface);
    void activate(Surface *surface);
    OriginullMenuServer* menuServerSurfaceObject() { return m_menuServer; }
    void handleMouseEvent(QWaylandView *target, QMouseEvent *me);
    void handleResize(SurfaceView *target, const QSize &initialSize, const QPoint &delta, int edge);
    void handleDrag(SurfaceView *target, QMouseEvent *me);

    QWaylandClient *popupClient() const;
    bool hasMenuServer();
    uint nextId();

    uint decorationSize() const;
    uint borderSize() const { return m_borderSize; }
    uint viewMode() const { return m_apperance; }

    PlasmaWindowManagement *windowManager() { return m_wndmgr; }
    WlrLayerShellV1 *layerShell() { return m_layerShell; }
    Output* outputAtPosition(const QPoint &pos);
    Output* outputFor(QWaylandOutput *wloutput);
    void resetLayerShellLayer(Surface *s);
    QList<Surface*> backgroundLayerSurfaces() { return m_layer_bg; }
    QList<Surface*> bottomLayerSurfaces() { return m_layer_bottom; }
    QList<Surface*> topLayerSurfaces() { return m_layer_top; }
    QList<Surface*> overlayLayerSurfaces() { return m_layer_overlay; }

    QString surfaceZOrderByUUID() const;
    void generateDesktopInfoLabelImage();
    QImage *desktopLabelImage();
    bool hasFullscreenSurface() const { return m_hasFullscreenSurface; }
    bool processHasTwilightMode(quint64 pid) const;
    bool isRunningLoginManager() const { return m_sddm; }
    QPointF correctedPosition(const QPointF &point);
    ShortcutManager* shortcuts();
protected:
    void adjustCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY);
    void recycleSurfaceObject(Surface *obj);
signals:
    void startMove();
    void startResize(int edge, bool anchored);
    void dragStarted(Surface *dragIcon);
    void frameOffset(const QPoint &offset);
    void settingsChanged();
public slots:
    void triggerRender();
    void lockSession();
protected slots:
    void surfaceHasContentChanged();
    void onStartMove(QWaylandSeat *seat);
    void onWlStartResize(QWaylandSeat *seat, QWaylandWlShellSurface::ResizeEdge edges);
    void onXdgStartResize(QWaylandSeat *seat, Qt::Edges edges);
    void startDrag();

    void onSurfaceCreated(QWaylandSurface *surface);
    void onMenuServerRequest(OriginullMenuServer *menu);
    void onDesktopRequest(QWaylandSurface *surface);
    void onWlShellSurfaceCreated(QWaylandWlShellSurface *wlShellSurface);
    void onXdgSurfaceCreated(HWWaylandXdgSurface *xdgSurface);
    void onXdgTopLevelCreated(HWWaylandXdgToplevel *toplevel, HWWaylandXdgSurface *surface);
    void onXdgPopupCreated(HWWaylandXdgPopup *popup, HWWaylandXdgSurface *surface);
    void onLayerSurfaceCreated(WlrLayerSurfaceV1 *layerSurface);
    void onGtkSurfaceCreated(GtkSurface *gtkSurface);
    void onQtSurfaceCreated(QtSurface *qtSurface);
    void onFullscreenSurfaceRequested(QWaylandSurface *surface);
    void onSetTransient(QWaylandSurface *parentSurface, const QPoint &relativeToParent, bool inactive);
    void onSetPopup(QWaylandSeat *seat, QWaylandSurface *parent, const QPoint &relativeToParent);

    void onSubsurfaceChanged(QWaylandSurface *child, QWaylandSurface *parent);
    void onSubsurfacePositionChanged(const QPoint &position);
    void updateCursor();
private slots:
    void appMenuCreated(AppMenu *m);
    void menuServerDestroyed();
    void configChanged();
    void loadSettings();
    void setupIdleTimer();
    void idleTimeout();
private:
    void sendWindowUUID(QUuid &uuid);
private:
    friend class CompositorApp;
    friend class OutputWindow; // TEMPORARY!
    friend class Surface;
    friend class Output;
    friend class OriginullProtocol;

    void addOutput(Output* output);
    void removeOutput(Output* output);
    SurfaceView* findView(const QWaylandSurface *s) const;
    Surface* findSurfaceObject(const QWaylandSurface *s) const;
private:
    // metrics (stored in settings)
    uint m_decorationSize = 30;
    uint m_borderSize = 1;
    uint m_apperance = 0;
    QColor m_accent;
    QColor m_desktop_bg;

    QList<Output*> m_outputs;
    uint m_id = 0;
    // Our list of surfaces
    QList<Surface*> m_surfaces;
    QVector<Surface*> m_zorder;

    // Our wl_shell protocol support
    QWaylandWlShell *m_wlShell = nullptr;
    // Our xdg_shell protocol support
    HWWaylandXdgShell *m_xdgShell = nullptr;
    // Our decoration protocol support
    QWaylandXdgDecorationManagerV1 *m_xdgDecoration = nullptr;
    // Hollywood Private UI Protocol support
    OriginullProtocol *m_hwPrivate = nullptr;
    // wlroots layer shell support
    WlrLayerShellV1 *m_layerShell = nullptr;
    // KDE Plasma window-management protocol
    PlasmaWindowManagement *m_wndmgr = nullptr;
    // KDE Plasma AppMenu protocol support (for dbusmenu)
    AppMenuManager *m_appMenu = nullptr;
    // GTK Shell protocol support
    GtkShell *m_gtk = nullptr;
    // Qt Shell protocol support
    QtShell *m_qt = nullptr;
    // fullscreen-shell protocol support
    FullscreenShell *m_fs = nullptr;

    // running under user 'sddm'
    bool m_sddm = false;

    // Our cursor
    Surface *m_cursorObject = nullptr;
    int m_cursorHotspotX;
    int m_cursorHotspotY;

    OriginullMenuServer *m_menuServer = nullptr;
    uint m_menuServerReserved = 0;
    QList<Surface*> m_desktops;

    // layer shell objects
    QList<Surface*> m_layer_bg;
    QList<Surface*> m_layer_bottom;
    QList<Surface*> m_layer_top;
    QList<Surface*> m_layer_overlay;

    QFileSystemWatcher *m_cfgwatch = nullptr;
    QString m_configfile;
    QPointF m_globalCursorPos;
    int m_resizeOldVal = 0;

    bool m_legacyExperience = false;

    QImage *m_dtlabel = nullptr;

    bool m_hasFullscreenSurface = false;
    Surface *m_fullscreenSurface = nullptr;

    // global hotkey / shortcuts
    ShortcutManager *m_shortcuts = nullptr;

    // idle tracking and timeouts
    uint m_timeout_display;
    uint m_timeout_sleep;
    uint m_lock_after_sleep;
    QTimer *m_timeout = nullptr;
    bool m_display_sleeping = false;
    bool m_idle_inhibit = false;
};

#endif // WINDOWCOMPOSITOR_H
