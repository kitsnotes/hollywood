// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "fullscreen.h"
#include <QWaylandSurface>

#define FULLSCREEN_VERSION 1

FullscreenShell::FullscreenShell(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<FullscreenShell>(compositor)
    , QtWaylandServer::zwp_fullscreen_shell_v1(compositor->display(), FULLSCREEN_VERSION)
{
    //qCInfo(hwLayerShell, "Supporting fullscreen_shell (protocol version %i)", FULLSCREEN_VERSION);
}

void FullscreenShell::zwp_fullscreen_shell_v1_present_surface(Resource *resource, wl_resource *surface, uint32_t method, wl_resource *output)
{
    Q_UNUSED(resource);
    Q_UNUSED(method);
    Q_UNUSED(output); // TODO: multi-output honor this choice

    QWaylandSurface *qsurface = QWaylandSurface::fromResource(surface);

    emit surfacePresented(qsurface);
}
