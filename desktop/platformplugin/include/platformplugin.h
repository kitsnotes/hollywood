#ifndef ARIONPLATFORMTHEME_H
#define ARIONPLATFORMTHEME_H

#include <QObject>
#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformintegrationplugin.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include "wayland-client-core.h"

class AIPlatformPlugin : public QPlatformIntegrationPlugin {
    Q_OBJECT
#if QT_VERSION < 0x060000
    Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid FILE "platformplugin.json")
#else
    Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid FILE "platformplugin.json")
#endif
public:
    QPlatformIntegration *create(const QString&, const QStringList&) override;
};

#endif // ARIONPLATFORMTHEME_H
