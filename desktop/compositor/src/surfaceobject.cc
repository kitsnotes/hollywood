// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "surfaceobject.h"

#include "compositor.h"
#include "view.h"
#include "output.h"
#include "appmenu.h"
#include "wndmgmt.h"
#include "layershell.h"
#include "hwc.h"
#include "gtkshell.h"
#include "qtshell.h"

#include <QWaylandXdgDecorationManagerV1>
#include <QDateTime>
#include <QPainter>
#include <QStaticText>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include <hollywood/hollywood.h>

#include "../libshell/include/desktopentry.h"

Surface::Surface(QWaylandSurface *surface)
    : QObject(nullptr)
    ,  m_surfaceType(Unknown)
    ,  m_surface(surface)
{
    m_uuid = QUuid::createUuid();
    m_id = hwComp->nextId();
    connect(m_surface, &QWaylandSurface::surfaceDestroyed, this, &Surface::surfaceDestroyed);
    connect(m_surface, &QWaylandSurface::hasContentChanged, hwComp, &Compositor::surfaceHasContentChanged);
    connect(m_surface, &QWaylandSurface::redraw, hwComp, &Compositor::triggerRender);    
    connect(m_surface, &QWaylandSurface::subsurfacePositionChanged, hwComp, &Compositor::onSubsurfacePositionChanged);
    connect(m_surface, &QWaylandSurface::sourceGeometryChanged, this, &Surface::onSurfaceSourceGeometryChanged);
    connect(m_surface, &QWaylandSurface::destinationSizeChanged, this, &Surface::onDestinationSizeChanged);
    connect(m_surface, &QWaylandSurface::bufferScaleChanged, this, &Surface::onBufferScaleChanged);

    // TODO: multi-monitor suppot
}

Surface::~Surface()
{
    if(m_parentSurface)
        m_parentSurface->recycleChildSurfaceObject(this);

    if(m_ls_output)
        m_ls_output->removeLayerShellReservation(this);

    delete m_wndctl;
    //delete m_view;
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

bool Surface::canMaximize() const { return m_canMaximize; }

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

    if(m_xdgTopLevel)
        return m_xdgTopLevel->activated();

    /*if(m_qt)
        return m_qt->h();*/

    return false;
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
        QRect r = deco.intersected(i.key()->wlOutput()->geometry()).toRect();
        uint pixels = r.width() * r.height();
        if(pixels > content)
        {
            content = pixels;
            view = i.value();
        }
    }

    return view;
}

Surface::SurfaceType Surface::surfaceType() const { return m_surfaceType; }

WlrLayerSurfaceV1::Anchors Surface::anchors()
{
    return m_layerSurface->anchors();
}

bool Surface::isSpecialShellObject() const
{
    if(surface())
        if(surface()->isCursorSurface())
            return true;

    if(m_surfaceType == Desktop)
        return true;

    return false;
}

bool Surface::hasWlShell() const { return m_wlShellSurface; }

bool Surface::hasPlasmaControl() const
{
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
    m_surfacePosition = hwComp->correctedPosition(pos);

    if(m_qt != nullptr && m_qt_moveserial != 0)
    {
        auto x = m_surfacePosition.x();
        auto y = m_surfacePosition.y();
        m_qt->send_set_position(m_qt_moveserial, x, y);
    }
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

QSize Surface::surfaceSize() const
{
    if(m_resize_animation)
    {
        // return our resize animation
        return m_resize_animation_size;
    }

    if(m_fullscreenShell)
        return surface()->bufferSize();

    if(m_layerSurface)
        return m_ls_size;

    if(m_qt)
    {
        if(m_qt_size.isEmpty())
            return surface()->bufferSize();
        else
            return m_qt_size;
    }

    if(m_xdgSurface)
        return surface()->bufferSize();

    return surface()->bufferSize();
}

QSize Surface::decoratedSize() const
{
    auto size = surfaceSize();
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
    auto size = decoratedSize();
    uint shadow = shadowSize();
    if(shadow > 1)
    {
        size.setHeight(size.height()+shadow*2);
        size.setWidth(size.width()+shadow*2);
    }
    return size;
}

QRectF Surface::windowRect() const
{
    return QRectF(surfacePosition(), QSizeF(surfaceSize()));
}

QRectF Surface::decoratedRect() const
{
    auto pos = surfacePosition();
    if(m_ssd)
    {
        pos.setX(pos.x()-hwComp->borderSize()*surface()->bufferScale());
        pos.setY(pos.y()-hwComp->decorationSize()*surface()->bufferScale());
    }
    return QRectF(pos, decoratedSize());
}

QRectF Surface::closeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    const int windowRight = decoratedRect().right() + 1;
    return QRectF(windowRight - BUTTON_WIDTH - BUTTON_SPACING * 0 - BUTTONS_RIGHT_MARGIN,
                   decorationPosition().y()+(ds - BUTTON_WIDTH) / 2, BUTTON_WIDTH, BUTTON_WIDTH);
}

