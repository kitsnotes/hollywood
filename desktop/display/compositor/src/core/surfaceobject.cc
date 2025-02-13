// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "surfaceobject.h"

#include "compositor.h"
#include "view.h"
#include "output.h"
#include "appmenu.h"
#include "wndmgmt.h"
#include "layershell.h"
#include "gtkshell.h"
#include "qtshell.h"
#include "originull.h"
#include "xwaylandshellsurface.h"

#include <QWaylandXdgDecorationManagerV1>
#include <QDateTime>
#include <QPainter>
#include <QStaticText>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>
#include "decoration.h"

#include <hollywood/hollywood.h>

#include "../libshell/include/desktopentry.h"

QLoggingCategory lcs("surface");
Q_LOGGING_CATEGORY(hwSurface, "compositor.surface")

Surface::Surface(QWaylandSurface *surface)
    : QObject(nullptr)
    , m_surfaceType(Unknown)
    , m_surface(surface)
    , m_ssdMgr(new ServerSideDecoration(this))
{
    m_uuid = QUuid::createUuid();
    m_id = hwComp->nextId();
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: Surface()", uuid);

    connect(m_surface, &QWaylandSurface::surfaceDestroyed, this, &Surface::surfaceDestroyed);
    connect(m_surface, &QWaylandSurface::hasContentChanged, hwComp, &Compositor::surfaceHasContentChanged);
    connect(m_surface, &QWaylandSurface::redraw, hwComp, &Compositor::triggerRender);    
    connect(m_surface, &QWaylandSurface::subsurfacePositionChanged, hwComp, &Compositor::onSubsurfacePositionChanged);
    connect(m_surface, &QWaylandSurface::sourceGeometryChanged, this, &Surface::onSurfaceSourceGeometryChanged);
    connect(m_surface, &QWaylandSurface::destinationSizeChanged, this, &Surface::onDestinationSizeChanged);
    connect(m_surface, &QWaylandSurface::bufferScaleChanged, this, &Surface::onBufferScaleChanged);
    connect(m_surface, &QWaylandSurface::childAdded, this, &Surface::onChildAdded);
    connect(m_surface, &QWaylandSurface::inhibitsIdleChanged, this, &Surface::inhibitsIdleChanged);

    // TODO: multi-monitor support
}

Surface::~Surface()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: ~Surface()", uuid);
    hwComp->removeIdleInhibit(this);
    if(m_loadTimer)
    {
        m_loadTimer->stop();
        m_loadTimer->deleteLater();
    }
    if(m_parentSurface)
        m_parentSurface->recycleChildSurfaceObject(this);

    if(m_ls_output)
        m_ls_output->removeLayerShellReservation(this);

    delete m_wndctl;
    //delete m_view;
    delete m_ssdMgr;
}

QWaylandSurface *Surface::surface() const
{
    return m_surface;
}

Surface *Surface::parentSurfaceObject() const { return m_parentSurface; }

bool Surface::isCursor() const
{
    if(surface())
        return surface()->isCursorSurface();

    return false;
}

bool Surface::isMinimized() const
{
    return m_minimized;
}

bool Surface::isMaximized() const
{
    return m_maximized;
}

bool Surface::isFullscreen() const { return m_fullscreen; }

bool Surface::canMaximize() const
{
    if(m_xdgTopLevel)
    {
        if(m_xdgTopLevel->maxSize() == QSize(-1,-1))
            return true;

        if(m_xdgTopLevel->maxSize().height() < primaryView()->output()->availableGeometry().height() &&
            m_xdgTopLevel->maxSize().width() < primaryView()->output()->availableGeometry().width())
            return false;

        return true;
    }

    if(m_gtk)
        return true;

    return false;
}

bool Surface::canMinimize() const
{
    if(m_canMinimize)
    {
        // if nothing is bound to plasma window management
        // then don't give the user a minimize option
        if(hwComp->windowManager()->resourceMap().isEmpty())
            return false;
    }
    return m_canMinimize;
}

bool Surface::canClose() const { return m_canClose; }

void Surface::sendClose()
{
    if(m_xdgTopLevel)
    {
        m_xdgTopLevel->sendClose();
        return;
    }

    if(m_qt)
    {
        m_qt->send_close();
        return;
    }
}

void Surface::setMaximized()
{
    if(m_xdgTopLevel)
    {
        emit m_xdgTopLevel->setMaximized();
        return;
    }
}

void Surface::unsetMaximized()
{
    if(m_xdgTopLevel)
    {
        emit m_xdgTopLevel->unsetMaximized();
        return;
    }
}

void Surface::setMinimized()
{
    if(m_xdgTopLevel)
    {
        emit m_xdgTopLevel->setMinimized();
        return;
    }
}

bool Surface::activated() const
{
    if(m_minimized)
        return false;

    return hwComp->raisedSurface() == this ? true : false;
}

QString Surface::windowTitle() const
{
    return m_windowTitle;
}

SurfaceView *Surface::viewForOutput(Output *o)
{
    if(m_viewList.contains(o))
        return m_viewList[o];

    return createViewForOutput(o);
}

SurfaceView *Surface::primaryView() const
{
    SurfaceView *view = nullptr;
    uint content = 0;

    if(m_viewList.count() == 1)
        return m_viewList.first();

    QMap<Output*, SurfaceView*>::const_iterator i;
    for (i = m_viewList.begin(); i != m_viewList.end(); ++i)
    {
        QRectF deco = decoratedRect();
        QRect r = deco.intersected(i.key()->geometry()).toRect();
        uint pixels = r.width() * r.height();
        if(pixels > content)
        {
            content = pixels;
            view = i.value();
        }
    }

    return view;
}

void Surface::onChildAdded(QWaylandSurface *child)
{
    Q_UNUSED(child)
}

