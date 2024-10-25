// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "relativepointer.h"
#include "compositor.h"
#include "surfaceobject.h"
#include <QWaylandPointer>
#include <chrono>

#define WP_RELATIVE_POINTER_VERSION 1

Q_LOGGING_CATEGORY(hwRelativePointer, "compositor.relativepointer")

RelativePointerManagerV1::RelativePointerManagerV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::zwp_relative_pointer_manager_v1(compositor->display(), WP_RELATIVE_POINTER_VERSION)
{
    qCInfo(hwRelativePointer, "Supporting wp_relative_pointer_v1 (protocol version %i)", WP_RELATIVE_POINTER_VERSION);
}

void RelativePointerManagerV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), WP_RELATIVE_POINTER_VERSION);
}

QList<RelativePointerV1*> RelativePointerManagerV1::relativePointerForWaylandPointer(QWaylandPointer *pointer)
{
    QList<RelativePointerV1*> pointers;

    for(auto p : m_relativePointers)
    {
        if(p->m_pointer == pointer)
            pointers.append(p);
    }

    return pointers;
}

void RelativePointerManagerV1::removePointer(RelativePointerV1 *pointer)
{
    if(pointer != nullptr)
        m_relativePointers.removeOne(pointer);
}

void RelativePointerManagerV1::zwp_relative_pointer_manager_v1_get_relative_pointer(Resource *resource, uint32_t id, wl_resource *pointer)
{
    // TODO: handle multiple seats/transient pointers
    Q_UNUSED(pointer);
    auto qpointer = hwComp->defaultSeat()->pointer();

    wl_resource *pointer_resource = wl_resource_create(resource->client(),
                        &zwp_relative_pointer_v1_interface, resource->version(), id);

    if (!pointer_resource)
    {
        wl_client_post_no_memory(resource->client());
        return;
    }

    auto rp = new RelativePointerV1(qpointer, pointer_resource, this);
    m_relativePointers.append(rp);
}

RelativePointerV1::RelativePointerV1(QWaylandPointer *pointer, wl_resource *resource, RelativePointerManagerV1 *parent)
    : QObject(nullptr)
    , QtWaylandServer::zwp_relative_pointer_v1(resource)
    , m_parent(parent)
    , m_pointer(pointer)
{
    qCInfo(hwRelativePointer, "wp_relative_pointer_v1 created for main pointer");
}

RelativePointerV1::~RelativePointerV1()
{
    qCInfo(hwRelativePointer, "wp_relative_pointer_v1 destroyed for main pointer");
}

void RelativePointerV1::sendRelativeMotion(int dx, int dy, int dx_unaccel, int dy_unaccel, quint64 ts)
{
    send_relative_motion(resource()->handle, ts >> 32, ts & 0xffffffff,
                               dx, dy, dx_unaccel, dy_unaccel);
}

void RelativePointerV1::zwp_relative_pointer_v1_destroy(Resource *resource)
{
    // client has requested destruction
    wl_resource_destroy(resource->handle);
    m_parent->removePointer(this);
    deleteLater();
}
