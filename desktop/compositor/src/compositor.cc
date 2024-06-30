// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "compositor.h"

#include "view.h"
#include "surfaceobject.h"
#include "output.h"
#include "outputwnd.h"
#include "wallpaper.h"

// Wayland Extensions
#include "appmenu.h"
#include "originull.h"
#include "wndmgmt.h"
#include "layershell.h"
#include "gtkshell.h"
#include "qtshell.h"
#include "fullscreen.h"
#include "activation.h"
#include "screencopy.h"

#include "surfaceobject.h"
#include "shortcuts.h"

#include <QSettings>
#include <QPainter>
#include <QTextOption>
#include <QFile>
#include <hollywood/hollywood.h>
#include <sys/utsname.h>

QLoggingCategory lc("compositor");

Compositor::Compositor(bool sddm)
    : m_desktop_bg(QColor(HOLLYWOOD_DEF_DESKTOP_BG))
    , m_wlShell(new QWaylandWlShell(this))
    , m_xdgShell(new HWWaylandXdgShell(this))
    , m_xdgDecoration(new QWaylandXdgDecorationManagerV1())
    , m_hwPrivate(new OriginullProtocol(this))
    , m_layerShell(new WlrLayerShellV1(this))
    , m_wndmgr(new PlasmaWindowManagement(this))
    , m_appMenu(new AppMenuManager(this))
    , m_gtk(new GtkShell(this))
    , m_qt(new QtShell(this))
    , m_fs(new FullscreenShell(this))
    , m_activation(new XdgActivation(this))
    , m_outputmgr(new QWaylandXdgOutputManagerV1(this))
    , m_screencopy(new WlrScreencopyManagerV1(this))
    , m_sddm(sddm)
    , m_shortcuts(new ShortcutManager(this))
    , m_timeout(new QTimer(this))
{
    m_timeout->setTimerType(Qt::VeryCoarseTimer);
    loadSettings();
    m_cfgwatch = new QFileSystemWatcher();
    m_cfgwatch->addPath(m_configfile);
    connect(m_cfgwatch, &QFileSystemWatcher::fileChanged,
            this, &Compositor::configChanged);

    generateDesktopInfoLabelImage();
    connect(m_timeout, &QTimer::timeout, this, &Compositor::idleTimeout);
}

Compositor::~Compositor() {}

void Compositor::create()
{
    QWaylandCompositor::create();
    connect(this, &QWaylandCompositor::surfaceCreated, this, &Compositor::onSurfaceCreated);
    connect(this, &QWaylandCompositor::subsurfaceChanged, this, &Compositor::onSubsurfaceChanged);
    // TODO: cursors are leaky & buggy
    //connect(defaultSeat(), &QWaylandSeat::cursorSurfaceRequest, this, &Compositor::adjustCursorSurface);
    connect(defaultSeat()->drag(), &QWaylandDrag::dragStarted, this, &Compositor::startDrag);

    m_hwPrivate->setExtensionContainer(this);
    m_hwPrivate->initialize();
    m_appMenu->initialize();
    m_wndmgr->setExtensionContainer(this);
    m_wndmgr->initialize();
    m_xdgDecoration->setExtensionContainer(this);
    m_xdgDecoration->initialize();
    m_xdgDecoration->setPreferredMode(QWaylandXdgToplevel::ServerSideDecoration);
    m_outputmgr->setExtensionContainer(this);
    m_outputmgr->initialize();
    m_screencopy->setExtensionContainer(this);
    m_screencopy->initialize();
    connect(m_appMenu, &AppMenuManager::appMenuCreated, this, &Compositor::appMenuCreated);
    connect(m_hwPrivate, &OriginullProtocol::menuServerSet, this, &Compositor::onMenuServerRequest);
    connect(m_hwPrivate, &OriginullProtocol::wallpaperRotationRequested, this, &Compositor::onRotateWallpaper);
    connect(m_hwPrivate, &OriginullProtocol::desktopSurfaceMarked, this, &Compositor::onDesktopRequest);
    connect(m_wlShell, &QWaylandWlShell::wlShellSurfaceCreated, this, &Compositor::onWlShellSurfaceCreated);
    connect(m_xdgShell, &HWWaylandXdgShell::xdgSurfaceCreated, this, &Compositor::onXdgSurfaceCreated);
    connect(m_xdgShell, &HWWaylandXdgShell::toplevelCreated, this, &Compositor::onXdgTopLevelCreated);
    connect(m_xdgShell, &HWWaylandXdgShell::popupCreated, this, &Compositor::onXdgPopupCreated);
    connect(m_layerShell, &WlrLayerShellV1::layerSurfaceCreated, this, &Compositor::onLayerSurfaceCreated);
    connect(m_gtk, &GtkShell::surfaceCreated, this, &Compositor::onGtkSurfaceCreated);
    connect(m_qt, &QtShell::surfaceCreated, this, &Compositor::onQtSurfaceCreated);
    connect(m_fs, &FullscreenShell::surfacePresented, this, &Compositor::onFullscreenSurfaceRequested);

    connect(m_activation, &XdgActivation::surfaceActivated, this, &Compositor::onXdgSurfaceActivated);
}