Surface::SurfaceType Surface::surfaceType() const { return m_surfaceType; }

WlrLayerSurfaceV1::Anchors Surface::anchors()
{
    return m_layerSurface->anchors();
}

bool Surface::isSpecialShellObject() const
{
    if(m_surface == nullptr)
        return false;

    if(m_cursor)
        return true;

    return false;
}

bool Surface::hasWlShell() const { return m_wlShellSurface; }

bool Surface::hasPlasmaControl() const
{
    if(isSpecialShellObject())
        return false;

    return m_wndctl == nullptr ? false : true;
}

bool Surface::isXdgPopup() const
{
    return m_xdgPopup;
}

bool Surface::isGtkSurface() const
{
    return m_gtk == nullptr ? false : true;
}

bool Surface::isQtSurface() const
{
    return m_qt == nullptr ? false : true;
}

bool Surface::isLayerSurface() const
{
    return m_layerSurface == nullptr ? false : true;
}

QWaylandWlShellSurface *Surface::wlShellSurface() const { return m_wlShellSurface; }

HWWaylandXdgSurface *Surface::xdgSurface() const { return m_xdgSurface; }

HWWaylandXdgToplevel *Surface::xdgTopLevel() const { return m_xdgTopLevel; }

PlasmaWindowControl *Surface::plasmaControl() const { return m_wndctl; }

WlrLayerSurfaceV1 *Surface::layerSurface() const { return m_layerSurface; }

AppMenu *Surface::appMenu() const { return m_appMenu; }

GtkSurface *Surface::gtkSurface() const
{
    return m_gtk;
}

QtSurface *Surface::qtSurface() const
{
    return m_qt;
}

OriginullMenuServer *Surface::menuServer() const
{
    return m_menuServer;
}

void Surface::setPosition(const QPointF &pos)
{
    if(m_surfaceType == Popup)
    {
        m_surfacePosition = pos;
        return;
    }

    auto newpos = pos;
    if(m_ssd)
    {
        newpos.setX(newpos.x()-hwComp->borderSize());
        newpos.setY(newpos.y()-hwComp->decorationSize());
    }
    auto correctedPos = hwComp->correctedPosition(pos);
    if(m_ssd)
    {
        correctedPos.setX(correctedPos.x()+hwComp->borderSize());
        correctedPos.setY(correctedPos.y()+hwComp->decorationSize());
    }
    m_surfacePosition = correctedPos;
    if(m_qt != nullptr && m_qt_moveserial != 0)
    {
        auto x = m_surfacePosition.x();
        auto y = m_surfacePosition.y();
        m_qt->send_set_position(m_qt_moveserial, x, y);
    }

    if(m_xwl_shell)
        m_xwl_shell->sendPosition(m_surfacePosition);
}

uint Surface::shadowSize() const
{
    if(m_layerSurface)
        return 0;

    if(isMaximized() || isFullscreen())
        return 0;

    // GTK Shell does it's own shadowing
    if(m_gtk)
        return 0;

    if(surfaceType() == Popup)
        return 15;

    if(surfaceType() == TopLevel)
        return 45;

    return 0;
}

bool Surface::inhibitsIdle() const
{
    return surface()->inhibitsIdle();
}

QSize Surface::surfaceSize() const
{
    if(m_resize_animation)
    {
        // return our resize animation
        return m_resize_animation_size;
    }

    if(xdgSurface())
        return xdgSurface()->windowGeometry().size();
    return surface()->destinationSize();
}

QSize Surface::decoratedSize() const
{
    // The size of our decorated window
    QSize size;
    if(xdgSurface())
        size = xdgSurface()->windowGeometry().size()*surface()->bufferScale();
    else
        size = surface()->destinationSize()*surface()->bufferScale();

    if(m_ssd)
    {
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        bs = bs * surface()->bufferScale();
        ds = ds * surface()->bufferScale();
        size.setHeight(size.height()+bs+ds);
        size.setWidth(size.width()+bs+bs);
    }
    return size;
}

QSize Surface::renderSize() const
{
    // What we will be rendering: visible region + decorations + shadows
    auto size = decoratedSize();
    uint shadow = shadowSize();
    if(shadow > 1)
    {
        size.setHeight(size.height()+shadow*2);
        size.setWidth(size.width()+shadow*2);
    }
    return size;
}

QSize Surface::maximumSize() const
{
    if(m_xdgTopLevel)
        return m_xdgTopLevel->maxSize();

    return QSize();
}

QRectF Surface::decoratedRect() const
{
    auto pos = surfacePosition();
    if(m_ssd)
    {
        pos.setX(pos.x()-hwComp->borderSize());
        pos.setY(pos.y()-hwComp->decorationSize());
    }
    return QRectF(pos, decoratedSize());
}

QRectF Surface::closeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize();
    const int windowRight = decoratedRect().right() + 1;
    int bw = BUTTON_WIDTH*surface()->bufferScale();
    int bs = BUTTON_SPACING*surface()->bufferScale();
    int brm = BUTTONS_RIGHT_MARGIN*surface()->bufferScale();
    return QRectF(windowRight - bw - bs * 0 - brm,
                   decorationPosition().y()+(ds - bw) / 2, bw, bw);
}

QRectF Surface::maximizeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    const int windowRight = decoratedRect().right() + 1;
    int bw = BUTTON_WIDTH*surface()->bufferScale();
    int bs = BUTTON_SPACING*surface()->bufferScale();
    int brm = BUTTONS_RIGHT_MARGIN*surface()->bufferScale();
    return QRectF(windowRight - bw * 2 - bs * 1 - brm,
                   decorationPosition().y()+(ds - bw) / 2, bw, bw);
}