QRectF Surface::maximizeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    const int windowRight = decoratedRect().right() + 1;
    return QRectF(windowRight - BUTTON_WIDTH * 2 - BUTTON_SPACING * 1 - BUTTONS_RIGHT_MARGIN,
                   decorationPosition().y()+(ds - BUTTON_WIDTH) / 2, BUTTON_WIDTH, BUTTON_WIDTH);
}

QRectF Surface::minimizeButtonRect() const
{
    if(!serverDecorated())
        return QRectF();

    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    const int windowRight = decoratedRect().right() + 1;
    return QRectF(windowRight - BUTTON_WIDTH * 3 - BUTTON_SPACING * 2 - BUTTONS_RIGHT_MARGIN,
                  decorationPosition().y()+(ds - BUTTON_WIDTH) / 2, BUTTON_WIDTH, BUTTON_WIDTH);
}

QRectF Surface::titleBarRect() const
{
    auto point = decorationPosition();
    auto bs = hwComp->borderSize()*surface()->bufferScale();
    auto ds = hwComp->decorationSize()*surface()->bufferScale();
    return QRectF(point, QSize(surfaceSize().width()+(bs*2), ds));
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
        connect(view, &QWaylandView::surfaceDestroyed, this, &Surface::viewSurfaceDestroyed);

        //connect(m_surface, &QWaylandSurface::offsetForNextFrame, view, &ACView::onOffsetForNextFrame);

        view->setOutput(o->wlOutput());
        view->setSurface(m_surface);
        m_viewList.insert(o, view);

        return view;
    }
    qDebug() << "ACSurfaceObject::createView() failed. returning nullptr";
    return nullptr;
}

void Surface::setAppMenu(AppMenu *m)
{
    m_appMenu = m;
}


QPointF Surface::parentPosition() const
{
    return m_parentSurface ?
                (m_parentSurface->surfacePosition() + m_parentSurface->parentPosition())
              : QPointF();
}

QList<Surface *> Surface::childSurfaceObjects() const { return m_children; }

bool Surface::surfaceReadyToRender() const
{
    return m_surfaceInit;
}

bool Surface::serverDecorated() const
{
    // right now we don't decorate gtk-shells
    // we only decorate xdg-shell or qt-shell surfaces
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
            return hwComp->correctedPosition(m_surfacePosition);
    }

    if(m_parentSurface->layerSurface())
         return m_parentSurface->surfacePosition() + m_surfacePosition;

    return hwComp->correctedPosition(m_parentSurface->surfacePosition() + m_surfacePosition);
}

