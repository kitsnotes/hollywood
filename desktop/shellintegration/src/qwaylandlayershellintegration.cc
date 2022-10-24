/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *   SPDX-FileCopyrightText: 2018 Drew DeVault <sir@cmpwn.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "qwaylandlayershell_p.h"
#include "qwaylandlayershellintegration_p.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <qwayland-wlr-layer-shell-unstable-v1.h>

namespace LayerShellQt
{
QWaylandLayerShellIntegration::QWaylandLayerShellIntegration()
{
}

QWaylandLayerShellIntegration::~QWaylandLayerShellIntegration()
{
}

bool QWaylandLayerShellIntegration::initialize(QtWaylandClient::QWaylandDisplay *display)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QWaylandShellIntegration::initialize(display);
#endif
    display->addRegistryListener(registryLayer, this);

    for (QtWaylandClient::QWaylandDisplay::RegistryGlobal global : display->globals()) {
        if (global.interface == QLatin1String("xdg_wm_base")) {
            m_xdgShell.reset(new QtWaylandClient::QWaylandXdgShell(display, global.id, global.version));
            break;
        }
    }

    if (!m_xdgShell) {
        qDebug() << "Couldn't find global xdg_wm_base for xdg-shell stable";
        return false;
    }

    return m_layerShell != nullptr;
}

QtWaylandClient::QWaylandShellSurface *QWaylandLayerShellIntegration::createShellSurface(QtWaylandClient::QWaylandWindow *window)
{
    if(window->window()->flags() & Qt::FramelessWindowHint &&
            window->window()->flags() &~ Qt::Popup)
        return m_layerShell->createLayerSurface(window);

    return m_xdgShell->getXdgSurface(window);
}

void QWaylandLayerShellIntegration::registryLayer(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    QWaylandLayerShellIntegration *shell = static_cast<QWaylandLayerShellIntegration *>(data);

    if (interface == zwlr_layer_shell_v1_interface.name)
        shell->m_layerShell.reset(new QWaylandLayerShell(registry, id, std::min(version, 4u)));
}

}

//#include "qwaylandlayershellintegration.moc"