QRectF Surface::minimizeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize();
    const int windowRight = decoratedRect().right() + 1;
    int bw = BUTTON_WIDTH*surface()->bufferScale();
    int bs = BUTTON_SPACING*surface()->bufferScale();
    int brm = BUTTONS_RIGHT_MARGIN*surface()->bufferScale();
    return QRectF(windowRight - bw * 3 - bs * 2 - brm,
                  decorationPosition().y()+(ds - bw) / 2, bw, bw);
}

QRectF Surface::titleBarRect() const
{
    auto point = decorationPosition();
    auto bs = hwComp->borderSize()*surface()->bufferScale();
    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    auto returnVal = QRectF(point, QSize(surfaceSize().width()+(bs*2), ds));

    return returnVal;
}

QUuid Surface::uuid() const { return m_uuid; }

uint Surface::id() const { return m_id; }

SurfaceView* Surface::createViewForOutput(Output *o)
{
    if(m_viewList.contains(o))
        return m_viewList[o];

    SurfaceView *view = new SurfaceView(this);
    if(view)
    {
        view->setAllowDiscardFrontBuffer(true);
        connect(view, &QWaylandView::surfaceDestroyed, this, &Surface::viewSurfaceDestroyed);
        view->setOutput(o);
        view->setSurface(m_surface);
        m_viewList.insert(o, view);

        return view;
    }
    qWarning() << "Surface::createView() failed. returning nullptr";
    return nullptr;
}

void Surface::setAppMenu(AppMenu *m)
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: setting appmenu", uuid);
    m_appMenu = m;
    if(hwComp->raisedSurface() == this)
        QTimer::singleShot(2, [this](){hwComp->activate(this);});

    if(isShellDesktop())
        QTimer::singleShot(2, [this](){hwComp->activate(this);});
}


QPointF Surface::parentPosition() const
{
    return m_parentSurface ?
                (m_parentSurface->surfacePosition() + m_parentSurface->parentPosition())
                           : QPointF();
}

QPointF Surface::mapToSurface(const QPointF &pos) const
{
    if(!surface() || surface()->destinationSize().isEmpty())
        return pos / surface()->bufferScale();

    auto newpos = QPointF(pos.x() / surface()->bufferScale(), pos.y() / surface()->bufferScale());
    return newpos;
}

QList<Surface *> Surface::childSurfaceObjects() const { return m_children; }

QList<Surface *> Surface::childXdgSurfaceObjects() const { return m_xdgChildren; }

bool Surface::surfaceReadyToRender() const
{
    return m_surfaceInit;
}

bool Surface::serverDecorated() const
{
    // right now we don't decorate gtk-shells
    // we only decorate xdg-shell or qt-shell surfaces
    if(m_xwl_shell != nullptr)
        return true;

    if(m_xdgTopLevel == nullptr && m_qt == nullptr)
        return false;

    if(isSpecialShellObject())
        return false;

    if(m_layerSurface)
        return false;

    // if we are xdg top level see if we have negoitated decorations
    if(m_xdgTopLevel)
        return m_ssd;

    if(m_qt)
        return m_ssd;

    return false;
}

QPointF Surface::surfacePosition() const
{
    if(m_parentSurface == nullptr)
    {
        if(m_layerSurface != nullptr)
            return m_surfacePosition;
        else
            return m_surfacePosition;
        //hwComp->correctedPosition(m_surfacePosition);
    }

    if(m_parentSurface->layerSurface())
         return m_parentSurface->surfacePosition() + m_surfacePosition;

    return m_surfacePosition; //hwComp->correctedPosition(m_parentSurface->surfacePosition() + m_surfacePosition);
}

QPointF Surface::decorationPosition() const
{
    auto bs = hwComp->borderSize()*surface()->bufferScale();
    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    auto point = surfacePosition();
    point.setX(point.x()-bs);
    point.setY(point.y()-ds);
    return point;
}

void Surface::setParentSurfaceObject(Surface *parent)
{
    // this is only used for pop-up / transient windows
    // this is NOT USED for dialogs/top level child windows
    m_parentSurface = parent;
    m_parentSurface->addChildSurfaceObject(this);
    // remove ourself from the compositor's top level zorder list
    hwComp->m_zorder.removeOne(this);
    m_surfaceType = Surface::Popup;
}

void Surface::createPlasmaWindowControl()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: create plasma window control", uuid);
    m_wndctl = hwComp->m_wndmgr->createWindowControl(this);
    m_wndctl->setCloseable(true);
    m_wndctl->setMinimizable(true);
    m_wndctl->setMaximizable(true);
    connect(m_wndctl, &PlasmaWindowControl::minimizedRequested, this, &Surface::toggleMinimize);
    connect(m_wndctl, &PlasmaWindowControl::maximizedRequested, this, &Surface::toggleMaximize);
    connect(m_wndctl, &PlasmaWindowControl::activeRequested, this, &Surface::toggleActive);
    connect(m_wndctl, &PlasmaWindowControl::closeRequested, this, &Surface::sendClose);
    QIcon icon = QIcon::fromTheme(QLatin1String("process-working"));
    m_wndctl->setIcon(icon);
}

QPointF Surface::renderPosition() const
{
    if(isFullscreen())
        return surfacePosition();

    QRectF calculated(surfacePosition(), surfaceSize());

    // account for the server decoration
    if(m_ssd)
    {
        calculated.setTop(calculated.top()-(hwComp->decorationSize()*surface()->bufferScale()));
        calculated.setLeft(calculated.left()-(hwComp->borderSize()*surface()->bufferScale()));
        calculated.setRight(calculated.right()+(hwComp->borderSize()*surface()->bufferScale()));
        calculated.setBottom(calculated.bottom()+(hwComp->borderSize()*surface()->bufferScale()));
    }

    // account for shadows if we are using them
    uint shadow = shadowSize();
    if(shadow > 1 && !isMaximized())
    {
        calculated.setTop(calculated.top()-shadow);
        calculated.setLeft(calculated.left()-shadow);
        calculated.setRight(calculated.right()+shadow);
        calculated.setBottom(calculated.bottom()+shadow);
    }

    return calculated.topLeft();
}

