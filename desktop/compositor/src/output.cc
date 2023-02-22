// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "output.h"
#include "outputwnd.h"
#include "application.h"
#include "compositor.h"
#include "surfaceobject.h"
#include "layershell.h"

#include <QSettings>
#include <QDateTime>

Output::Output(QObject *parent)
    : QObject(parent)
    , m_window(new OutputWindow(this))
{
    m_wlOutput = new QWaylandOutput(wcApp->waylandCompositor(), m_window);
    wcApp->compositor()->addOutput(this);
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
    connect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this, &Output::reservedRegionsChanged);
    connect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this, &Output::reservedRegionsChanged);
    updateUsableGeometry();
    Q_EMIT reservedRegionsChanged();
    return true;
}

bool Output::removeLayerShellReservation(Surface *surface)
{
    if(!m_reserved.contains(surface))
        return false;

    m_reserved.removeOne(surface);
    disconnect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this, &Output::reservedRegionsChanged);
    disconnect(surface->layerSurface(), &WlrLayerSurfaceV1::exclusiveZoneChanged, this, &Output::reservedRegionsChanged);

    updateUsableGeometry();
    Q_EMIT reservedRegionsChanged();
    return true;
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
    auto scale = settings.value("ScaleFactor", defaultScaleFactor()).toUInt();
    if(scale > 4)
        scale = 4;

    if(m_wlOutput->scaleFactor() != scale)
        m_wlOutput->setScaleFactor(scale);
}

uint Output::defaultScaleFactor()
{
    // TODO: see if this is high dpi, adjust accordingly
    return 1;
}

void Output::updateUsableGeometry()
{
    QRect rect = m_wlOutput->geometry();
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

void Output::configureForScreen(QScreen *s, bool defaultScreen)
{
    m_screen = s;
    m_wlOutput->setManufacturer(s->manufacturer());
    m_wlOutput->setModel(s->model());

    // set our preferred mode
    QWaylandOutputMode mode(s->size(), s->refreshRate());
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);

    if(defaultScreen)
        wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);

    if(defaultScreen)
        m_wlOutput->setPosition(QPoint(0,0));
    m_wlOutput->setSizeFollowsWindow(false);

    reloadConfiguration();
    touchConfiguration();

    m_window->resize(s->size());
    m_window->show();
}

void Output::configureDebugWindow()
{
    QWaylandOutputMode mode(QSize(1024,768), 60000);
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setManufacturer(tr("Originull"));
    m_wlOutput->setModel(tr("Debug Wnd 1"));

    wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);
    m_wlOutput->setPosition(QPoint(0,0));
    m_wlOutput->setSizeFollowsWindow(true);
    m_window->resize(1024,768);
    m_window->show();
    if(m_primary)
        m_window->setTitle(tr("Primary Display"));

}
