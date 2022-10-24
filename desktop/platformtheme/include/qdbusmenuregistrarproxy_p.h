#ifndef QDBUSMENUREGISTRARPROXY_P_H
#define QDBUSMENUREGISTRARPROXY_P_H
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>

QT_BEGIN_NAMESPACE

/*
 * Proxy class for interface com.canonical.AppMenu.Registrar
 */
class DBusMenuRegistrarInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    {
        return "com.canonical.AppMenu.Registrar";
    }

public:
    explicit DBusMenuRegistrarInterface(const QString &service,
                                         const QString &path,
                                         const QDBusConnection &connection,
                                         QObject *parent = nullptr);

    ~DBusMenuRegistrarInterface();

public Q_SLOTS: // METHODS
    QDBusPendingReply<QString, QDBusObjectPath> GetMenuForWindow(uint windowId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(windowId);
        return asyncCallWithArgumentList(QStringLiteral("GetMenuForWindow"), argumentList);
    }
    QDBusReply<QString> GetMenuForWindow(uint windowId, QDBusObjectPath &menuObjectPath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(windowId);
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QStringLiteral("GetMenuForWindow"), argumentList);
        QList<QVariant> arguments = reply.arguments();
        if (reply.type() == QDBusMessage::ReplyMessage && arguments.count() == 2)
            menuObjectPath = qdbus_cast<QDBusObjectPath>(arguments.at(1));
        return reply;
    }

    QDBusPendingReply<> RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(windowId) << QVariant::fromValue(menuObjectPath);
        return asyncCallWithArgumentList(QStringLiteral("RegisterWindow"), argumentList);
    }

    QDBusPendingReply<> UnregisterWindow(uint windowId)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(windowId);
        return asyncCallWithArgumentList(QStringLiteral("UnregisterWindow"), argumentList);
    }
};

QT_END_NAMESPACE
#endif // QDBUSMENUREGISTRARPROXY_P_H