void Surface::addChildSurfaceObject(Surface *child)
{
    m_children.append(child);
}

void Surface::addXdgChildSurfaceObject(Surface *child)
{
    m_xdgChildren.append(child);
}

void Surface::recycleChildSurfaceObject(Surface *child)
{
    m_children.removeOne(child);
}

void Surface::setLayerShellParent(Surface *surface)
{
    m_parentSurface = surface;
}

void Surface::handleLayerShellPopupPositioning()
{
    auto popup = m_xdgPopup;
    auto parent = m_parentSurface->m_layerSurface;
    if (parent->anchors() & WlrLayerSurfaceV1::TopAnchor)
    {
        float xPos = popup->anchorRect().x();
        int yPos = popup->anchorRect().y();
        setPosition(QPointF(xPos,yPos));
        return;
    }

    if (parent->anchors() & WlrLayerSurfaceV1::BottomAnchor)
    {
        auto bs = m_parentSurface->surface()->bufferScale();
        float xPos = popup->anchorRect().x();
        if(bs > 1 && xPos > 0)
            xPos = popup->anchorRect().x() - (popup->positionerSize().width());
        int height = popup->positionerSize().height()*bs;
        height = height - height - height;
        setPosition(QPointF(xPos,height));
        return;
    }

    return;
}

void Surface::onSurfaceSourceGeometryChanged()
{
    //m_viewport = surface()->sourceGeometry();
}

void Surface::onDestinationSizeChanged()
{    
    // if we are initializing an xdg toplevel we should set a
    // better initial position
    if(!m_surfaceInit)
    {
        if(!hwComp->isRunningLoginManager() && m_xdgTopLevel != nullptr)
        {
            if(m_isShellDesktop)
            {
                m_surfaceInit = true;
                return;
            }
            auto xdg = xdgSurface()->windowGeometry();
            auto size = xdg.size()*surface()->bufferScale();
            auto point = surfacePosition().toPoint();
            /*if(xdg.topLeft() != QPoint(0,0))
            {
                point.setX(point.x()+xdg.left());
                point.setY(point.y()+xdg.top());
            }*/
            auto rect = QRect(point, size);
            // TODO: dual monitor support - grab output of cursor
            auto outputRect = hwComp->defaultOutput()->availableGeometry();
            int x = 50;
            int y = 50;
            if(outputRect.width() > rect.width())
            x = (outputRect.width() - rect.width())/2;

                if(outputRect.height() > rect.height())
                    y = (outputRect.height() - rect.height())/2;

                 setPosition(QPointF(x,y));
        }

        if(m_xdgTopLevel)
        {
            QTimer::singleShot(250, [this]() {
                hwComp->raise(this);
            });
        }
        m_surfaceInit = true;
    }

    hwComp->triggerRender();
}

void Surface::onBufferScaleChanged()
{
    hwComp->triggerRender();
}

void Surface::onXdgStartResize(QWaylandSeat *seat, Qt::Edges edges)
{
    hwComp->onXdgStartResize(seat, edges);
}

void Surface::startMove()
{
    m_moving = true;
    if(m_qt == nullptr)
        return;

    m_qt_moveserial = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
}

void Surface::startResize()
{
    if(m_qt == nullptr)
        return;

    m_qt_sizeserial = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
}

void Surface::endResize()
{
    if(m_qt != nullptr)
        m_qt->send_configure(m_qt_sizeserial);
    m_qt_sizeserial = 0;

}

void Surface::endMove()
{
    m_moving = false;
    if(m_qt != nullptr)
        m_qt->send_configure(m_qt_moveserial);
    m_qt_moveserial = 0;
}

void Surface::decorationModeChanged()
{
    if(m_xdgTopLevel->decorationMode() == HWWaylandXdgToplevel::ServerSideDecoration)
        m_ssd = true;
    else
        m_ssd = false;
    hwComp->triggerRender();
}

void Surface::onQtWindowTitleChanged(const QString &title)
{
    m_windowTitle = title;
    if(m_wndctl)
        m_wndctl->setTitle(m_windowTitle);

    hwComp->triggerRender();
}

void Surface::onQtShellActivationRequest()
{
    hwComp->raise(this);
    hwComp->triggerRender();
}

void Surface::onQtShellReposition(const QPoint &pos)
{
    m_surfacePosition = pos;
    hwComp->triggerRender();
}

void Surface::onQtShellSetSize(const QSize &size)
{
    m_qt_size = size;
    emit geometryChanged();
    hwComp->triggerRender();
}

void Surface::onQtWindowFlagsChanged(const Qt::WindowFlags &f)
{
    m_qt_wndflags = f;

    if(f.testFlag(Qt::Window) && !f.testFlag(Qt::Popup) && !f.testFlag(Qt::FramelessWindowHint))
    {
        m_ssd = true;
        createPlasmaWindowControl();
        m_wndctl->setTitle(m_windowTitle);
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        m_qt->send_set_frame_margins(bs,bs,ds,bs);
        m_surfaceType = TopLevel;
    }

    if(f.testFlag(Qt::Dialog) && !f.testFlag(Qt::Popup))
    {
        m_ssd = true;
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        m_qt->send_set_frame_margins(bs,bs,ds,bs);
        m_surfaceType = TopLevel;
    }

    if(f.testFlag(Qt::Tool))
    {
        m_ssd = true;
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        m_qt->send_set_frame_margins(bs,bs,ds,bs);
        m_surfaceType = TopLevelTool;
    }

    if(f.testFlag(Qt::Popup) && !f.testFlag(Qt::Tool))
        m_surfaceType = Popup;

    // TODO: ghetto assumption but nothing beyond shellfm should be using qt-shell
    if(f.testFlag(Qt::X11BypassWindowManagerHint))
    {
        m_isShellDesktop = true;
        m_surfaceType = TopLevel;
    }

    if(!f.testFlag(Qt::WindowMinMaxButtonsHint))
        m_showMinMaxBtn = false;

    if(!f.testFlag(Qt::WindowCloseButtonHint))
    {
        m_canClose = false;
    }

    m_surfaceInit = true;
    hwComp->triggerRender();
}

