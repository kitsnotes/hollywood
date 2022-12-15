#ifndef NOTIFICATIONDAEMON_H
#define NOTIFICATIONDAEMON_H

#include <QApplication>
#include <QObject>
#include "notification.h"

class NotificationDaemon : public QApplication
{
    Q_OBJECT
public:
    NotificationDaemon(int &argc, char **argv);
    static NotificationDaemon* instance() { return static_cast<NotificationDaemon*>(QApplication::instance()); }
public slots:
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString& vendor, QString& version, QString& spec_version);
    uint Notify(const QString& app_name,
                uint replaces_id,
                const QString& app_icon,
                const QString& summary,
                const QString& body,
                const QStringList& actions,
                const QVariantMap& hints,
                int expire_timeout,
                bool noSave = false);
signals:
    void ActionInvoked(uint in0, const QString& in1);
    void NotificationClosed(uint in0, uint in1);
private:
    uint m_id = 0;
    QHash<uint, Notification*> m_notifications;
};

#endif // NOTIFICATIONDAEMON_H