bool Compositor::hasMenuServer()
{
    return m_menuServer ? true : false;
}

uint Compositor::nextId()
{
    m_id++;
    return m_id;
}

uint Compositor::decorationSize() const { return m_decorationSize; }

Output *Compositor::outputAtPosition(const QPoint &pos)
{
    for(auto out : m_outputs)
    {
        if(out->wlOutput()->geometry().contains(pos))
            return out;
    }

    return nullptr;
}

Output *Compositor::outputFor(QWaylandOutput *wloutput)
{
    for(auto out : m_outputs)
    {
        if(out->wlOutput() == wloutput)
            return out;
    }

    return nullptr;
}

void Compositor::resetLayerShellLayer(Surface *s)
{
    if(!s->layerSurface())
        return;

    m_zorder.removeOne(s);
    m_layer_bg.removeOne(s);
    m_layer_bottom.removeOne(s);
    m_layer_top.removeOne(s);
    m_layer_overlay.removeOne(s);

    switch(s->layerSurface()->layer())
    {
    case WlrLayerShellV1::BackgroundLayer:
        m_layer_bg.prepend(s);
        break;
    case WlrLayerShellV1::BottomLayer:
        m_layer_bottom.prepend(s);
        break;
    case WlrLayerShellV1::TopLayer:
        m_layer_top.prepend(s);
        break;
    case WlrLayerShellV1::OverlayLayer:
        m_layer_overlay.prepend(s);
        break;

    }
}

QString Compositor::surfaceZOrderByUUID() const
{
    QStringList uuids;
    for(auto *obj : m_zorder)
    {
        if(obj->isSpecialShellObject())
            continue;
        if(obj->plasmaControl())
            uuids.append(obj->plasmaControl()->uuid().toString(QUuid::WithoutBraces));
    }

    return uuids.join(" ");
}

void Compositor::generateDesktopInfoLabelImage()
{
    QStringList lines;
    auto kernel = QString();
    struct utsname unb;
    if(uname(&unb) != 0)
        kernel = QLatin1String("Unknown");
    else
        kernel = QLatin1String(unb.release);

    lines << QString("Hollywood %1 (API %2, Kernel %3)")
        .arg(QLatin1String("Edge"))
        .arg(QLatin1String(QT_VERSION_STR))
        .arg(kernel);

    QFont font(HOLLYWOOD_DEF_STDFONT, 9);
    QFontMetrics m = QFontMetrics(font);
    int textHeight = m.height();
    QTextOption to;

    auto env = qgetenv("HOLLYWOOD_GENUINE");
    if(!env.isEmpty())
        lines << QLatin1String("This copy of Linux is not genuine.");
    to.setAlignment(Qt::AlignRight);

    int max_height = lines.count()*textHeight;
    int max_width = 0;
    for(int i = 0; i < lines.count(); i++)
    {
        auto text = lines.at(i);
        auto rect = m.boundingRect(text, to);
        if(rect.width() > max_width)
            max_width = rect.width();
    }
    max_width = max_width+5;

    QRect bounds(QPoint(0,0),QSize(max_width,max_height));
    m_dtlabel = new QImage(bounds.size(),
                          QImage::Format_RGBA8888);
    QPainter px(m_dtlabel);
    px.setCompositionMode(QPainter::CompositionMode_Clear);
    px.fillRect(QRect(QPoint(0,0), bounds.size()), QColor(255,255,255,0));

    px.setCompositionMode(QPainter::CompositionMode_SourceOver);
    px.setRenderHint(QPainter::TextAntialiasing);
    px.setPen(Qt::white);
    px.setFont(font);

    for(int i = 0; i < lines.count(); i++)
    {
        auto text = lines.at(i);
        auto rect = m.boundingRect(text, to);
        int y = bounds.size().height() - (textHeight * lines.count()) +
                (textHeight * (i+1)) - 5;
        int x = bounds.size().width() - rect.width() - 5;
        px.drawText(QPoint(x,y), text);
    }
    px.end();
}