void Surface::removeXdgTopLevelChild(Surface *s)
{
    if(m_xdgChildren.contains(s))
    {
        auto uuid = s->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
        auto myuuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
        qCDebug(hwSurface, "%s: removing as top level child from %s", uuid, myuuid);
        m_xdgChildren.removeOne(s);
    }
}

void Surface::setXWaylandShell(XWaylandShellSurface *surface)
{
    // we can only call this once
    if(m_xwl_shell != nullptr)
        return;

    m_xwl_shell = surface;
    m_surface = m_xwl_shell->surface();
    m_surfaceType = TopLevel;
    m_ssd = true;
    m_windowTitle = m_xwl_shell->title();
    m_xwl_shell->setWmState(XWaylandShellSurface::NormalState);
    m_xwl_shell->setWorkspace(0);
    setPosition(QPoint(45,45));
    m_xwl_shell->sendPosition(QPointF(45,45));
    //m_xwl_shell->sendConfigure(QRect(15,15,350,350));
    emit iconChanged();
    hwComp->activate(this);
    hwComp->raise(this);
}

void Surface::activate()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: activate", uuid);
    if(m_xdgTopLevel)
    {
        QList<HWWaylandXdgToplevel::State> states = m_xdgTopLevel->states();
        states.append(HWWaylandXdgToplevel::ActivatedState);
        m_xdgTopLevel->sendConfigure(surfaceSize(), states);
        if(m_wndctl)
            m_wndctl->setActive(true);
    }
    hwComp->defaultSeat()->setKeyboardFocus(surface());
    hwComp->defaultSeat()->setMouseFocus(surface()->primaryView());
}

void Surface::deactivate()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: deactivate", uuid);
    if(m_xdgTopLevel)
    {
        QList<HWWaylandXdgToplevel::State> states = m_xdgTopLevel->states();
        states.removeOne(HWWaylandXdgToplevel::ActivatedState);
        m_xdgTopLevel->sendConfigure(surfaceSize(), states);
    }

    if(m_wndctl)
        m_wndctl->setActive(false);
}

void Surface::toggleMinimize()
{
    if(canMinimize())
    {
        if(m_minimized)
            unsetMinimized();
        else
            setMinimized();
    }
}

void Surface::toggleMaximize()
{
    if(canMaximize())
    {
        if(m_maximized)
            unsetMaximized();
        else
            setMaximized();
    }
}

void Surface::toggleActive()
{
    hwComp->raise(this);
}

void Surface::setAnimatedSurfaceSize(QSize size)
{
    m_resize_animation_size = size;
}

void Surface::createXdgShellSurface(HWWaylandXdgSurface *surface)
{
    m_xdgSurface = surface;
    connect(m_xdgSurface, &HWWaylandXdgSurface::windowGeometryChanged, this, &Surface::onXdgWindowGeometryChanged);
}

void Surface::createXdgTopLevelSurface(HWWaylandXdgToplevel *topLevel)
{
    m_xdgTopLevel = topLevel;
    m_surfaceType = TopLevel;

    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::decorationModeChanged, this, &Surface::decorationModeChanged);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::startMove, hwComp, &Compositor::onStartMove);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::startResize, this, &Surface::onXdgStartResize);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::setMaximized, this, &Surface::onXdgSetMaximized);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::setFullscreen, this, &Surface::onXdgSetFullscreen);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::setMinimized, this, &Surface::onXdgSetMinimized);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::unsetMaximized, this, &Surface::onXdgUnsetMaximized);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::unsetFullscreen, this, &Surface::onXdgUnsetFullscreen);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::parentToplevelChanged, this, &Surface::onXdgParentTopLevelChanged);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::titleChanged, this, &Surface::onXdgTitleChanged);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::appIdChanged, this, &Surface::onXdgAppIdChanged);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::maximizedChanged, this, &Surface::completeXdgConfigure);
    connect(m_xdgTopLevel, &HWWaylandXdgToplevel::fullscreenChanged, this, &Surface::completeXdgConfigure);
    m_loadTimer = new QTimer(this);
    m_loadTimer->setSingleShot(true);
    m_loadTimer->setInterval(20);
    // a temporary bit so we can at least know where we are
    setPosition(QPoint(0,0));
    connect(m_loadTimer, &QTimer::timeout, this, [this, topLevel](){
        if(m_parentTopLevelSurface == nullptr)
        {
            if(!isShellDesktop())
            {
                createPlasmaWindowControl();
                m_wndctl->setTitle(topLevel->title());
                if(m_menuServer)
                    m_menuServer->setTopWindowForMenuServer(this);

            }
        }
    });
    m_loadTimer->start();
}

