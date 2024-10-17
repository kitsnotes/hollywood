// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "pointerconstraints.h"

#define WP_POINTER_CONSTRAINT_VERSION 1

Q_LOGGING_CATEGORY(hwPointerConstraints, "compositor.pointerconstraints")

PointerConstraintsV1::PointerConstraintsV1(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::zwp_pointer_constraints_v1(compositor->display(), WP_POINTER_CONSTRAINT_VERSION)
{}

void PointerConstraintsV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), WP_POINTER_CONSTRAINT_VERSION);
}

void PointerConstraintsV1::zwp_pointer_constraints_v1_lock_pointer(Resource *resource, uint32_t id, wl_resource *surface, wl_resource *pointer, wl_resource *region, uint32_t lifetime)
{

}

void PointerConstraintsV1::zwp_pointer_constraints_v1_confine_pointer(Resource *resource, uint32_t id, wl_resource *surface, wl_resource *pointer, wl_resource *region, uint32_t lifetime)
{

}

LockedPointerV1::LockedPointerV1(QWaylandSurface *surface, PointerConstraintsV1 *parent)
{

}

void LockedPointerV1::zwp_locked_pointer_v1_destroy(Resource *resource)
{

}

void LockedPointerV1::zwp_locked_pointer_v1_set_cursor_position_hint(Resource *resource, wl_fixed_t surface_x, wl_fixed_t surface_y)
{

}

void LockedPointerV1::zwp_locked_pointer_v1_set_region(Resource *resource, wl_resource *region)
{

}

ConfiedPointerV1::ConfiedPointerV1(QWaylandSurface *surface, PointerConstraintsV1 *parent)
{

}

void ConfiedPointerV1::zwp_confined_pointer_v1_destroy(Resource *resource)
{

}

void ConfiedPointerV1::zwp_confined_pointer_v1_set_region(Resource *resource, wl_resource *region)
{

}