QImage *Compositor::desktopLabelImage()
{
    if(m_dtlabel == nullptr)
        generateDesktopInfoLabelImage();
    return m_dtlabel;
}

bool Compositor::processHasTwilightMode(quint64 pid) const
{
    QFile file(QString("/proc/%1/environ").arg(QString::number(pid)));
    if(file.exists())
    {
        if(file.open(QFile::ReadOnly))
        {
            auto data = file.readAll();
            file.close();
            if(data.contains("HW_TWILIGHT_SHELL"))
                return true;
        }
    }

    return false;
}

QPointF Compositor::correctedPosition(const QPointF &point)
{
    QPointF returnPoint = point;
    for(auto *obj : m_layer_top)
    {
        // TODO: reserved right/left
        if(obj->layerSurface()->anchors() & WlrLayerSurfaceV1::anchor_top
                && obj->layerSurface()->exclusiveZone() > 0)
        {
            QPoint screenTop = obj->primaryView()->output()->position();
            int reserved = screenTop.y()+obj->layerSurface()->exclusiveZone();
            // see if our point y conflicts
            if(point.y() >= screenTop.y() && point.y() <= reserved)
                returnPoint.setY(reserved+1);
        }
    }

    return returnPoint;
}

ShortcutManager *Compositor::shortcuts()
{
    return m_shortcuts;
}

QList<SurfaceView*> Compositor::views() const
{
    QList<SurfaceView*> returnList;
    for(auto *obj : m_zorder)
    {
        if(!obj->primaryView())
            returnList.append(obj->primaryView());
    }

    return returnList;
}

void Compositor::onSurfaceCreated(QWaylandSurface *surface)
{
    auto *obj = new Surface(surface);
    /* connect(surface, &QWaylandSurface::surfaceDestroyed, this, [this,obj]() {
        recycleSurfaceObject(obj);
    });*/
    bool twilight = processHasTwilightMode(surface->client()->processId());
    obj->setTwilight(twilight);
    auto outputAt = outputAtPosition(obj->surfacePosition().toPoint());
    if(outputAt == nullptr)
        outputAt = m_outputs.first();

    obj->createViewForOutput(outputAt);
    qDebug(lc) << QString("Compositor::onSurfaceCreated %1").arg(obj->uuid().toString(QUuid::WithoutBraces));
    m_surfaces << (obj);
    m_zorder << (obj);
}

void Compositor::recycleSurfaceObject(Surface *obj)
{
    qDebug(lc) << QString("Compositor::recycleSurfaceObject %1").arg(obj->uuid().toString(QUuid::WithoutBraces));

    if(obj->xdgTopLevelParent())
        obj->xdgTopLevelParent()->removeXdgTopLevelChild(obj);

    m_surfaces.removeOne(obj);
    m_zorder.removeOne(obj);
    m_desktops.removeOne(obj);
    m_layer_bg.removeOne(obj);
    m_layer_bottom.removeOne(obj);
    m_layer_top.removeOne(obj);
    m_layer_overlay.removeOne(obj);

    // TODO: check if we just destroyed an xdg toplevel modal dialog
    // and raise our parent
    if(m_tl_raised == obj)
        raiseNextInLine();

    delete obj;
    obj = nullptr;
}

void Compositor::surfaceHasContentChanged()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface->hasContent()) {
        // TODO: if we support more compositor protocols we need to add roles here
        if (surface->role() == QWaylandWlShellSurface::role()
                || surface->role() == HWWaylandXdgToplevel::role()
                || surface->role() == QtSurface::role()) {
            defaultSeat()->setKeyboardFocus(surface);
            defaultSeat()->setMouseFocus(surface->primaryView());
        }
    }
    // TODO: avoid this if surface is minimized?

    triggerRender();
}

SurfaceView * Compositor::findView(const QWaylandSurface *s) const
{
    for (auto* obj : m_surfaces) {
        if (obj->surface() == s)
            return obj->primaryView();
    }
    return nullptr;
}

Surface* Compositor::findSurfaceObject(const QWaylandSurface *s) const
{
    for (auto* obj : m_surfaces) {
        if (obj->surface() == s)
            return obj;
    }
    return nullptr;
}

void Compositor::onMenuServerRequest(OriginullMenuServer *menu)
{
    m_menuServer = menu;
    connect(m_menuServer, &OriginullMenuServer::menuServerDestroyed,
               this, &Compositor::menuServerDestroyed);
}