void Surface::createXdgPopupSurface(HWWaylandXdgPopup *popup)
{
    m_xdgPopup = popup;
    m_surfaceType = Popup;
    Surface *parentSurface = nullptr;

    if(popup->parentXdgSurface() != nullptr)
        parentSurface = hwComp->findSurfaceObject(popup->parentXdgSurface()->surface());

    if(parentSurface != nullptr)
        setParentSurfaceObject(parentSurface);

    if(popup->parentXdgSurface() == nullptr)
        return;


    auto rect = popup->anchorRect();

    float yPos = 0;
    if(popup->anchorEdges() & Qt::TopEdge)
        yPos = parentSurface->surfacePosition().y()+(rect.top()*parentSurface->surface()->bufferScale());
    else if(popup->anchorEdges() & Qt::BottomEdge)
        yPos = rect.bottom()*parentSurface->surface()->bufferScale();
    else
        yPos = (rect.top() + rect.height() / 2)*parentSurface->surface()->bufferScale();

    float xPos = 0;
    if(popup->anchorEdges() & Qt::LeftEdge)
        xPos = parentSurface->surfacePosition().x()+(rect.left()*parentSurface->surface()->bufferScale());
    else if(popup->anchorEdges() & Qt::RightEdge)
        xPos = rect.right()*parentSurface->surface()->bufferScale();
    else
        xPos = (rect.left() + rect.width() / 2)*parentSurface->surface()->bufferScale();

    setPosition(QPointF(xPos,yPos));

    m_surfaceInit = true;
}

void Surface::createGtkSurface(GtkSurface *gtk)
{
    m_gtk = gtk;
    m_surfaceType = TopLevel;
    m_surfaceInit = true;
}

void Surface::createQtSurface(QtSurface *qt)
{
    m_qt = qt;
    connect(m_qt, &QtSurface::reposition, this, &Surface::onQtShellReposition);
    connect(m_qt, &QtSurface::setSize, this, &Surface::onQtShellSetSize);
    connect(m_qt, &QtSurface::windowTitleChanged, this, &Surface::onQtWindowTitleChanged);
    connect(m_qt, &QtSurface::windowFlagsChanged, this, &Surface::onQtWindowFlagsChanged);

    m_surfaceType = TopLevel;
    QTimer::singleShot(50, [this]() {
        hwComp->raise(this);
    });
}

void Surface::createMenuServer(OriginullMenuServer *menu)
{
    m_menuServer = menu;
}

void Surface::surfaceDestroyed()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: surfaceDestroyed", uuid);

    if(m_wndctl)
        m_wndctl->destroy();
    hwComp->recycleSurfaceObject(this);
    hwComp->triggerRender();
}

void Surface::viewSurfaceDestroyed()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: viewSurfaceDestroyed", uuid);
    bool requestRecycle = false;
    SurfaceView *view = qobject_cast<SurfaceView*>(sender());
    m_viewList.remove(m_viewList.key(view));
    if(m_viewList.isEmpty())
        requestRecycle = true;

    delete view;

    if(requestRecycle)
        hwComp->recycleSurfaceObject(this);

    hwComp->triggerRender();
}

void Surface::onXdgSetMaximized()
{
    m_minimized = false;
    m_maximized = true;
    m_priorNormalPos = m_surfacePosition;
    m_prior_normal_size = surfaceSize()/surface()->bufferScale();
    m_resize_animation_start_size = surfaceSize();
    auto size = primaryView()->output()->availableGeometry();
    if(this->serverDecorated())
    {
        size.setHeight((size.height()/surface()->bufferScale()) - hwComp->decorationSize() - (hwComp->borderSize()*2));
        size.setWidth((size.width()/surface()->bufferScale()) - hwComp->borderSize()*2);
    }
    m_maximized_complete = false;
    m_animation_minmax_target_size = size.size();
    m_xdgTopLevel->sendMaximized(size.size());
    if(m_wndctl)
        m_wndctl->setMaximized(true);
}

void Surface::onXdgUnsetMaximized()
{
    m_maximized = false;
    m_resize_animation_start_point = surfacePosition();
    m_resize_animation_start_size = surfaceSize();
    m_animation_minmax_target_size = m_prior_normal_size;
    m_xdgTopLevel->sendUnmaximized();

    if(m_wndctl)
        m_wndctl->setMaximized(false);
}

void Surface::onXdgSetMinimized()
{
    m_minimized = true;
    hwComp->triggerRender();
    if(m_wndctl)
        m_wndctl->setMinimized(true);
    hwComp->raiseNextInLine();
}

