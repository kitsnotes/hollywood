// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "compositor.h"

#include "view.h"
#include "surfaceobject.h"
#include "output.h"
#include "outputwnd.h"
#include "wallpaper.h"
#include "surfaceobject.h"

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
#include "relativepointer.h"
#include "pointerconstraints.h"
#include "shortcuts.h"

#include "xwayland.h"
#include "xwaylandserver.h"
#include "xwaylandshellsurface.h"

#include <QSettings>
#include <QPainter>
#include <QTextOption>
#include <QFile>
#include <hollywood/hollywood.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <hollywood/hollywood.h>
#include <stdlib.h>
#include <QMessageBox>

Q_LOGGING_CATEGORY(hwCompositor, "compositor.core")

Compositor::Compositor(bool sddm)
    : m_desktop_bg(QColor(HOLLYWOOD_DEF_DESKTOP_BG))
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
    , m_screencopy(new WlrScreencopyManagerV1(this))
    , m_viewporter(new QWaylandViewporter(this))
    , m_relative_pointer(new RelativePointerManagerV1(this))
    , m_pointer_constraints(new PointerConstraintsV1(this))
    , m_idle_inhibit_protocol(new QWaylandIdleInhibitManagerV1(this))
    , m_outputmgr(new QWaylandXdgOutputManagerV1(this))
    , m_sddm(sddm)
    , m_shortcuts(new ShortcutManager(this))
    , m_timeout(new QTimer(this))
{    
    qCInfo(hwCompositor, "Supporting wp_viewporter (protocol version 1)");
    qCInfo(hwCompositor, "Supporting xdg_decoration_manager_v1 (protocol version 1)");


    auto fd = qgetenv("WAYLAND_SOCKET_FD");
    bool ok = false;
    fd.toInt(&ok);

    if(!fd.isEmpty() && ok)
    {
        addSocketDescriptor(fd.toInt());
        setSocketName("");
        qCInfo(hwCompositor, "Using wlproxy socket handoff");
        qunsetenv("WAYLAND_SOCKET_FD");
    }

    /*auto sockname = qgetenv("WAYLAND_SOCKET_NAME");
    if(!sockname.isEmpty())
    {
        setSocketName(sockname);
        qunsetenv("WAYLAND_SOCKET_NAME");
    }*/

    auto envvar = qgetenv("HOLLYWOOD_RECOVERY_ENV");
    if(!envvar.isEmpty())
    {
        if(envvar == "1")
        {
            qCInfo(hwCompositor, "Using 'mini mode'");
            m_mini = true;
        }
    }
    m_timeout->setTimerType(Qt::VeryCoarseTimer);
    loadSettings();
    m_cfgwatch = new QFileSystemWatcher();
    m_cfgwatch->addPath(m_configfile);
    connect(m_cfgwatch, &QFileSystemWatcher::fileChanged,
            this, &Compositor::configChanged);

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

    m_xdgDecoration->setExtensionContainer(this);
    m_xdgDecoration->initialize();
    m_xdgDecoration->setPreferredMode(QWaylandXdgToplevel::ServerSideDecoration);
    m_outputmgr->setExtensionContainer(this);
    m_outputmgr->initialize();
    m_viewporter->initialize();
    connect(m_appMenu, &AppMenuManager::appMenuCreated, this, &Compositor::appMenuCreated);
    connect(m_hwPrivate, &OriginullProtocol::menuServerSet, this, &Compositor::onMenuServerRequest);
    connect(m_hwPrivate, &OriginullProtocol::wallpaperRotationRequested, this, &Compositor::onRotateWallpaper);
    connect(m_xdgShell, &HWWaylandXdgShell::xdgSurfaceCreated, this, &Compositor::onXdgSurfaceCreated);
    connect(m_xdgShell, &HWWaylandXdgShell::toplevelCreated, this, &Compositor::onXdgTopLevelCreated);
    connect(m_xdgShell, &HWWaylandXdgShell::popupCreated, this, &Compositor::onXdgPopupCreated);
    connect(m_layerShell, &WlrLayerShellV1::layerSurfaceCreated, this, &Compositor::onLayerSurfaceCreated);
    connect(m_gtk, &GtkShell::surfaceCreated, this, &Compositor::onGtkSurfaceCreated);
    connect(m_qt, &QtShell::surfaceCreated, this, &Compositor::onQtSurfaceCreated);
    connect(m_fs, &FullscreenShell::surfacePresented, this, &Compositor::onFullscreenSurfaceRequested);
    connect(m_activation, &XdgActivation::surfaceActivated, this, &Compositor::onXdgSurfaceActivated);

    setupX11();
}

