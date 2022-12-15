#include "daemon.h"
#include "notifywindow.h"
#include "notification.h"
#include "adaptor.h"
#include <QDBusConnection>

#include <hollywood/hollywood.h>

NotificationDaemon::NotificationDaemon(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setApplicationName("Notification Daemon");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    new NotificationsAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService(QStringLiteral("org.freedesktop.Notifications")))
    {
        qDebug() << "registerService failed: another service with 'org.freedesktop.Notifications' is already running";
        exit(-1);
    }
    else
    {
        if (!connection.registerObject(QStringLiteral("/org/freedesktop/Notifications"), this))
        {
            qDebug() << "registerObject failed: another object with '/org/freedesktop/Notifications' is already running";
            connection.unregisterService("org.freedesktop.Notifications");
        }
    }
}

void NotificationDaemon::CloseNotification(uint id)
{

}

QStringList NotificationDaemon::GetCapabilities()
{
    return QStringList{"actions",
            "body",
            "body-hyperlinks",
            "body-images",
            "body-markup",
            "persistence"};
}

QString NotificationDaemon::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    spec_version = "1.2";
    vendor = "Originull";
    version = "1.0";
    return "Hollywood";
}

uint NotificationDaemon::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout, bool noSave)
{
    m_id++;
    uint id = m_id;
    if(replaces_id != 0)
        id = replaces_id;

    qDebug() << app_name << app_icon << summary << body;
    auto n = new Notification(id, app_name, app_icon, summary, body,
                              actions, hints, expire_timeout, noSave);
    m_notifications.insert(id, n);
    n->show();
}


int main(int argc, char *argv[])
{

    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
    NotificationDaemon a(argc, argv);
    return a.exec();
}
