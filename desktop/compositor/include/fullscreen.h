#ifndef FULLSCREENSHELL_H
#define FULLSCREENSHELL_H

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandQuickExtension>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-fullscreen-shell-unstable-v1.h"

class Compositor;
class Surface;
class FullscreenSurface;
class FullscreenShell : public QWaylandCompositorExtensionTemplate<FullscreenShell>
               , public QtWaylandServer::zwp_fullscreen_shell_v1
{
    Q_OBJECT
public:
    FullscreenShell(QWaylandCompositor *compositor);
signals:
    void surfacePresented(QWaylandSurface *surface);
protected:
    void zwp_fullscreen_shell_v1_present_surface(Resource *resource, struct ::wl_resource *surface, uint32_t method, struct ::wl_resource *output) override;
};

#endif // FULLSCREENSHELL_H