void Compositor::setupX11()
{
    if(m_x11)
        return;

    qCInfo(hwCompositor, "Supporting Xwayland");
    m_xserver = new XWaylandServer(this);
    m_x11 = new XWaylandManager;
    m_x11->setCompositor(this);
    connect(m_x11, &XWaylandManager::shellSurfaceCreated, this, &Compositor::onXWaylandShellSurfaceCreated);
    connect(m_x11, &XWaylandManager::shellSurfaceRequested, this, &Compositor::onXWaylandShellSurfaceRequested);
    m_x11->setServer(m_xserver);
    connect(m_xserver, &XWaylandServer::started, this, [this](const QString &displayName) {
        m_x_display = displayName;
        m_x11->start(m_xserver->wmFd());
    });
    m_xserver->start();

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
        m_layer_bg.append(s);
        if(s->m_wndctl)
        {
            s->m_wndctl->deleteLater();
            s->m_wndctl = nullptr;
        }
        s->m_surfaceType = Surface::Desktop;
        s->m_ssd = false;
        s->m_isShellDesktop = true;
        s->m_surfacePosition = s->primaryView()->output()->availableGeometry().topLeft();
        s->m_ls_size = s->primaryView()->output()->availableGeometry().size();
        s->m_layerSurface->sendConfigure(s->primaryView()->output()->availableGeometry().width(),
                                         s->primaryView()->output()->availableGeometry().height());
        s->setPosition(s->primaryView()->output()->availableGeometry().topLeft());
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
    QListIterator i(m_zorder);
    i.toBack();
    while(i.hasPrevious())
    {
        auto obj = i.previous();
        if(obj->isSpecialShellObject())
            continue;
        if(obj->plasmaControl())
            uuids.append(obj->plasmaControl()->uuid().toString(QUuid::WithoutBraces));
    }

    return uuids.join(" ");
}

void Compositor::generateDesktopInfoLabelImage()
{
    return;
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
    for(auto *out : m_outputs)
    {
        if(out->wlOutput()->geometry().contains(point.toPoint()))
        {
            if(!out->wlOutput()->availableGeometry().contains(point.toPoint()))
            {
                // TODO: multiple monitor - is there an ajacent monitor boundry?
                auto rect = out->wlOutput()->availableGeometry();
                int closestX = qMax(rect.left(), qMin(point.toPoint().x(), rect.right()));
                int closestY = qMax(rect.top(), qMin(point.toPoint().y(), rect.bottom()));
                return QPointF(closestX, closestY);
            }
        }
    }

    return point;
}

ShortcutManager *Compositor::shortcuts()
{
    return m_shortcuts;
}

QWaylandXdgOutputManagerV1 *Compositor::xdgOutputManager() { if(m_outputmgr) return m_outputmgr;  return nullptr;}

RelativePointerManagerV1 *Compositor::relativePointerManager() { if(m_relative_pointer) return m_relative_pointer; return nullptr; }

void Compositor::addIdleInhibit(Surface *surface)
{
    if(!m_inhibit_surfaces.contains(surface))
        m_inhibit_surfaces.append(surface);

    if(m_timeout->isActive())
    {
        qCDebug(hwCompositor, "idle inhibitor stopping idle timer");
        m_timeout->stop();
    }
}

void Compositor::removeIdleInhibit(Surface *surface)
{
    m_inhibit_surfaces.removeOne(surface);
    if(m_inhibit_surfaces.count() == 0)
    {
        qCDebug(hwCompositor, "no more idle inhibitors, restarting idle timer");
        setupIdleTimer();
    }
}

bool Compositor::isInhibitingIdle()
{
    return m_inhibit_surfaces.count() == 0 ? false : true;
}

void Compositor::wake()
{
    qCInfo(hwCompositor, "wake event");
    m_display_sleeping = false;
    //TODO: multiple displays
    primaryOutput()->wakeDisplay();
    setupIdleTimer();
    triggerRender();
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
    auto uuid = obj->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: wl_surface created", uuid);

    // handle xwayland
    if(surface->client())
    {
        if(surface->client()->client() == m_xserver->client())
        {
            for(auto shl : m_x11->m_unpairedWindows)
            {
                quint32 id = wl_resource_get_id(surface->resource());
                if(shl->surfaceId() == id)
                {
                    qCDebug(hwCompositor, "%s: wl_surface is X11", uuid);
                    shl->setSurfaceId(0);
                    shl->setSurface(surface);
                    m_x11->m_unpairedWindows.removeOne(shl);
                    obj->setXWaylandShell(shl);
                    break;
                }
            }
        }
    }
    m_surfaces << (obj);
    m_zorder << (obj);
}