void Compositor::onDesktopRequest(QWaylandSurface *surface)
{
    auto *sf = findSurfaceObject(surface);
    Q_ASSERT(sf);
    qDebug(lc) << "Compositor::onDesktopRequest" << sf->uuid().toString();

    if(m_zorder.contains(sf))
        m_zorder.removeOne(sf);

    delete sf->m_wndctl;
    m_desktops.append(sf);
    sf->m_surfaceType = Surface::Desktop;
    sf->m_isShellDesktop = true;
    sf->m_ssd = false;
    auto start = sf->primaryView()->output()->availableGeometry().topLeft();
    sf->m_surfacePosition = start;
    sf->setPosition(start);
}


void Compositor::loadSettings()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    m_configfile = settings.fileName();

    settings.beginGroup(QLatin1String("General"));
    auto accent = settings.value(QLatin1String("AccentColor")).toString();
    QColor ac = QColor(accent);
    if(!ac.isValid())
        ac = QColor(QLatin1String(HOLLYWOOD_ACCENT_BLUE));
    m_accent = ac;

    uint old_app = m_apperance;
    uint _app = settings.value(QLatin1String("ApperanceMode")).toUInt();
    if(_app > 2)
        _app = 0;   // default to light mode on invalid settings
    m_apperance = _app;

    settings.endGroup();

    settings.beginGroup("PrimaryWallpaper");
    auto bgcol = settings.value(QLatin1String("BackgroundColor")).toString();
    QColor bg = QColor(bgcol);
    if(!bg.isValid())
        bg = QColor(QLatin1String(HOLLYWOOD_DEF_DESKTOP_BG));
    m_desktop_bg = bg;
    settings.endGroup();

    settings.beginGroup(QLatin1String("Metrics"));
    auto tbh = settings.value("TitleBarHeight", 30).toUInt();
    if(tbh < 4)
        tbh = 4;

    if(tbh > 80)
        tbh = 80;

    m_decorationSize = tbh;

    // borders suck
    auto border = settings.value("BorderSize", 0).toUInt();
    if(border < 1)
        border = 1;

    if(border > 10)
        border = 10;

    m_borderSize = border;
    settings.endGroup();
    settings.beginGroup(QLatin1String("Compositor"));

    m_legacyExperience = settings.value("LegacyExperience", false).toBool();

    settings.endGroup();
    settings.beginGroup(QLatin1String("Energy"));

    // TODO: put these defaults into hollywood.h
    m_timeout_display = settings.value("DisplayTimeout", 60).toUInt();
    m_timeout_sleep = settings.value("SleepTimeout", 300).toUInt();

    // monitors should not sleep before the system does
    if(m_timeout_display >= m_timeout_sleep)
        m_timeout_display = 0;

    settings.endGroup();
    if(m_apperance != old_app)
    {
        // dark/light mode changed - redraw SSD's
        for(auto s : m_surfaces)
            s->renderDecoration();
    }

    if(m_timeout->isActive())
        m_timeout->stop();
    setupIdleTimer();
    emit settingsChanged();
}

void Compositor::setupIdleTimer()
{
    // setup the timer
    if(!m_display_sleeping && m_timeout_display > 0)
        m_timeout->start(m_timeout_display*1000);
    else if(m_display_sleeping || (m_timeout_display == 0 && m_timeout_sleep > 0))
        m_timeout->start(m_timeout_sleep*1000);
}

void Compositor::idleTimeout()
{
    // idle inhibited by client, don't do anything
    if(m_idle_inhibit)
        return;

    // timeout to handle a display sleep
    if(!m_display_sleeping && m_timeout_display > 0)
    {
        // display not sleeping, display timeout > 0 so this
        // will be a display sleep command

        if(m_lock_after_sleep == 0)
            lockSession();
        else
            QTimer::singleShot(m_lock_after_sleep, this, &Compositor::lockSession);

        m_display_sleeping = false;
        if(m_timeout_sleep > 0)
            setupIdleTimer();
    }

    // timeout to handle a system sleep
    if(m_display_sleeping ||
       (m_timeout_display == 0 &&
        m_timeout_sleep > 0))
    {
        // we don't have display sleep but we do have system sleep
        lockSession();
        // TODO: tell session to put system to sleep
    }
}

