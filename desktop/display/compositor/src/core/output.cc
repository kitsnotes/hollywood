// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "output.h"
#include "outputwnd.h"
#include "compositor.h"
#include "surfaceobject.h"
#include "layershell.h"
#include "wallpaper.h"

#include <QSettings>
#include <QDateTime>

Q_LOGGING_CATEGORY(hwOutput, "compositor.output")

Output::Output(QScreen *s, bool defaultScreen)
    : QObject(nullptr)
    , m_window(new OutputWindow(this))
    , m_screen(s)
{
    qCInfo(hwOutput, "Supporting xdg_output (protocol version 3)");

    m_wlOutput = new QWaylandOutput(wcApp->waylandCompositor(), m_window);
    wcApp->compositor()->addOutput(this);
    connect(m_wlOutput, &QWaylandOutput::currentModeChanged, this, &Output::modeChanged);
    connect(s, &QScreen::availableGeometryChanged, this, [this]() {
        qCDebug(hwOutput, "available geom changed: %i x %i",m_screen->size().width(), m_screen->size().height());

        m_window->resize(m_screen->size().width(), m_screen->size().height());
        QWaylandOutputMode mode(m_screen->size(), 60);
        m_wlOutput->addMode(mode, true);
        m_wlOutput->setCurrentMode(mode);
        m_xdgOutput->setLogicalSize(mode.size());
        m_window->wallpaperManager()->wallpaperChanged();
        updateUsableGeometry();
        emit availableGeometryChanged(QRect(QPoint(0,0), m_screen->size()));
        for(auto lswnd : m_reserved)
        {
            lswnd->recalculateLayerShellAnchorPosition();
            lswnd->onLayerShellSizeChanged();
        }
        wcApp->compositor()->triggerRender();
    });
    connect(s, &QScreen::refreshRateChanged, this, [this]() {
        qCDebug(hwOutput, "refresh rate changed %f", m_screen->refreshRate());
    });
    m_wlOutput->setManufacturer(s->manufacturer());
    m_wlOutput->setModel(s->model());

    // set our preferred mode
    QWaylandOutputMode mode(s->size(), s->refreshRate());
    m_wlOutput->addMode(mode, true);
    /*m_wlOutput->addMode(QWaylandOutputMode(QSize(1920,1080), 75), true);
    m_wlOutput->addMode(QWaylandOutputMode(QSize(1920,1080), 60));*/

    m_wlOutput->setCurrentMode(mode);

    if(defaultScreen)
        wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);

    if(defaultScreen)
        m_wlOutput->setPosition(QPoint(0,0));

    reloadConfiguration();
    touchConfiguration();

    m_window->resize(s->size());
    m_window->show();

    m_xdgOutput = new QWaylandXdgOutputV1(m_wlOutput, wcApp->compositor()->xdgOutputManager());
    m_xdgOutput->setName(s->name());
    m_xdgOutput->setLogicalPosition(QPoint(0,0));
    m_xdgOutput->setLogicalSize(s->size());
    m_xdgOutput->setDescription(QString("%1 %2").arg(s->manufacturer(), s->model()));
}

QSize Output::size() const
{
    return m_window->size();
}

bool Output::reserveLayerShellRegion(Surface *surface)
{
    if(surface->layerSurface() == nullptr)
        return false;

    m_reserved.append(surface);
    connect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this,
            &Output::reservedRegionsChanged);
    connect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this,
            &Output::reservedRegionsChanged);
    updateUsableGeometry();
    Q_EMIT reservedRegionsChanged();
    return true;
}

bool Output::removeLayerShellReservation(Surface *surface)
{
    if(!m_reserved.contains(surface))
        return false;

    m_reserved.removeOne(surface);
    disconnect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged,
               this, &Output::reservedRegionsChanged);
    disconnect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged,
               this, &Output::reservedRegionsChanged);

    updateUsableGeometry();
    Q_EMIT reservedRegionsChanged();
    return true;
}

void Output::modeChanged()
{
    updateUsableGeometry();
}

QString Output::persistentSettingName() const
{
    // preferred
    if(!m_screen->serialNumber().isEmpty() &&
       !m_screen->manufacturer().isEmpty() &&
       !m_screen->model().isEmpty())
    return QString("%1-%2-%3").arg(m_screen->manufacturer(), m_screen->model(), m_screen->serialNumber());

    // fallback #1
    if(!m_screen->serialNumber().isEmpty() &&
       !m_screen->manufacturer().isEmpty())
    return QString("%1-%2").arg(m_screen->manufacturer(), m_screen->serialNumber());

    // fallback #2
    if(!m_screen->serialNumber().isEmpty())
        return QString("%1").arg(m_screen->serialNumber());

    // fallback #3
    if(!m_screen->model().isEmpty() &&
       !m_screen->manufacturer().isEmpty())
    return QString("%1-%2").arg(m_screen->manufacturer(), m_screen->model());
    // bare minimum

    return m_screen->name();
}

void Output::touchConfiguration()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup(QLatin1String("Displays"));

    settings.beginGroup(persistentSettingName());
    settings.setValue("LastSeen", QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
}

void Output::reloadConfiguration()
{
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup(QLatin1String("Displays"));
    settings.beginGroup(persistentSettingName());

    // TODO: set a resolution set if we have one set

    // set our scale factor
    auto scale = settings.value("ScaleFactor", defaultScaleFactor()).toInt();
    if(scale > 4)
        scale = 4;

    if(m_wlOutput->scaleFactor() != scale)
        m_wlOutput->setScaleFactor(scale);
}

void Output::modesetFromConfig()
{
    QSettings settings(QSettings::UserScope,
                       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup(QLatin1String("Displays"));
    settings.beginGroup(persistentSettingName());

    Originull::Platform::ScreenChange chg;
    chg.screen = m_screen;
    // TODO: fix for multiple monitors
    chg.position = QPoint(0,0);
    chg.enabled = true;
    chg.refreshRate = settings.value("Refresh", 60000).toInt();
    chg.scale = 1.0;
    chg.resolution = QSize(settings.value("Width", m_screen->geometry().width()).toInt()
                           ,settings.value("Height",m_screen->geometry().height()).toInt());

    // TODO: test, check if valid mode
    QVector<Originull::Platform::ScreenChange> changes;
    changes << chg;
    Originull::Platform::EglFSFunctions::applyScreenChanges(changes);
}

uint Output::defaultScaleFactor()
{
    qCDebug(hwOutput, "default scale factor: %f", m_screen->physicalDotsPerInch());
    if(m_screen->physicalDotsPerInch() > 190)
        return 1;

    return 1;
}

void Output::updateUsableGeometry()
{
    QRect rect = m_screen->geometry();
    QMargins margins(0,0,0,0);
    for(auto r : m_reserved)
    {
        if(r->layerSurface()->layer() != WlrLayerShellV1::TopLayer)
            continue;

        auto reserved = r->layerSurface()->exclusiveZone();
        auto anchors = r->anchors();
        if(anchors == WlrLayerSurfaceV1::TopAnchor)
            margins.setTop(margins.top()+reserved);
        if(anchors == WlrLayerSurfaceV1::LeftAnchor)
            margins.setLeft(margins.left()+reserved);
        if(anchors == WlrLayerSurfaceV1::RightAnchor)
            margins.setRight(margins.right()+reserved);
        if(anchors == WlrLayerSurfaceV1::BottomAnchor)
            margins.setBottom(margins.bottom()+reserved);

        // TODO: scope out multiple anchors based on height/width?
    }
    m_wlOutput->setAvailableGeometry(rect.marginsRemoved(margins));
}

