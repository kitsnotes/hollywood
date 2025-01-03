// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
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

Output::Output(QScreen *s, QWindow *output, bool defaultScreen)
    : QWaylandOutput(wcApp->waylandCompositor(), output)
    , m_primary(defaultScreen)
    , m_screen(s)
{
    setManufacturer(s->manufacturer());
    setModel(s->model());
    setPhysicalSize(s->physicalSize().toSize());
    m_available_modes = Originull::Platform::EglFSFunctions::modesForScreen(s);
    QWaylandOutputMode mode(m_screen->size(), m_screen->refreshRate());
    setCurrentMode(mode);

    qCInfo(hwOutput, "Supporting xdg_output (protocol version 3)");
    m_xdg_output = new QWaylandXdgOutputV1(this, wcApp->compositor()->xdgOutputManager());
    m_xdg_output->setName(s->name());
    m_xdg_output->setLogicalPosition(QPoint(0,0));
    m_xdg_output->setLogicalSize(s->size());
    m_xdg_output->setDescription(QString("%1 %2").arg(s->manufacturer(), s->model()));

    //connect(m_wlOutput, &QWaylandOutput::currentModeChanged, this, &Output::modeChanged);
    connect(s, &QScreen::availableGeometryChanged, this, [this]() {
        qCDebug(hwOutput, "available geom changed: %i x %i",m_screen->size().width(), m_screen->size().height());
        window()->resize(m_screen->size().width(), m_screen->size().height());
        QWaylandOutputMode mode(m_screen->size(), m_screen->refreshRate());
        setCurrentMode(mode);
        m_xdg_output->setLogicalSize(mode.size());
        if(hwWindow() != nullptr)
            hwWindow()->wallpaperManager()->wallpaperChanged();
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

    window()->resize(s->size());
}

Output::~Output()
{

}

OutputWindow *Output::hwWindow()
{
    return qobject_cast<OutputWindow*>(window());
}

QSize Output::size() const
{
    return window()->size();
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

void Output::modesetFromConfig()
{

}

void Output::sleepDisplay()
{
    Originull::Platform::EglFSFunctions::setPowerState(m_screen,
                            Originull::Platform::EglFSFunctions::PowerStateSuspend);
}

void Output::wakeDisplay()
{
    Originull::Platform::EglFSFunctions::setPowerState(m_screen,
                       Originull::Platform::EglFSFunctions::PowerStateOn);
}

void Output::setConfigPosition(const QPoint &point)
{
    m_config_position = point;
}

QPoint Output::configPosition() const
{
    return m_config_position;
}

void Output::modeset(const QSize &size, const unsigned int refresh)
{
    qCInfo(hwOutput, "Changing mode to %ix%i at %i Hz", size.width(),size.height(),refresh);
    Originull::Platform::ScreenChange chg;
    chg.screen = m_screen;
    chg.position = position();
    chg.enabled = true;
    chg.scale = 1.0;
    chg.refreshRate = refresh*1000;
    chg.resolution = size;

    // TODO: test, check if valid mode
    QVector<Originull::Platform::ScreenChange> changes;
    changes << chg;
    Originull::Platform::EglFSFunctions::applyScreenChanges(changes);
}

uint Output::defaultScaleFactor()
{
    if(m_screen->physicalDotsPerInch() > 190)
        return 2;

    return 1;
}

QList<QPlatformScreen::Mode> Output::availableModes()
{
    return m_available_modes;
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
        if(anchors == WlrLayerSurfaceV1::TopAnchor ||
            anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::LeftAnchor ||
            anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::RightAnchor ||
            anchors == WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::RightAnchor)
            margins.setTop(margins.top()+reserved);

        /*if(anchors == WlrLayerSurfaceV1::LeftAnchor ||
            anchors == WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::TopAnchor ||
            anchors == WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::BottomAnchor ||
            anchors == WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::BottomAnchor)
            margins.setLeft(margins.left()+reserved);

        if(anchors == WlrLayerSurfaceV1::RightAnchor ||
            anchors == WlrLayerSurfaceV1::RightAnchor+WlrLayerSurfaceV1::TopAnchor ||
            anchors == WlrLayerSurfaceV1::RightAnchor+WlrLayerSurfaceV1::TopAnchor+WlrLayerSurfaceV1::BottomAnchor ||
            anchors == WlrLayerSurfaceV1::RightAnchor+WlrLayerSurfaceV1::BottomAnchor)
            margins.setRight(margins.right()+reserved);

        if(anchors == WlrLayerSurfaceV1::BottomAnchor ||
            anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::LeftAnchor ||
            anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::LeftAnchor+WlrLayerSurfaceV1::RightAnchor ||
            anchors == WlrLayerSurfaceV1::BottomAnchor+WlrLayerSurfaceV1::RightAnchor)
            margins.setBottom(margins.bottom()+reserved);*/
    }
    setAvailableGeometry(rect.marginsRemoved(margins));
}