QPointF Surface::decorationPosition() const
{
    auto bs = hwComp->borderSize();
    auto ds = hwComp->decorationSize();
    auto point = m_surfacePosition;
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
    m_wndctl = hwComp->m_wndmgr->createWindowControl(this);
    m_wndctl->setCloseable(true);
    m_wndctl->setMinimizable(true);
    m_wndctl->setMaximizable(true);
    connect(m_wndctl, &PlasmaWindowControl::minimizedRequested, this, &Surface::toggleMinimize);
    connect(m_wndctl, &PlasmaWindowControl::maximizedRequested, this, &Surface::toggleMaximize);
    connect(m_wndctl, &PlasmaWindowControl::activeRequested, this, &Surface::toggleActive);
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

void Surface::adjustPostionX(float x)
{
    m_surfacePosition.setX(x);
}

void Surface::adjustPostionY(float y)
{
    m_surfacePosition.setY(y);
}

void Surface::renderDecoration()
{
    if(m_ssdimg != nullptr)
        delete m_ssdimg;

    if(!m_ssd)
        return;

    m_ssdimg = new QImage(decoratedRect().size().toSize(),
                          QImage::Format_RGBA8888);
    QPainter p(m_ssdimg);

    bool do_dark = false;
    if(hwComp->viewMode() == 1)
        do_dark = true;

    if(hwComp->viewMode() == 2 && m_twilight)
        do_dark = true;

    auto fg = QColor(HOLLYWOOD_TEXTCLR_LIGHT);
    auto bg = QColor(HOLLYWOOD_WNDCLR_LIGHT);
    bg.setAlpha(255);
    auto fg_inactive = QColor(HOLLYWOOD_TEXTCLR_LIGHT);
    auto stroke = QColor(HOLLYWOOD_WNDCLR_DARK);

    if(do_dark)
    {
        fg = QColor(HOLLYWOOD_TEXTCLR_DARK);
        bg = QColor(HOLLYWOOD_WNDCLR_DARK);
        fg_inactive = QColor(HOLLYWOOD_TEXTCLR_DARK);
        stroke = QColor(HOLLYWOOD_WNDCLR_LIGHT);
    }

    bool active = activated();

    auto bs = hwComp->borderSize()*surface()->bufferScale();
    auto ds = hwComp->decorationSize()*surface()->bufferScale();

    auto btnsp = 3*surface()->bufferScale(); // TODO: metrics
    int btnsz = 22*surface()->bufferScale();

    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(decoratedRect(), QColor(255,255,255,0));

    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QRect wg = QRect(QPoint(0,0),
             QPoint(surfaceSize().width()+bs+bs, surfaceSize().height()+bs+ds));

    QRect clips[] =
    {
        // left top width height
        QRect(wg.left(), wg.top(), wg.width(), ds), // title bar
        QRect(wg.left(), wg.bottom() - bs, wg.width(), bs), // bottom line
        QRect(wg.left(), ds, bs+bs, wg.height() + ds + bs+bs), // left line
        QRect((wg.right() - bs), wg.top() + ds, bs+bs+bs, wg.height() - bs) // right line
    };

    p.setRenderHint(QPainter::Antialiasing);

    // Title bar
    QPainterPath roundedRect;
    roundedRect.addRoundedRect(wg, 3, 3);
    for (int i = 0; i < 4; ++i) {
        p.save();
        p.setClipRect(clips[i]);
        p.fillPath(roundedRect, bg);
        p.restore();
    }
    p.save();
    wg.adjust(1,1,2,-1);
    p.restore();

    // Window icon
    QIcon icon = QIcon::fromTheme("wayland");
    if(!m_icon.isNull())
        icon = m_icon;
    QRectF iconRect(wg.left(), wg.top(), btnsz, btnsz);
    iconRect.adjust(bs + btnsp, 4,
                    bs + btnsp, 4);
    //QIcon myIcon = QIcon::fromTheme(obj->xdgTopLevel()->appId(), icon);
    if (!icon.isNull())
        icon.paint(&p, iconRect.toRect());

    // Window title
    QString windowTitleText = windowTitle();
    if (!windowTitleText.isEmpty())
    {
        QStaticText m_windowTitle;
        if (m_windowTitle.text() != windowTitleText) {
            m_windowTitle.setText(windowTitleText);
            m_windowTitle.prepare();
        }

        QFont font = p.font();
        auto psz = 12*surface()->bufferScale();
        font.setPixelSize(psz);
        p.setFont(font);
        QFontMetrics m(p.font());
        QPoint start(iconRect.topLeft().toPoint());
        if(!icon.isNull())
            start.setX(iconRect.x()+iconRect.width()+btnsp);
        start.setY(start.y()+m.height());
        p.save();
        p.setPen(active ? fg : fg_inactive);
        p.drawText(start, windowTitleText);
        p.restore();
    }

    // Default pen
    QPen pen(active ? fg : fg_inactive);
    p.setPen(pen);

    // Close button
    p.save();
    // close button
    auto icopath = do_dark ? ":/Icons/Dark/window-close"
                           : ":/Icons/Light/window-close";
    int close_left = surfaceSize().width()-bs-btnsz;
    QIcon wcico = QIcon(icopath);
    if (!wcico.isNull())
    {
        QRect iconRect(close_left, wg.top()+4, btnsz, btnsz);
        wcico.paint(&p, iconRect);
    }
    p.restore();

    // Maximize button
    // maximize/restore
    int max_left = close_left - btnsp - btnsz;
    if(m_showMinMaxBtn)
    {
        p.save();
        if(isMaximized())
        {
            auto icopath = do_dark ? ":/Icons/Dark/window-restore"
                                   : ":/Icons/Light/window-restore";
            QIcon ico = QIcon(icopath);
            if (!ico.isNull())
            {
                QRect iconRect(max_left, wg.top()+4, btnsz, btnsz);
                ico.paint(&p, iconRect);
            }
        }
        else
        {
            auto icopath = do_dark ? ":/Icons/Dark/window-maximize"
                                   : ":/Icons/Light/window-maximize";
            QIcon ico = QIcon(icopath);
            if (!ico.isNull())
            {
                QRect iconRect(max_left, wg.top()+4, btnsz, btnsz);
                ico.paint(&p, iconRect);
            }
        }
        p.restore();

        p.save();
        int min_left = max_left - btnsp - btnsz;
        if(canMinimize())
        {
            auto icopath = do_dark ? ":/Icons/Dark/window-minimize"
                                   : ":/Icons/Light/window-minimize";
            QIcon ico = QIcon(icopath);
            if (!ico.isNull())
            {
                QRect iconRect(min_left, wg.top()+4, btnsz, btnsz);
                ico.paint(&p, iconRect);
            }
        }
        p.restore();
    }
    hwComp->triggerRender();
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
        float xPos = popup->anchorRect().x();
        int height = popup->positionerSize().height();
        height = height - height - height;
        setPosition(QPointF(xPos,height));
        return;
    }

    return;
}

