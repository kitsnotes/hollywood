#include "server/privateprotocol.h"

#include <QWaylandSurface>
#include <QDebug>

ACPrivateProtocolExtension::ACPrivateProtocolExtension(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
}
void ACPrivateProtocolExtension::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void ACPrivateProtocolExtension::setTopWindowForMenuServer(QWaylandSurface* surface, const QString &serviceName, const QString &objectPath)
{
    auto wlSurface = surface->resource();
    qDebug() << "ACPrivateProtocolExtension::setTopWindowForMenuServer serviceName:"
             << serviceName << "ojbectPath:" << objectPath;
    return;
    QtWaylandServer::arion_private_protocol::
            send_appmenu_top_level_window_changed(wlSurface, serviceName, objectPath);
}

void ACPrivateProtocolExtension::arion_private_protocol_provision_menu_server(QtWaylandServer::arion_private_protocol::Resource *resource, wl_resource *wl_surface)
{
    Q_UNUSED(resource);
    auto surface = QWaylandSurface::fromResource(wl_surface);
    qDebug() << "ACPrivateProtocolExtension::arion_private_protocol_provision_menu_server received MenuServer Request" << surface;
    emit menuServerSet(surface);
}