void Surface::completeXdgConfigure()
{
    if(m_resize_animation)
        return;

    if(m_maximized)
    {
        auto pos = primaryView()->output()->availableGeometry().topLeft();
        if(!hwComp->useAnimations() || m_maximized_complete)
        {
            setPosition(pos);
            hwComp->triggerRender();
        }
        else
        {
            if(!m_resize_animation)
            {
                auto group = new QParallelAnimationGroup;
                QPropertyAnimation *posAnimation = new QPropertyAnimation(this, "surfacePosition");
                posAnimation->setStartValue(m_priorNormalPos);
                posAnimation->setEndValue(pos);
                posAnimation->setDuration(190);
                posAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                connect(posAnimation, &QPropertyAnimation::valueChanged, hwComp, &Compositor::triggerRender);
                QPropertyAnimation *sizeAnimation = new QPropertyAnimation(this, "animatedSurfaceSize");
                sizeAnimation->setStartValue(m_resize_animation_start_size);
                sizeAnimation->setEndValue(m_animation_minmax_target_size);
                sizeAnimation->setDuration(190);
                sizeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

                connect(posAnimation, &QPropertyAnimation::valueChanged, [this](){
                    m_xdgTopLevel->sendResizing(m_resize_animation_size);
                    hwComp->triggerRender();
                });
                m_resize_animation = true;
                group->addAnimation(posAnimation);
                group->addAnimation(sizeAnimation);
                group->start(QPropertyAnimation::DeleteWhenStopped);
                connect(group, &QParallelAnimationGroup::finished, [this]() {
                    m_maximized_complete = true;
                    m_resize_animation = false;

                    QList<int> states;
                    states << HWWaylandXdgToplevel::State::MaximizedState;
                    m_xdgTopLevel->sendConfigure(m_animation_minmax_target_size, states);
                    hwComp->triggerRender();
                });
            }
        }
    }
    else if(m_fullscreen)
    {
        // TODO: use cache from onXdgSetFullscreen
        setPosition(primaryView()->output()->position());
    }
    else
    {
        // restore our window to prior position
        if(!hwComp->useAnimations())
        {
            setPosition(m_priorNormalPos);
            hwComp->triggerRender();
        }
        else
        {
            if(!m_resize_animation)
            {
                auto group = new QParallelAnimationGroup;
                QPropertyAnimation *posAnimation = new QPropertyAnimation(this, "surfacePosition");
                posAnimation->setStartValue(m_resize_animation_start_point);
                posAnimation->setEndValue(m_priorNormalPos);
                posAnimation->setDuration(190);
                posAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                connect(posAnimation, &QPropertyAnimation::valueChanged, hwComp, &Compositor::triggerRender);
                QPropertyAnimation *sizeAnimation = new QPropertyAnimation(this, "animatedSurfaceSize");
                sizeAnimation->setStartValue(m_resize_animation_start_size);
                sizeAnimation->setEndValue(m_animation_minmax_target_size);
                sizeAnimation->setDuration(190);
                sizeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                connect(posAnimation, &QPropertyAnimation::valueChanged, [this](){
                    m_xdgTopLevel->sendConfigure(m_resize_animation_size, m_xdgTopLevel->states());
                    hwComp->triggerRender();
                });
                m_resize_animation = true;
                group->addAnimation(posAnimation);
                group->addAnimation(sizeAnimation);
                group->start(QPropertyAnimation::DeleteWhenStopped);
                connect(group, &QParallelAnimationGroup::finished, [this]() {
                    m_resize_animation = false;
                    hwComp->triggerRender();
                });
            }
        }
    }
}

void Surface::unsetMinimized()
{
    m_minimized = false;
    if(m_wndctl)
        m_wndctl->setMinimized(false);
    hwComp->triggerRender();
}

void Surface::onXdgSetFullscreen(QWaylandOutput* clientPreferredOutput)
{
    m_minimized = false;
    m_fullscreen = true;
    // TODO: cache this
    QWaylandOutput *outputToFullscreen = clientPreferredOutput
            ? clientPreferredOutput
            : primaryView()->output();

    m_xdgSurface->toplevel()->sendFullscreen(outputToFullscreen->geometry().size());    
}

void Surface::onXdgUnsetFullscreen()
{
    m_fullscreen = false;
    onXdgUnsetMaximized();
}

void Surface::onXdgTitleChanged()
{
    m_windowTitle = m_xdgTopLevel->title();
    if(m_wndctl)
        m_wndctl->setTitle(m_windowTitle);
}

void Surface::onXdgParentTopLevelChanged()
{
    m_parentTopLevelSurface =
            hwComp->findSurfaceObject(m_xdgTopLevel->parentToplevel()->xdgSurface()->surface());
    m_parentTopLevelSurface->addXdgChildSurfaceObject(this);
    //hwComp->m_zorder.removeOne(this);
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    auto tl = m_parentTopLevelSurface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwSurface, "%s: xdg parent toplevel changed: %s", uuid, tl);
}

void Surface::onXdgAppIdChanged()
{
    m_appid = m_xdgTopLevel->appId();
    auto de = LSDesktopEntry::findDesktopFile(m_appid+".desktop");
    if(!de.isEmpty())
    {
        LSDesktopEntry e;
        if(e.load(de))
        {
            m_icontheme = e.value("Icon").toString();
            m_icon = e.icon();
            if(m_wndctl)
            {
                if(!m_icontheme.isEmpty())
                    m_wndctl->setThemedIcon(m_icontheme);
            }
            emit iconChanged();
        }
    }
}

void Surface::onXdgWindowGeometryChanged()
{
}

// Layer Shell Functions

void Surface::createLayerShellSurface(WlrLayerSurfaceV1 *surface)
{
    m_layerSurface = surface;
    connect(m_layerSurface, &WlrLayerSurfaceV1::layerChanged, this, &Surface::onLayerChanged);
    connect(m_layerSurface, &WlrLayerSurfaceV1::exclusiveZoneChanged, this, &Surface::onExclusiveZoneChanged);
    connect(m_layerSurface, &WlrLayerSurfaceV1::anchorsChanged, this, &Surface::onAnchorsChanged);
    connect(m_layerSurface, &WlrLayerSurfaceV1::sizeChanged, this, &Surface::onLayerShellSizeChanged);
    connect(m_layerSurface, &WlrLayerSurfaceV1::xdgPopupParentChanged, this, &Surface::onLayerShellXdgPopupParentChanged);

    m_surfaceType = LayerShell;
    hwComp->resetLayerShellLayer(this);
}

void Surface::onAnchorsChanged()
{
    recalculateLayerShellAnchorPosition();
}

void Surface::onExclusiveZoneChanged()
{
    if(m_layerSurface->exclusiveZone() > 0)
    {
        auto output = hwComp->outputFor(primaryView()->output());
        m_ls_output = output;
        output->reserveLayerShellRegion(this);
    }
    else
    {
        auto output = hwComp->outputFor(primaryView()->output());
        output->removeLayerShellReservation(this);
        m_ls_output = nullptr;
    }
}

