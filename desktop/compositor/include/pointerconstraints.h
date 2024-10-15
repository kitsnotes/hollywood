// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-pointer-constraints-unstable-v1.h"

class Compositor;
class Surface;
class PointerConstraintsV1;

class LockedPointerV1 : public QWaylandCompositorExtensionTemplate<LockedPointerV1>
    , public QtWaylandServer::zwp_locked_pointer_v1
{
    Q_OBJECT
public:
    LockedPointerV1(QWaylandSurface *surface, PointerConstraintsV1 *parent);
signals:
    void surfacePresented(QWaylandSurface *surface);
protected:
    void zwp_locked_pointer_v1_destroy(Resource *resource) override;
    void zwp_locked_pointer_v1_set_cursor_position_hint(Resource *resource, wl_fixed_t surface_x, wl_fixed_t surface_y) override;
    void zwp_locked_pointer_v1_set_region(Resource *resource, struct ::wl_resource *region) override;
private:
    PointerConstraintsV1 *m_parent = nullptr;
};

class ConfiedPointerV1 : public QWaylandCompositorExtensionTemplate<ConfiedPointerV1>
    , public QtWaylandServer::zwp_confined_pointer_v1
{
    Q_OBJECT
public:
    ConfiedPointerV1(QWaylandSurface *surface, PointerConstraintsV1 *parent);
signals:
    void setConfinedPointerRegion();
protected:
    void zwp_confined_pointer_v1_destroy(Resource *resource) override;
    void zwp_confined_pointer_v1_set_region(Resource *resource, struct ::wl_resource *region) override;
private:
    PointerConstraintsV1 *m_parent = nullptr;
};

class PointerConstraintsV1 : public QWaylandCompositorExtensionTemplate<PointerConstraintsV1>
    , public QtWaylandServer::zwp_pointer_constraints_v1
{
    Q_OBJECT
public:
    PointerConstraintsV1(QWaylandCompositor *compositor);
    void initialize() override;
signals:
    void surfacePresented(QWaylandSurface *surface);
protected:
    void zwp_pointer_constraints_v1_lock_pointer(Resource *resource, uint32_t id, struct ::wl_resource *surface, struct ::wl_resource *pointer, struct ::wl_resource *region, uint32_t lifetime) override;
    void zwp_pointer_constraints_v1_confine_pointer(Resource *resource, uint32_t id, struct ::wl_resource *surface, struct ::wl_resource *pointer, struct ::wl_resource *region, uint32_t lifetime) override;
};