void Compositor::raiseNextInLine()
{
    qDebug(lc) << "Compositor::raiseNextInLine";
    m_tl_raised = nullptr;
    m_activated = nullptr;

    int sc = m_zorder.count();
    bool raised = false;
    if(m_zorder.count() > 0)
    {
        for(auto i = m_zorder.count()-1; i >= 0; --i)
        {
            if(m_zorder.count() != sc)
                return raiseNextInLine();

            if(m_zorder.at(i))
            {
                Surface* obj = m_zorder.at(i);
                if(obj && obj->isInitialized())
                {
                    if(obj->xdgTopLevel() != nullptr ||
                        obj->qtSurface() != nullptr ||
                        obj->gtkSurface() != nullptr)
                    {
                        if(obj->isInitialized() && !obj->isMinimized())
                        {
                            raise(obj);
                            raised = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if(raised)
        return;

    if(!raised && m_desktops.count() > 0)
        activate(m_desktops.last());
}

void Compositor::onWlShellSurfaceCreated(QWaylandWlShellSurface *wlShellSurface)
{
    auto *surface = findSurfaceObject(wlShellSurface->surface());
    Q_ASSERT(surface);
    surface->createWlShellSurface(wlShellSurface);
}

void Compositor::onXdgSurfaceCreated(HWWaylandXdgSurface *xdgSurface)
{
    auto *surface = findSurfaceObject(xdgSurface->surface());
    Q_ASSERT(surface);
    qDebug(lc) << QString("Compositor::onXdgSurfaceCreated %1").arg(surface->uuid().toString(QUuid::WithoutBraces));
    surface->createXdgShellSurface(xdgSurface);
}

void Compositor::onXdgPopupCreated(HWWaylandXdgPopup *popup, HWWaylandXdgSurface *surface)
{
    Surface *mySurface = findSurfaceObject(surface->surface());
    Q_ASSERT(mySurface);
    m_zorder.removeOne(mySurface);
    qDebug(lc) << QString("Compositor::onXdgPopupCreated %1").arg(mySurface->uuid().toString(QUuid::WithoutBraces));
    mySurface->createXdgPopupSurface(popup);
}

void Compositor::onLayerSurfaceCreated(WlrLayerSurfaceV1 *layerSurface)
{
    auto *surface = findSurfaceObject(layerSurface->surface());
    Q_ASSERT(surface);
    qDebug(lc) << QString("Compositor::onLayerSurfaceCreated %1").arg(surface->uuid().toString(QUuid::WithoutBraces));
    surface->createLayerShellSurface(layerSurface);
}

void Compositor::onGtkSurfaceCreated(GtkSurface *gtkSurface)
{
    Surface *surface = findSurfaceObject(gtkSurface->surface());
    Q_ASSERT(surface);
    qDebug(lc) << QString("Compositor::onGtkSurfaceCreated %1").arg(surface->uuid().toString(QUuid::WithoutBraces));
    surface->createGtkSurface(gtkSurface);
}

void Compositor::onQtSurfaceCreated(QtSurface *qtSurface)
{
    Surface *surface = findSurfaceObject(qtSurface->surface());
    Q_ASSERT(surface);
    qDebug(lc) << QString("Compositor::onQtSurfaceCreated %1").arg(surface->uuid().toString(QUuid::WithoutBraces));
    surface->createQtSurface(qtSurface);
}

void Compositor::onFullscreenSurfaceRequested(QWaylandSurface *surface)
{
    m_hasFullscreenSurface = true;
    auto ms = findSurfaceObject(surface);
    m_fullscreenSurface = ms;
    ms->m_fullscreenShell = true;
    // TODO: multi-output
    ms->setPosition(QPoint(0,0));

    defaultSeat()->setKeyboardFocus(surface);
    // TODO: handle destruction
}

void Compositor::onXdgTopLevelCreated(HWWaylandXdgToplevel *topLevel, HWWaylandXdgSurface *surface)
{
    auto *mySurface = findSurfaceObject(surface->surface());
    Q_ASSERT(mySurface);
    mySurface->createXdgTopLevelSurface(topLevel);
    qDebug(lc) << QString("Compositor::onXdgTopLevelCreated %1").arg(mySurface->uuid().toString(QUuid::WithoutBraces));
    // this is temporary and will be better calculated in onXdgWindowGeometryChanged
    mySurface->setPosition(QPointF(1,1));

    m_tl_raised = mySurface;
}

void Compositor::onStartMove(QWaylandSeat *seat)
{
    Q_UNUSED(seat);
    m_wlShell->closeAllPopups();
    emit startMove();
}

void Compositor::onWlStartResize(QWaylandSeat *, QWaylandWlShellSurface::ResizeEdge edges)
{
    m_wlShell->closeAllPopups();
    emit startResize(int(edges), false);
}

void Compositor::onXdgStartResize(QWaylandSeat *seat,
                                  Qt::Edges edges)
{
    Q_UNUSED(seat);
    emit startResize(int(edges), false);
}

void Compositor::onSetTransient(QWaylandSurface *parent, const QPoint &relativeToParent, bool inactive)
{
    Q_UNUSED(relativeToParent);
    Q_UNUSED(inactive);

    //QWaylandWlShellSurface *wlShellSurface = qobject_cast<QWaylandWlShellSurface*>(sender());
    //ACView *view = findView(wlShellSurface->surface());

    if (parent) {
        raise(findSurfaceObject(parent));
        /* ACView *parentView = findView(parent);
        if (parentView)
            view->setPosition(parentView->position() + relativeToParent); */
    }
}

void Compositor::onSetPopup(QWaylandSeat *seat, QWaylandSurface *parent, const QPoint &relativeToParent)
{
    Q_UNUSED(relativeToParent);
    Q_UNUSED(seat);
    QWaylandWlShellSurface *surface = qobject_cast<QWaylandWlShellSurface*>(sender());
    SurfaceView *view = findView(surface->surface());
    if (view)
        raise(findSurfaceObject(parent));
}

void Compositor::onSubsurfaceChanged(QWaylandSurface *child, QWaylandSurface *parent)
{
    Surface *obj = findSurfaceObject(child);
    Surface *objParent = findSurfaceObject(parent);
    obj->setParentSurfaceObject(objParent);
    obj->setSubsurface(true);
    obj->m_surfaceInit = true;
    m_zorder.removeOne(obj);
}

void Compositor::onSubsurfacePositionChanged(const QPoint &position)
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender());
    if (!surface)
        return;
    findSurfaceObject(surface)->setPosition(position);
    triggerRender();
}

void Compositor::onXdgSurfaceActivated(QWaylandSurface *surface)
{
    if(surface->isDestroyed())
        return;

    auto sf = findSurfaceObject(surface);
    qDebug(lc) << QString("Compositor::onXdgSurfaceActivated %1").arg(sf->uuid().toString(QUuid::WithoutBraces));
    if(sf)
    {
        if(m_activated != sf && m_tl_raised != sf)
            raise(sf);
    }
}

void Compositor::triggerRender()
{
    for(auto out : m_outputs)
    {
        out->window()->requestUpdate();
    }
}

void Compositor::lockSession()
{

}

void Compositor::startRender()
{
    for(auto out : m_outputs)
    {
        if(out)
            out->wlOutput()->frameStarted();
    }
}

void Compositor::endRender()
{
    for(auto out : m_outputs)
    {
        if(out)
            out->wlOutput()->sendFrameCallbacks();
    }
}

QColor Compositor::accentColor() const
{
    return m_accent;
}

QColor Compositor::primaryBackgroundColor() const
{
    return m_desktop_bg;
}

void Compositor::updateCursor()
{
    /*if(!m_cursorObject)
        qDebug() << "Compositor::updateCursor null m_cursorObject";*/
    if(m_cursorObject->primaryView())
        m_cursorObject->primaryView()->advance();
    /*else
        qDebug() << "Compositor::updateCursor null m_cursorObject primaryView";*/
    QImage image = m_cursorObject->primaryView()->currentBuffer().image();
    if (!image.isNull())
        primaryOutput()->window()->setCursor(QCursor(QPixmap::fromImage(image), m_cursorHotspotX, m_cursorHotspotY));
}

void Compositor::onRotateWallpaper()
{
    // TODO: multiple screens
    primaryOutput()->window()->wallpaperManager()->rotateNow();
}

void Compositor::appMenuCreated(AppMenu *m)
{
    m->surface()->setAppMenu(m);
}

void Compositor::menuServerDestroyed()
{
    disconnect(m_menuServer, &OriginullMenuServer::menuServerDestroyed,
               this, &Compositor::menuServerDestroyed);
    delete m_menuServer;
    m_menuServer = nullptr;
}


void Compositor::configChanged()
{
    bool file_deleted = !m_cfgwatch->files().contains(m_configfile);
    if(file_deleted)
        m_cfgwatch->addPath(m_configfile);

    loadSettings();
}

void Compositor::adjustCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY)
{
    if(m_cursorObject)
    {
        if(m_cursorObject->surface() != surface)
        {
            //qDebug() << "Compositor::adjustCursorSurface: changing surface";
            if (m_cursorObject->surface())
            {
                try
                {
                    //qDebug() << "Compositor::adjustCursorSurface: disconnecting updateCursor";
                    disconnect(this, SLOT(updateCursor()));
                }
                catch(int e)
                {
                    qCritical() << "Compositor::adjustCursorSurface: couldn't disconnect QWaylandSurface::redraw";
                }
            }
            if(m_surfaces.contains(m_cursorObject))
            {
            //qDebug() << "Compositor::adjustCursorSurface: removing old cursor from surface list";
            m_surfaces.removeOne(m_cursorObject);
            }

            if(m_cursorObject)
            {
                //TODO: Fix me i leak
//                m_cursorObject->deleteLater();
                m_cursorObject = nullptr;
                //qDebug() << "Compositor::adjustCursorSurface: m_cursorObject == nullptr";
            }
        }
    }
    if(!m_cursorObject)
    {
        m_cursorObject = new Surface(surface);
        auto outputAt = outputAtPosition(m_cursorObject->surfacePosition().toPoint());
        if(outputAt == nullptr)
            outputAt = m_outputs.first();

        m_cursorObject->createViewForOutput(outputAt);
        m_surfaces.append(m_cursorObject);
        if(surface)
        {
            m_cursorObject->surface()->markAsCursorSurface(true);
            connect(surface, &QWaylandSurface::redraw, this, &Compositor::updateCursor);
        }
    }

    m_cursorHotspotX = hotspotX;
    m_cursorHotspotY = hotspotY;

    if (surface && surface->hasContent())
    {
        //qDebug() << "Compositor::adjustCursorSurface: updating new cursor";
        updateCursor();
    }
}

void Compositor::handleMouseEvent(QWaylandView *target, QMouseEvent *me)
{
    auto popClient = popupClient();
    if (target && me->type() == QEvent::MouseButtonPress
            && popClient && popClient != target->surface()->client()) {
        m_wlShell->closeAllPopups();
    }

    QWaylandSeat *seat = defaultSeat();
    QWaylandSurface *surface = target ? target->surface() : nullptr;
    if(!surface)
        return;

    switch (me->type()) {
        case QEvent::MouseButtonPress:
            seat->sendMousePressEvent(me->button());
            if (surface != seat->keyboardFocus()) {
                // TODO: if we add more protocols we need to add kbd focus
                // things here
                if (surface == nullptr
                        || surface->role() == QWaylandWlShellSurface::role()
                        || surface->role() == HWWaylandXdgToplevel::role()
                        || surface->role() == QtSurface::role()
                        || findSurfaceObject(surface)->isFullscreenShell()) {
                    seat->setKeyboardFocus(surface);
                    seat->setMouseFocus(surface->primaryView());
                }
            }
            break;
    case QEvent::MouseButtonRelease:
         seat->sendMouseReleaseEvent(me->button());
         break;
    case QEvent::MouseMove:
         if(findSurfaceObject(surface) != nullptr)
         {
             seat->sendMouseMoveEvent(target, findSurfaceObject(surface)->mapToSurface(me->position()), me->globalPosition());
         }
         break;
    default:
        break;
    }
}

void Compositor::handleResize(SurfaceView *target, const QSize &initialSize, const QPoint &delta, int edge)
{
    // This function is handled on compositor level as it is triggered
    // From the QWindow of individual screens
    QWaylandWlShellSurface *wlShellSurface = target->surfaceObject()->wlShellSurface();
    if (wlShellSurface)
    {
        QWaylandWlShellSurface::ResizeEdge edges = QWaylandWlShellSurface::ResizeEdge(edge);
        QSize newSize = wlShellSurface->sizeForResize(initialSize, delta, edges);
        auto qedge = static_cast<Qt::Edge>(edge);

        if(qedge == Qt::TopEdge)
            target->m_surface->m_surfacePosition.setY(m_globalCursorPos.y());

        if(qedge == Qt::LeftEdge)
            target->m_surface->m_surfacePosition.setX(m_globalCursorPos.x());
        wlShellSurface->sendConfigure(newSize, edges);
    }

    // TODO: Qt Surface resize

    HWWaylandXdgSurface *xdgSurface = target->surfaceObject()->xdgSurface();
    if (xdgSurface)
    {
        HWWaylandXdgToplevel *topLevel = target->surfaceObject()->xdgTopLevel();
        if(topLevel && !topLevel->maximized())
        {
            auto qedge = static_cast<Qt::Edge>(edge);

            QSize newSize = topLevel->sizeForResize(initialSize, delta, qedge);
            if(qedge == Qt::TopEdge && newSize.height() != m_resizeOldVal)
                    target->m_surface->m_surfacePosition.setY(m_globalCursorPos.y());

            if(qedge == Qt::LeftEdge && newSize.width() != m_resizeOldVal)
                target->m_surface->m_surfacePosition.setX(m_globalCursorPos.x());

            topLevel->sendResizing(newSize);

            if(qedge == Qt::TopEdge)
                m_resizeOldVal = newSize.height();

            if(qedge == Qt::LeftEdge)
                m_resizeOldVal = newSize.width();

        }
    }
}

void Compositor::startDrag()
{
    QWaylandDrag *currentDrag = defaultSeat()->drag();
    Q_ASSERT(currentDrag);
    Surface *iconSurface = findSurfaceObject(currentDrag->icon());
    //SurfaceView *view = findView(currentDrag->icon());
    iconSurface->setPosition(primaryOutput()->window()->mapFromGlobal(QCursor::pos()));

    emit dragStarted(iconSurface);
}

void Compositor::handleDrag(SurfaceView *target, QMouseEvent *me)
{
    QPointF pos = me->position();
    QWaylandSurface *surface = nullptr;
    if (target) {
        surface = target->surface();
        pos -= findSurfaceObject(surface)->renderPosition();
    }
    QWaylandDrag *currentDrag = defaultSeat()->drag();
    // TODO: properly handle drag/drop icon surface
    if(surface)
        currentDrag->dragMove(surface, pos);
    if (me->buttons() == Qt::NoButton) {
        // TODO: destroy surface??
        m_surfaces.removeOne(findSurfaceObject(currentDrag->icon()));
        currentDrag->drop();
    }
}

QWaylandClient *Compositor::popupClient() const
{
    auto client = m_wlShell->popupClient();
    return client ;//? client : m_xdgShell->popupClient();
}

void Compositor::raise(Surface *obj)
{
    if(!obj)
    {
        qDebug(lc) << "Compositor::raise: request to raise invalid surface";
        return;
    }

    if(!m_surfaces.contains(obj))
    {
        qDebug(lc) << "Compositor::raise: request to raise recycled surface";
        return;
    }

    // we're already raised
    if(m_activated == obj)
        return;

    if(obj->isSpecialShellObject())
        return;

    qDebug(lc) << QString("Compositor::raise %1").arg(obj->uuid().toString(QUuid::WithoutBraces));

    if(!m_zorder.contains(obj))
    {
        activate(obj);
        return;
    }

    m_zorder.removeOne(obj);
    m_zorder.push_back(obj);
    activate(obj);
    // Since the compositor is only tracking unparented objects
    // we leave the ordering of children to SurfaceObject

    if(m_wndmgr)
    {
        if(m_wndmgr->isInitialized())
           m_wndmgr->updateZOrder(surfaceZOrderByUUID());
    }
    if(!obj->isXdgPopup())
        m_tl_raised = obj;
    // if we have a subchild, then activate further
    if(obj->childXdgSurfaceObjects().count() > 0)
    {
        raise(obj->childXdgSurfaceObjects().first());
    }
}

void Compositor::activate(Surface *obj)
{
    if(!obj)
    {
        qWarning() << "request to activate invalid surface";
        return;
    }
    if(!obj->surface())
        return;

    try {
        qDebug(lc) << QString("Compositor::activate %1").arg(obj->uuid().toString(QUuid::WithoutBraces));
        auto last = m_tl_raised;

        // Since the compositor is only tracking unparented objects
        // we leave the ordering of children to SurfaceObject
        if((obj->m_xdgTopLevel ||
             obj->m_gtk ||
             obj->m_qt)&& !obj->isSpecialShellObject())
        {
            if(m_menuServer)
               m_menuServer->setTopWindowForMenuServer(obj);

            m_tl_raised = obj;
            if(last && last != obj)
               last->deactivate();
        }

        if(obj->surface() && !obj->surface()->isDestroyed())
        {
           defaultSeat()->setKeyboardFocus(obj->surface());
           defaultSeat()->setMouseFocus(obj->surface()->primaryView());
        }

        obj->activate();
        m_activated = obj;
    }
    catch(int e)
    {
        qDebug(lc) << "Compositor::activate: error activating surface" << e;
    }
}

void Compositor::addOutput(Output *output)
{
    //connect(output, &Output::availableGeometryChanged, this, )
    m_outputs.append(output);
}

Output* Compositor::primaryOutput() const
{
    return m_outputs.first();
}

bool Compositor::legacyRender() const
{
    return m_legacyExperience;
}

bool Compositor::useAnimations() const
{
    return true;
}

void Compositor::resetIdle()
{
    if(m_timeout)
        m_timeout->stop();
    if(!m_idle_inhibit)
        setupIdleTimer();
}

void Compositor::removeOutput(Output *output)
{
    Q_UNUSED(output);
}