void Surface::inhibitsIdleChanged()
{
    auto uuid = m_uuid.toString(QUuid::WithoutBraces).toUtf8().data();
    if(inhibitsIdle())
    {
        hwComp->addIdleInhibit(this);
        qCDebug(hwSurface, "%s: is inhibiting idle", uuid);
    }
    else
    {
        hwComp->removeIdleInhibit(this);
        qCDebug(hwSurface, "%s: idle inhibitor removed", uuid);
    }
}

void Surface::reconfigureLayerSurface()
{
    m_layerSurface->sendConfigure(m_ls_size);
    m_surfaceInit = true;
}

void Surface::onLayerChanged()
{
    hwComp->resetLayerShellLayer(this);
    m_surfaceInit = true;
}

void Surface::onLayerShellSizeChanged()
{
    if(!m_layerSurface->size().isValid())
        return;
    m_ls_size = m_layerSurface->size();
    recalculateLayerShellAnchorPosition();
    reconfigureLayerSurface();
}

void Surface::onLayerShellXdgPopupParentChanged(HWWaylandXdgPopup *popup)
{
    auto popupSurface = popup->xdgSurface()->surface();
    hwComp->findSurfaceObject(popupSurface)->setLayerShellParent(this);
    hwComp->findSurfaceObject(popupSurface)->handleLayerShellPopupPositioning();
    hwComp->m_zorder.removeOne(hwComp->findSurfaceObject(popupSurface));
    addChildSurfaceObject(hwComp->findSurfaceObject(popupSurface));

}

void Surface::recalculateLayerShellAnchorPosition()
{
    if(!m_ls_size.isValid())
        return;

    auto anchors = m_layerSurface->anchors();
    auto view = primaryView();

    if(anchors == WlrLayerSurfaceV1::TopAnchor)
    {
        // center on the top (OK)
        auto pos = primaryView()->output()->availableGeometry().topLeft();
        auto x = floor((primaryView()->output()->availableGeometry().width() -
                        m_ls_size.width()*surface()->bufferScale())/2);
        pos.setX(x);
        m_surfacePosition = pos;
    }
    if(anchors == WlrLayerSurfaceV1::LeftAnchor)
    {
        // center on the left (OK)
        auto pos = primaryView()->output()->availableGeometry().topLeft();
        auto y = floor((primaryView()->output()->availableGeometry().height()
                        - m_ls_size.height()*surface()->bufferScale())/2);
        pos.setY(y);
        m_surfacePosition = pos;
    }
    if(anchors == WlrLayerSurfaceV1::RightAnchor)
    {
        // center on the right (OK)
        auto pos = primaryView()->output()->availableGeometry().topRight();
        auto y = floor((primaryView()->output()->availableGeometry().height()
                        - m_ls_size.height()*surface()->bufferScale())/2);
        pos.setY(y);
        pos.setX(pos.x() - m_ls_size.width()*surface()->bufferScale());
        m_surfacePosition = pos;
    }
    if(anchors == WlrLayerSurfaceV1::BottomAnchor)
    {
        // center on the bottom (OK)
        auto pos = primaryView()->output()->availableGeometry().bottomLeft();
        auto x = floor((primaryView()->output()->availableGeometry().width() - m_ls_size.width())/2);
        pos.setX(x);
        pos.setY(pos.y() - m_ls_size.height()*surface()->bufferScale());
        m_surfacePosition = pos;
    }

    if(anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::RightAnchor)
    {
        // spread out on the top (OK)
        m_ls_size.setWidth(view->output()->availableGeometry().width());
        m_surfacePosition = primaryView()->output()->availableGeometry().topLeft();
    }
    if(anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::RightAnchor)
    {
        // spread out on the bottom
        m_ls_size.setWidth(view->output()->availableGeometry().width());
        auto pos = primaryView()->output()->availableGeometry().bottomLeft();
        pos.setY(pos.y() - m_ls_size.height());
        m_surfacePosition = pos;
    }

    if(anchors == WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::BottomAnchor)
    {
        // spread out on the left
        m_ls_size.setHeight(view->output()->availableGeometry().height());
        m_surfacePosition = primaryView()->output()->availableGeometry().topLeft();
    }
    if(anchors == WlrLayerSurfaceV1::RightAnchor+WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::BottomAnchor)
    {
        // spread out on the right
        auto pos = primaryView()->output()->availableGeometry().topRight();
        pos.setX(pos.x()-m_ls_size.width()*surface()->bufferScale());
        m_surfacePosition = pos;
        m_ls_size.setHeight(view->output()->availableGeometry().height());
        return;
    }

    if(anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::LeftAnchor)
    {
        // corner top-left (OK)
        auto pos = primaryView()->output()->availableGeometry().topLeft();
        pos.setX(pos.x()+m_layerSurface->leftMargin());
        pos.setY(pos.y()+m_layerSurface->topMargin());
        m_surfacePosition = pos;
        return;
    }
    if(anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::RightAnchor)
    {
        // corner top-right
        auto pos = primaryView()->output()->availableGeometry().topRight();
        pos.setX(pos.x()-m_layerSurface->size().width()-m_layerSurface->rightMargin());
        pos.setY(pos.y()+m_layerSurface->topMargin());
        m_surfacePosition = pos;
    }

    if(anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::LeftAnchor)
    {
        // corner bottom-left
        auto pos = primaryView()->output()->availableGeometry().bottomLeft();
        pos.setY(pos.y()-m_layerSurface->size().height()-m_layerSurface->bottomMargin());
        m_surfacePosition = pos;
    }
    if(anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::RightAnchor)
    {
        // corner bottom-right
        auto pos = primaryView()->output()->availableGeometry().bottomRight();
        pos.setX(pos.x()-m_ls_size.width()*surface()->bufferScale());
        pos.setY(pos.y()-m_ls_size.height()*surface()->bufferScale());
        m_surfacePosition = pos;
    }
}