void Compositor::recycleSurfaceObject(Surface *obj)
{
    auto uuid = obj->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: recycling surface", uuid);

    if(obj->xdgTopLevelParent())
        obj->xdgTopLevelParent()->removeXdgTopLevelChild(obj);

    m_surfaces.removeOne(obj);
    m_zorder.removeOne(obj);
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
                || surface->role() == GtkSurface::role()
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

void Compositor::loadSettings()
{
    qCInfo(hwCompositor, "reloading settings");

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

    if(miniMode())
        m_desktop_bg = QColor(Qt::black);

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

    if(miniMode())
        m_legacyExperience = true;

    settings.endGroup();
    settings.beginGroup(QLatin1String("Energy"));

    // TODO: put these defaults into hollywood.h
    m_timeout_display = settings.value("DisplayTimeout", 120).toUInt();
    m_timeout_sleep = settings.value("SleepTimeout", 420).toUInt();

    // monitors should not sleep after the system does
    if(m_timeout_display >= m_timeout_sleep)
        m_timeout_display = 0;

    settings.endGroup();

    /*if(m_timeout->isActive())
        m_timeout->stop();*/
    setupIdleTimer();
    emit settingsChanged();
}

void Compositor::setupIdleTimer()
{
    return;

    if(isInhibitingIdle())
        return;

    m_timeout->setSingleShot(true);
    // setup the timer
    if(!m_display_sleeping && m_timeout_display > 0)
        m_timeout->start(m_timeout_display*1000);
    else if(m_display_sleeping || (m_timeout_display == 0 && m_timeout_sleep > 0))
    {
        if(m_timeout_display == 0)
            m_timeout->start(m_timeout_sleep*1000);
        else
            m_timeout->start((m_timeout_sleep-m_timeout_display)*1000);
    }
}

void Compositor::idleTimeout()
{
    // idle inhibited by client, don't do anything
    if(isInhibitingIdle())
    {
        qCDebug(hwCompositor, "idle would happen, but currently inhibited");
        return;
    }

    qCDebug(hwCompositor, "idle timeout");

    // timeout to handle a display sleep
    if(!m_display_sleeping && m_timeout_display > 0)
    {
        // display not sleeping, display timeout > 0 so this
        // will be a display sleep command

        if(m_lock_after_sleep == 0)
            lockSession();
        else
            QTimer::singleShot(m_lock_after_sleep, this, &Compositor::lockSession);

        qCDebug(hwCompositor, "placing displays to sleep");
        // TODO: multiple displays
        m_display_sleeping = true;
        triggerRender();
        primaryOutput()->sleepDisplay();

        if(m_timeout_sleep > 0)
            setupIdleTimer();

        return;
    }

    // timeout to handle a system sleep
    if(m_display_sleeping ||
       (m_timeout_display == 0 &&
        m_timeout_sleep > 0))
    {
        qCDebug(hwCompositor, "turning off displays, placing system to sleep");

        // we don't have display sleep but we do have system sleep
        lockSession();
        // TODO: tell session to put system to sleep
    }
}

void Compositor::onXWaylandShellSurfaceRequested(quint32 window, const QRect &geometry, bool overrideRedirect, XWaylandShellSurface *parentShellSurface)
{
    auto qclient = QWaylandClient::fromWlClient(this, m_xserver->client());
    if(qclient)
    {
        auto xwlshell = new XWaylandShellSurface(this);
        xwlshell->initialize(m_x11, window, geometry, overrideRedirect, parentShellSurface);
    }
}

void Compositor::onXWaylandShellSurfaceCreated(XWaylandShellSurface *shellSurface)
{

}

void Compositor::raiseNextInLine()
{
    qCDebug(hwCompositor, "raise next in line");
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

    if(!raised && m_layer_bg.count() > 0)
        activate(m_layer_bg.last());
}

void Compositor::onXdgSurfaceCreated(HWWaylandXdgSurface *xdgSurface)
{
    auto *surface = findSurfaceObject(xdgSurface->surface());
    Q_ASSERT(surface);
    auto uuid = surface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: xdg_surface created", uuid);
    surface->createXdgShellSurface(xdgSurface);
}

void Compositor::onXdgPopupCreated(HWWaylandXdgPopup *popup, HWWaylandXdgSurface *surface)
{
    Surface *mySurface = findSurfaceObject(surface->surface());
    Q_ASSERT(mySurface);
    m_zorder.removeOne(mySurface);
    auto uuid = mySurface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: xdg_popup created", uuid);
    mySurface->createXdgPopupSurface(popup);
}

void Compositor::onLayerSurfaceCreated(WlrLayerSurfaceV1 *layerSurface)
{
    auto *surface = findSurfaceObject(layerSurface->surface());
    Q_ASSERT(surface);
    auto uuid = surface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: layer_shell surface created", uuid);
    surface->createLayerShellSurface(layerSurface);
}

void Compositor::onGtkSurfaceCreated(GtkSurface *gtkSurface)
{
    Surface *surface = findSurfaceObject(gtkSurface->surface());
    Q_ASSERT(surface);
    auto uuid = surface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: gtk_shell surface created", uuid);
    surface->createGtkSurface(gtkSurface);
}

void Compositor::onQtSurfaceCreated(QtSurface *qtSurface)
{
    Surface *surface = findSurfaceObject(qtSurface->surface());
    Q_ASSERT(surface);
    auto uuid = surface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: qt_shell surface created", uuid);
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
    auto uuid = mySurface->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: xdg_toplevel surface created", uuid);
    // this is temporary and will be better calculated in onXdgWindowGeometryChanged
    mySurface->setPosition(QPointF(1,1));

    m_tl_raised = mySurface;
}

void Compositor::onStartMove(QWaylandSeat *seat)
{
    Q_UNUSED(seat);
    // TODO: close xdg_popups
    emit startMove();
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
    if(sf)
    {
        auto uuid = sf->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
        qCDebug(hwCompositor, "%s: wl_surface activated", uuid);
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
    if(m_cursorObject->primaryView())
        m_cursorObject->primaryView()->advance();
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
        // TODO: close xdg_popups
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
                if (surface != nullptr
                        || surface->role() == QWaylandWlShellSurface::role()
                        || surface->role() == HWWaylandXdgToplevel::role()
                        || surface->role() == GtkSurface::role()
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
    auto qedge = static_cast<Qt::Edges>(edge);

    // This function is handled on compositor level as it is triggered
    // From the QWindow of individual screens
    XWaylandShellSurface *xwlShellSurface = target->surfaceObject()->getXWaylandShellSurface();
    if (xwlShellSurface)
    {
        XWaylandShellSurface::ResizeEdge edges = XWaylandShellSurface::ResizeEdge(edge);
        QSize newSize = xwlShellSurface->sizeForResize(initialSize, delta, edges);

        if(qedge&Qt::TopEdge)
            target->m_surface->m_surfacePosition.setY(m_globalCursorPos.y());

        if(qedge&Qt::LeftEdge)
            target->m_surface->m_surfacePosition.setX(m_globalCursorPos.x());
        xwlShellSurface->sendResize(newSize);
    }

    QWaylandWlShellSurface *wlShellSurface = target->surfaceObject()->wlShellSurface();
    if (wlShellSurface)
    {
        QWaylandWlShellSurface::ResizeEdge edges = QWaylandWlShellSurface::ResizeEdge(edge);
        QSize newSize = wlShellSurface->sizeForResize(initialSize, delta, edges);

        if(qedge|Qt::TopEdge)
            target->m_surface->m_surfacePosition.setY(m_globalCursorPos.y());

        if(qedge&Qt::LeftEdge)
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
            QSize newSize = topLevel->sizeForResize(initialSize, delta, qedge);
            if(qedge & Qt::TopEdge && newSize.height() != m_resizeOldVal)
                    target->m_surface->m_surfacePosition.setY(m_globalCursorPos.y());

            if(qedge & Qt::LeftEdge && newSize.width() != m_resizeOldVal)
                target->m_surface->m_surfacePosition.setX(m_globalCursorPos.x());

            topLevel->sendResizing(newSize);

            if(qedge & Qt::TopEdge)
                m_resizeOldVal = newSize.height();

            if(qedge & Qt::LeftEdge)
                m_resizeOldVal = newSize.width();

        }
    }
}

void Compositor::startDrag()
{
    QWaylandDrag *currentDrag = defaultSeat()->drag();
    Q_ASSERT(currentDrag);
    Surface *iconSurface = findSurfaceObject(currentDrag->icon());
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
    if(surface)
        currentDrag->dragMove(surface, pos);
    if (me->buttons() == Qt::NoButton) {
        // TODO: destroy surface??
        defaultSeat()->setKeyboardFocus(surface);
        // TODO: multiple monitors/views
        defaultSeat()->setMouseFocus(nullptr);
        surface->updateSelection();
        currentDrag->drop();
        m_surfaces.removeOne(findSurfaceObject(currentDrag->icon()));
    }
}

QWaylandClient *Compositor::popupClient() const
{
    return nullptr;
}

void Compositor::raise(Surface *obj)
{
    if(!obj)
    {
        qCDebug(hwCompositor, "request to raise invalid surface");
        return;
    }

    if(!m_surfaces.contains(obj))
    {
        qCDebug(hwCompositor, "request to raise recycled surface");
        return;
    }

    // we're already raised
    if(m_activated == obj)
        return;

    if(obj->isSpecialShellObject())
        return;

    auto uuid = obj->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
    qCDebug(hwCompositor, "%s: raise", uuid);
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
        qCDebug(hwCompositor, "request to activate invalid surface");
        return;
    }
    if(!obj->surface())
        return;

    try {
        auto uuid = obj->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
        qCDebug(hwCompositor, "%s: activated", uuid);

        auto last = m_tl_raised;

        // Since the compositor is only tracking unparented objects
        // we leave the ordering of children to SurfaceObject
        if((obj->m_xdgTopLevel ||
             obj->m_gtk ||
             obj->m_qt ||
             obj->getXWaylandShellSurface() != nullptr ||
             (obj->m_layerSurface && obj->m_layerSurface->layer() == WlrLayerShellV1::BackgroundLayer))
             && !obj->isSpecialShellObject())
        {
            if(m_menuServer)
               m_menuServer->setTopWindowForMenuServer(obj);

            m_tl_raised = obj;
            if(last && last != obj)
               last->deactivate();
        }

        obj->activate();
        m_activated = obj;
    }
    catch(int e)
    {
        auto uuid = obj->uuid().toString(QUuid::WithoutBraces).toUtf8().data();
        qCDebug(hwCompositor, "%s: error %i activating surface", uuid, e);
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
    return;
    if(m_timeout)
        m_timeout->stop();
    m_timeout->start();
}

void Compositor::removeOutput(Output *output)
{
    Q_UNUSED(output);
}

void handleShutDownSignal(int /* signalId */)
{
    return;
}

CompositorApp::CompositorApp(bool use_sddm, int &argc, char **argv)
    :QGuiApplication(argc, argv),
    m_compositor(new Compositor(use_sddm))
{
    setApplicationName("Hollywood Compositor");
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    Originull::Platform::EglFSFunctions::setCursorTheme(QLatin1String("neutral"), 32);
    m_compositor->create();
}

Compositor* CompositorApp::compositor()
{
    return m_compositor;
}

QWaylandCompositor* CompositorApp::waylandCompositor()
{
    return static_cast<QWaylandCompositor*>(m_compositor);
}
void CompositorApp::doInit()
{
    if(platformName() == QLatin1String("hollywood") ||
        platformName() == QLatin1String("wayland") ||
        platformName() == QLatin1String("xcb"))
    {
        /* right now we don't support nested wayland so error out */
        QMessageBox::critical(0, tr("Unsupported Compositor Action"),
                              tr("Currently Hollywood does not support using a nested compositor.  Now Exiting."));
        exit(0);
    }
    if(platformName() == QLatin1String("eglfs") ||
        platformName() == QLatin1String("hwc-eglfs"))
        createForDisplays();
}

void CompositorApp::createForDisplays()
{
    auto ps = primaryScreen();
    auto output = new Output(ps, true);
    output->window()->show();
    output->modesetFromConfig();
}

int main(int argc, char *argv[])
{
    const char *name = "sddm";
    struct passwd *p;
    bool is_sddm = false;
    p = getpwnam(name);
    if(p != NULL)
    {
        if(getuid() == p->pw_uid)
            is_sddm = true;
    }

    qputenv("QT_QPA_PLATFORM", "hwc-eglfs");
    if(!is_sddm)
    {
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = handleShutDownSignal;
        sigaction(SIGINT, &sa, NULL);
    }
    else
    {
        QString xdg = QString("/run/user/%1").arg(getuid());
        qputenv("XDG_RUNTIME_DIR", xdg.toUtf8().data());
    }

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    CompositorApp app(is_sddm, argc, argv);
    app.doInit();

    return app.exec();
}
