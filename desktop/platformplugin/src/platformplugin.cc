#include "platformplugin.h"

#include <qpa/qplatformintegrationplugin.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>

QPlatformIntegration* AIPlatformPlugin::create(const QString &key, const QStringList &params)
{
    using namespace QtWaylandClient;
    Q_UNUSED(key);
    Q_UNUSED(params);

    QtWaylandClient::QWaylandIntegration* integration = new QtWaylandClient::QWaylandIntegration();
    return integration;
}

