#ifndef ACPRIVATEPROTOCOLEXTENSION_H
#define ACPRIVATEPROTOCOLEXTENSION_H

#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandQuickExtension>
#include <QWaylandCompositor>

#include "libcompositor_global.h"
#include "qwayland-server-arion-private-ui.h"

class LIBCOMPOSITOR_EXPORT ACPrivateProtocolExtension  : public QWaylandCompositorExtensionTemplate<ACPrivateProtocolExtension>
        , public QtWaylandServer::arion_private_protocol
{
    Q_OBJECT
public:
    ACPrivateProtocolExtension(QWaylandCompositor *compositor = nullptr);
    void initialize() override;
signals:
    void menuServerSet(QWaylandSurface *surface);
public slots:
    void setTopWindowForMenuServer(QWaylandSurface *surface, const QString &serviceName, const QString &objectPath);
    //void setMenuServerSurface(QWaylandSurface *surface);
protected:
    void arion_private_protocol_provision_menu_server(Resource *resource, wl_resource *surface) override;
};

#endif // ACPRIVATEPROTOCOLEXTENSION_H
