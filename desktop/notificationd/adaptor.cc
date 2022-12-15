#include "adaptor.h"
/*
 * Implementation of adaptor class NotificationsAdaptor
 */

NotificationsAdaptor::NotificationsAdaptor(NotificationDaemon *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

NotificationsAdaptor::~NotificationsAdaptor()
{
    // destructor
}

void NotificationsAdaptor::CloseNotification(uint id)
{
    // handle method call org.freedesktop.Notifications.CloseNotification
    parent()->CloseNotification(id);
}

QStringList NotificationsAdaptor::GetCapabilities()
{
    // handle method call org.freedesktop.Notifications.GetCapabilities
    return parent()->GetCapabilities();
}

QString NotificationsAdaptor::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    // handle method call org.freedesktop.Notifications.GetServerInformation
    return parent()->GetServerInformation(vendor, version, spec_version);
}

uint NotificationsAdaptor::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int timeout)
{
    // handle method call org.freedesktop.Notifications.Notify
    return parent()->Notify(app_name, replaces_id, app_icon, summary, body, actions, hints, timeout);
}

