#include <QtGui/qtgui-config.h>

#ifndef QT_NO_SYSTEMTRAYICON
#include "qdbustrayicon_p.h"
#endif
#include "qdbusmenuconnection_p.h"
#include "qdbusmenuadaptor_p.h"
#include "qdbusplatformmenu_p.h"

#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusConnectionInterface>
#include <qdebug.h>
#include <qcoreapplication.h>

QT_BEGIN_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(qLcMenu)

/*const QString StatusNotifierWatcherService = QLatin1String("org.kde.StatusNotifierWatcher");
const QString StatusNotifierWatcherPath = QLatin1String("/StatusNotifierWatcher");
const QString StatusNotifierItemPath = QLatin1String("/StatusNotifierItem");*/
const QString MenuBarPath = QLatin1String("/MenuBar");

/*!
    \class QDBusMenuConnection
    \internal
    A D-Bus connection which is used for both menu and tray icon services.
    Connects to the session bus and registers with the respective watcher services.
*/
QDBusMenuConnection::QDBusMenuConnection(QObject *parent, const QString &serviceName)
    : QObject(parent)
    , m_connection(serviceName.isNull() ? QDBusConnection::sessionBus()
                                        : QDBusConnection::connectToBus(QDBusConnection::SessionBus, serviceName))
    , m_dbusWatcher(new QDBusServiceWatcher(StatusNotifierWatcherService, m_connection, QDBusServiceWatcher::WatchForRegistration, this))
    , m_statusNotifierHostRegistered(false)
{
#ifndef QT_NO_SYSTEMTRAYICON
    QDBusInterface systrayHost(StatusNotifierWatcherService, StatusNotifierWatcherPath, StatusNotifierWatcherService, m_connection);
    if (systrayHost.isValid() && systrayHost.property("IsStatusNotifierHostRegistered").toBool())
        m_statusNotifierHostRegistered = true;
    else
        qCDebug(qLcMenu) << "StatusNotifierHost is not registered";
#endif
}

void QDBusMenuConnection::dbusError(const QDBusError &error)
{
    qWarning() << "QDBusTrayIcon encountered a D-Bus error:" << error;
}

/* #ifndef QT_NO_SYSTEMTRAYICON
bool QDBusMenuConnection::registerTrayIconMenu(QDBusTrayIcon *item)
{
    bool success = connection().registerObject(MenuBarPath, item->menu());
    if (!success)  // success == false is normal, because the object may be already registered
        qCDebug(qLcMenu) << "failed to register" << item->instanceId() << MenuBarPath;
    return success;
}

void QDBusMenuConnection::unregisterTrayIconMenu(QDBusTrayIcon *item)
{
    if (item->menu())
        connection().unregisterObject(MenuBarPath);
}

bool QDBusMenuConnection::registerTrayIcon(QDBusTrayIcon *item)
{
    bool success = connection().registerService(item->instanceId());
    if (!success) {
        qWarning() << "failed to register service" << item->instanceId();
        return false;
    }

    success = connection().registerObject(StatusNotifierItemPath, item);
    if (!success) {
        unregisterTrayIcon(item);
        qWarning() << "failed to register" << item->instanceId() << StatusNotifierItemPath;
        return false;
    }

    if (item->menu())
        registerTrayIconMenu(item);

    return registerTrayIconWithWatcher(item);
}

bool QDBusMenuConnection::registerTrayIconWithWatcher(QDBusTrayIcon *item)
{
    QDBusMessage registerMethod = QDBusMessage::createMethodCall(
                StatusNotifierWatcherService, StatusNotifierWatcherPath, StatusNotifierWatcherService,
                QLatin1String("RegisterStatusNotifierItem"));
    registerMethod.setArguments(QVariantList() << item->instanceId());
    return m_connection.callWithCallback(registerMethod, this, SIGNAL(trayIconRegistered()), SLOT(dbusError(QDBusError)));
}

bool QDBusMenuConnection::unregisterTrayIcon(QDBusTrayIcon *item)
{
    unregisterTrayIconMenu(item);
    connection().unregisterObject(StatusNotifierItemPath);
    bool success = connection().unregisterService(item->instanceId());
    if (!success)
        qWarning() << "failed to unregister service" << item->instanceId();
    return success;
}
#endif // QT_NO_SYSTEMTRAYICON*/

QT_END_NAMESPACE
