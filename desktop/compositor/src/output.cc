// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "output.h"
#include "outputwnd.h"
#include "application.h"
#include "compositor.h"
#include "surfaceobject.h"
#include "layershell.h"

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
    qDebug() << "Output::updateUsableGeometry()" << rect.marginsRemoved(margins);
    m_wlOutput->setAvailableGeometry(rect.marginsRemoved(margins));
}

void Output::configureForScreen(QScreen *s, bool defaultScreen)
{
    m_screen = s;
    QWaylandOutputMode mode(s->size(), s->refreshRate());
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setManufacturer(s->manufacturer());
    m_wlOutput->setModel(s->model());
    if(defaultScreen)
        wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);

    if(defaultScreen)
        m_wlOutput->setPosition(QPoint(0,0));
    m_wlOutput->setSizeFollowsWindow(true);
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

void Output::configureDebugWindow2()
{
    QWaylandOutputMode mode(QSize(1024,768), 60000);
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setPosition(QPoint(1025,0));
    m_wlOutput->setSizeFollowsWindow(true);
    m_wlOutput->setManufacturer(tr("Originull"));
    m_wlOutput->setModel(tr("Debug Wnd 2"));
    m_window->resize(1024,768);
    m_window->show();
    if(m_primary)
        m_window->setTitle(tr("Secondary Display"));

}