void Surface::onSurfaceSourceGeometryChanged()
{
    qDebug() << "Surface::onSurfaceSourceGeometryChanged" << surface()->sourceGeometry();
    //m_viewport = surface()->sourceGeometry();
}

void Surface::onDestinationSizeChanged()
{
    renderDecoration();
    hwComp->triggerRender();
}

void Surface::onBufferScaleChanged()
{
    renderDecoration();
    hwComp->triggerRender();
}

void Surface::onXdgStartResize(QWaylandSeat *seat, Qt::Edges edges)
{
    hwComp->onXdgStartResize(seat, edges);
    renderDecoration();
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
    if(m_ssd)
        renderDecoration();
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
    {
        m_ssd = true;
        renderDecoration();
    }
    else
        m_ssd = false;
    hwComp->triggerRender();
}

void Surface::onQtWindowTitleChanged(const QString &title)
{
    m_windowTitle = title;
    if(m_wndctl)
        m_wndctl->setTitle(m_windowTitle);
    if(m_ssd)
        renderDecoration();
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
    if(m_ssd)
        renderDecoration();
    hwComp->triggerRender();
}

void Surface::onQtShellSetSize(const QSize &size)
{
    m_qt_size = size;
    if(m_ssd)
        renderDecoration();
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
        renderDecoration();
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        m_qt->send_set_frame_margins(bs,bs,ds,bs);
        m_surfaceType = TopLevel;
    }

    if(f.testFlag(Qt::Dialog) && !f.testFlag(Qt::Popup))
    {
        m_ssd = true;
        renderDecoration();
        auto bs = hwComp->borderSize();
        auto ds = hwComp->decorationSize();
        m_qt->send_set_frame_margins(bs,bs,ds,bs);
        m_surfaceType = TopLevel;
    }

    if(f.testFlag(Qt::Tool))
    {
        m_ssd = true;
        renderDecoration();
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
        hwComp->onDesktopRequest(this->surface());
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

QImage* Surface::decorationImage()
{
    return m_ssdimg;
}

void Surface::activate()
{
    if(m_xdgTopLevel)
    {
        QList<HWWaylandXdgToplevel::State> states = m_xdgTopLevel->states();
        states.append(HWWaylandXdgToplevel::ActivatedState);
        m_xdgTopLevel->sendConfigure(surfaceSize(), states);
    }
}

void Surface::deactivate()
{
    if(m_xdgTopLevel)
    {
        QList<HWWaylandXdgToplevel::State> states = m_xdgTopLevel->states();
        states.removeOne(HWWaylandXdgToplevel::ActivatedState);
        m_xdgTopLevel->sendConfigure(surfaceSize(), states);
    }
}

void Surface::toggleMinimize()
{
    if(m_minimized)
        unsetMinimized();
    else
        setMinimized();
}

void Surface::toggleMaximize()
{

}

void Surface::toggleActive()
{
    hwComp->raise(this);
}

void Surface::setAnimatedSurfaceSize(QSize size)
{
    m_resize_animation_size = size;
}

void Surface::createWlShellSurface(QWaylandWlShellSurface *surface)
{
    m_wlShellSurface = surface;
    m_surfaceType = TopLevel;
    // TODO: fix this?
    //m_wndctl = m_compositor->m_wndmgr->createWindowControl(this);
    connect(surface, &QWaylandWlShellSurface::startMove, hwComp, &Compositor::onStartMove);
    connect(surface, &QWaylandWlShellSurface::startResize, hwComp, &Compositor::onWlStartResize);
    connect(surface, &QWaylandWlShellSurface::setTransient, hwComp, &Compositor::onSetTransient);
    connect(surface, &QWaylandWlShellSurface::setPopup, hwComp, &Compositor::onSetPopup);
    hwComp->triggerRender();
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

    createPlasmaWindowControl();
    m_wndctl->setTitle(topLevel->title());
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
        yPos = rect.top();
    else if(popup->anchorEdges() & Qt::BottomEdge)
        yPos = rect.bottom();
    else
        yPos = rect.top() + rect.height() / 2;

    float xPos = 0;
    if(popup->anchorEdges() & Qt::LeftEdge)
        xPos = rect.left();
    else if(popup->anchorEdges() & Qt::RightEdge)
        xPos = rect.right();
    else
        xPos = rect.left() + rect.width() / 2;

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
}

void Surface::createMenuServer(OriginullMenuServer *menu)
{
    m_menuServer = menu;
}

void Surface::surfaceDestroyed()
{
    hwComp->recycleSurfaceObject(this);
    if(m_wndctl)
        m_wndctl->destroy();
    hwComp->triggerRender();
}

void Surface::viewSurfaceDestroyed()
{
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
    m_resize_animation_start_size = surfaceSize();
    auto size = primaryView()->output()->availableGeometry();
    if(this->serverDecorated())
    {
        size.setHeight(size.height() - hwComp->decorationSize() - hwComp->borderSize());
        size.setWidth(size.width() - hwComp->borderSize()*2);
    }
    m_maximized_complete = false;
    m_xdgTopLevel->sendMaximized(size.size());
    if(m_wndctl)
        m_wndctl->setMaximized(true);
}

void Surface::onXdgUnsetMaximized()
{
    m_maximized = false;
    m_resize_animation_start_point = surfacePosition();
    m_xdgTopLevel->sendUnmaximized();
    m_resize_animation_start_size = surfaceSize();
    if(m_wndctl)
        m_wndctl->setMaximized(true);
}

void Surface::onXdgSetMinimized()
{
    m_minimized = true;
    hwComp->triggerRender();
    if(m_wndctl)
        m_wndctl->setMinimized(true);
}

void Surface::completeXdgConfigure()
{
    renderDecoration();
    if(m_maximized)
    {
        auto pos = primaryView()->output()->position();
        pos.setX(pos.x()+hwComp->borderSize());
        pos.setY(pos.y()+hwComp->decorationSize());
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
                sizeAnimation->setEndValue(surfaceSize());
                sizeAnimation->setDuration(190);
                sizeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                connect(posAnimation, &QPropertyAnimation::valueChanged, [this](){
                    renderDecoration();
                    hwComp->triggerRender();
                });
                m_resize_animation = true;
                group->addAnimation(posAnimation);
                group->addAnimation(sizeAnimation);
                group->start(QPropertyAnimation::DeleteWhenStopped);
                connect(group, &QParallelAnimationGroup::finished, [this]() {
                    m_maximized_complete = true;
                    m_resize_animation = false;
                    renderDecoration();
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
                sizeAnimation->setEndValue(surfaceSize());
                sizeAnimation->setDuration(190);
                sizeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                connect(posAnimation, &QPropertyAnimation::valueChanged, [this](){
                    renderDecoration();
                    hwComp->triggerRender();
                });
                m_resize_animation = true;
                group->addAnimation(posAnimation);
                group->addAnimation(sizeAnimation);
                group->start(QPropertyAnimation::DeleteWhenStopped);
                connect(group, &QParallelAnimationGroup::finished, [this]() {
                    m_resize_animation = false;
                    renderDecoration();
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
    renderDecoration();
}

void Surface::onXdgParentTopLevelChanged()
{
    m_parentTopLevelSurface =
            hwComp->findSurfaceObject(m_xdgTopLevel->parentToplevel()->xdgSurface()->surface());
    m_parentTopLevelSurface->addXdgChildSurfaceObject(this);
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
        }
    }
}

void Surface::onXdgWindowGeometryChanged()
{
    renderDecoration();
    // if we are initializing an xdg toplevel we should set a
    // better initial position
    if(!m_surfaceInit)
    {
        // TODO: reposition
        qDebug() << surfaceSize();
        m_surfaceInit = true;
    }
}

// Layer Shell Functions

void Surface::createLayerShellSurface(WlrLayerSurfaceV1 *surface)
{
    m_layerSurface = surface;
    connect(m_layerSurface, &WlrLayerSurfaceV1::anchorsChanged, this, &Surface::onAnchorsChanged);
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
    recalculateAnchorPosition();
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
    recalculateAnchorPosition();
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

void Surface::recalculateAnchorPosition()
{
    if(!m_ls_size.isValid())
        return;

    auto anchors = m_layerSurface->anchors();
    if(anchors & WlrLayerSurfaceV1::TopAnchor || anchors & WlrLayerSurfaceV1::BottomAnchor)
    {
        if(m_ls_size.width() == 0)
        {
            auto view = primaryView();
            m_ls_size.setWidth(view->output()->geometry().width());
        }
    }

    if(anchors & WlrLayerSurfaceV1::LeftAnchor || anchors & WlrLayerSurfaceV1::RightAnchor)
    {
        if(m_ls_size.height() == 0)
            m_ls_size.setHeight(primaryView()->output()->geometry().height());
    }

    if(anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::LeftAnchor)
    {
        m_surfacePosition = primaryView()->output()->position();
        return;
    }

    if(anchors == WlrLayerSurfaceV1::TopAnchor)
    {
        QPoint pos = primaryView()->output()->position();
        if(m_ls_size.width() < primaryView()->output()->geometry().width())
        {
            uint viewdiff = primaryView()->output()->geometry().width() - m_ls_size.width();
            pos.setX(pos.x() + viewdiff/2);
        }
        m_surfacePosition = pos;
        return;
    }

    if(anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::RightAnchor)
    {
        m_surfacePosition = primaryView()->output()->position();
        return;
    }

    if(anchors == WlrLayerSurfaceV1::BottomAnchor)
    {
        QPoint btmpos = primaryView()->output()->position();
        if(m_ls_size.width() < primaryView()->output()->geometry().width())
        {
            uint viewdiff = primaryView()->output()->geometry().width() - m_ls_size.width();
            btmpos.setY(btmpos.y() + viewdiff/2);
        }
        btmpos.setY(primaryView()->output()->geometry().height() - m_ls_size.height());
        m_surfacePosition = btmpos;
        return;
    }

    if(anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::LeftAnchor)
    {
        QPoint btmpos = primaryView()->output()->position();
        btmpos.setX(primaryView()->output()->geometry().height() - m_ls_size.height());
        m_surfacePosition = btmpos;
        return;
    }
}

