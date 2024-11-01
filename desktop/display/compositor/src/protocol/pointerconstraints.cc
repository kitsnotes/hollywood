// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "pointerconstraints.h"

#define WP_POINTER_CONSTRAINT_VERSION 1

Q_LOGGING_CATEGORY(hwPointerConstraints, "compositor.pointerconstraints")

PointerConstraintsV1::PointerConstraintsV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::zwp_pointer_constraints_v1(compositor->display(), WP_POINTER_CONSTRAINT_VERSION)
{
    qCInfo(hwPointerConstraints, "Supporting wp_pointer_constraints_v1 (protocol version %i)", WP_POINTER_CONSTRAINT_VERSION);
}

void PointerConstraintsV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), WP_POINTER_CONSTRAINT_VERSION);
}

void PointerConstraintsV1::zwp_pointer_constraints_v1_lock_pointer(Resource *, uint32_t , wl_resource *, wl_resource *, wl_resource *, uint32_t )
{

}

void PointerConstraintsV1::zwp_pointer_constraints_v1_confine_pointer(Resource *, uint32_t , wl_resource *, wl_resource *, wl_resource *, uint32_t )
{

}

LockedPointerV1::LockedPointerV1(QWaylandSurface *, PointerConstraintsV1 *)
{

}

void LockedPointerV1::zwp_locked_pointer_v1_destroy(Resource *)
{

}

void LockedPointerV1::zwp_locked_pointer_v1_set_cursor_position_hint(Resource *, wl_fixed_t , wl_fixed_t )
{

}

void LockedPointerV1::zwp_locked_pointer_v1_set_region(Resource *, wl_resource *)
{

}

ConfiedPointerV1::ConfiedPointerV1(QWaylandSurface *, PointerConstraintsV1 *)
{

}

void ConfiedPointerV1::zwp_confined_pointer_v1_destroy(Resource *)
{

}

void ConfiedPointerV1::zwp_confined_pointer_v1_set_region(Resource *, wl_resource *)
{

}
