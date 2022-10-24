#include "qdbusmenuregistrarproxy_p.h"

QT_BEGIN_NAMESPACE

/*
 * Implementation of interface class QDBusMenuRegistrarInterface
 */

DBusMenuRegistrarInterface::DBusMenuRegistrarInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

DBusMenuRegistrarInterface::~DBusMenuRegistrarInterface()
{
}
