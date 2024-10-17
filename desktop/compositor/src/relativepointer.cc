#include "relativepointer.h"

#define WP_RELATIVE_POINTER_VERSION 1

Q_LOGGING_CATEGORY(hwRelativePointer, "compositor.relativepointer")

RelativePointerManagerV1::RelativePointerManagerV1(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::zwp_relative_pointer_manager_v1(compositor->display(), WP_RELATIVE_POINTER_VERSION)
{}

void RelativePointerManagerV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), WP_RELATIVE_POINTER_VERSION);
}

void RelativePointerManagerV1::zwp_relative_pointer_manager_v1_get_relative_pointer(Resource *resource, uint32_t id, wl_resource *pointer)
{

}

RelativePointerV1::RelativePointerV1(QWaylandSurface *surface, RelativePointerManagerV1 *parent)
{

}
