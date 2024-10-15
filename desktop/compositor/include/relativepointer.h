// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-relative-pointer-unstable-v1.h"

class Compositor;
class Surface;
class RelativePointerManagerV1;

class RelativePointerV1 : public QWaylandCompositorExtensionTemplate<RelativePointerV1>
    , public QtWaylandServer::zwp_relative_pointer_v1
{
    Q_OBJECT
public:
    RelativePointerV1(QWaylandSurface *surface, RelativePointerManagerV1 *parent);
public slots:
protected:
private:
    RelativePointerManagerV1 *m_parent = nullptr;
};

class RelativePointerManagerV1 : public QWaylandCompositorExtensionTemplate<RelativePointerManagerV1>
    , public QtWaylandServer::zwp_relative_pointer_manager_v1
{
    Q_OBJECT
public:
    RelativePointerManagerV1(QWaylandCompositor *compositor);
    void initialize() override;
signals:
    void surfacePresented(QWaylandSurface *surface);
protected:
    void zwp_relative_pointer_manager_v1_get_relative_pointer(Resource *resource, uint32_t id, struct ::wl_resource *pointer) override;
};
