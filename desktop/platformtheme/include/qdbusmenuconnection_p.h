#ifndef QDBUSMENUCONNECTION_P_H
#define QDBUSMENUCONNECTION_P_H
#include <QtCore/QString>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusVariant>

#include <QtGui/qtgui-config.h>

QT_BEGIN_NAMESPACE

class QDBusServiceWatcher;
#ifndef QT_NO_SYSTEMTRAYICON
class QDBusTrayIcon;
#endif // QT_NO_SYSTEMTRAYICON

class QDBusMenuConnection : public QObject
{
    Q_OBJECT

public:
    QDBusMenuConnection(QObject *parent = 0, const QString &serviceName = QString());
    QDBusConnection connection() const { return m_connection; }
    QDBusServiceWatcher *dbusWatcher() const { return m_dbusWatcher; }
    bool isStatusNotifierHostRegistered() const { return m_statusNotifierHostRegistered; }
#ifndef QT_NO_SYSTEMTRAYICON
    bool registerTrayIconMenu(QDBusTrayIcon *item);
    void unregisterTrayIconMenu(QDBusTrayIcon *item);
    bool registerTrayIcon(QDBusTrayIcon *item);
    bool registerTrayIconWithWatcher(QDBusTrayIcon *item);
    bool unregisterTrayIcon(QDBusTrayIcon *item);
#endif // QT_NO_SYSTEMTRAYICON

Q_SIGNALS:
#ifndef QT_NO_SYSTEMTRAYICON
    void trayIconRegistered();
#endif // QT_NO_SYSTEMTRAYICON

private Q_SLOTS:
    void dbusError(const QDBusError &error);

private:
    QDBusConnection m_connection;
    QDBusServiceWatcher *m_dbusWatcher;
    bool m_statusNotifierHostRegistered;
};

QT_END_NAMESPACE



#endif // QDBUSMENUCONNECTION_P_H
