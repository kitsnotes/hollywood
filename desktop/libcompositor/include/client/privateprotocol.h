#ifndef ACPRIVATEPROTOCOL_H
#define ACPRIVATEPROTOCOL_H

#include <QGuiApplication>
#include <QtGui/QWindow>

#include <QtWaylandClient/QWaylandClientExtensionTemplate>
#include <wayland-client-protocol.h>

#include "libcompositor_global.h"
#include "qwayland-originull-privateapi.h"

class QWaylandDisplay;
class QWaylandIntegration;

class OriginullMenuServerClient;
class LIBCOMPOSITOR_EXPORT AIPrivateWaylandProtocol
        : public QWaylandClientExtensionTemplate<AIPrivateWaylandProtocol>
        , public QtWayland::org_originull_privateapi
{
    Q_OBJECT
public:
    AIPrivateWaylandProtocol();
    Q_INVOKABLE OriginullMenuServerClient* createMenuServerResponder();
    Q_INVOKABLE void rotateWallpaper();

private:
    void windowForSurface();
};

class LIBCOMPOSITOR_EXPORT OriginullMenuServerClient
        : public QWaylandClientExtensionTemplate<OriginullMenuServerClient>
        , public QtWayland::org_originull_menuserver
{
    Q_OBJECT
public:
    OriginullMenuServerClient(struct ::org_originull_menuserver *menu);
protected:
    void org_originull_menuserver_appmenu_top_level_window_changed(const QString &service_name, const QString &object_path) override;
signals:
    void menuChanged(const QString &serviceName, const QString &objectPath);
private:
    void windowForSurface();
};

#endif // ACPRIVATEPROTOCOL_H
