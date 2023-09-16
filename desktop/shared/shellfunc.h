#ifndef SHELLFUNC_H
#define SHELLFUNC_H
#include <hollywood/hollywood.h>
#include <QDBusInterface>

namespace hw_shell {
    bool openHelpTopic(const QString &topic)
    {
        QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);
        if(!ldb.isValid())
        {
            qWarning() << QString("Could not call session dbus interface. Command: openHelpTopic");
            return false;
        }

        QDBusMessage msg = ldb.call("openHelpTopic", topic);

       if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
            return true;

        auto response = msg.arguments().constFirst();

        if(msg.arguments().isEmpty())
            return true;

        if(response.isNull())
            return true;

        if(response.toBool())
            return true;

        return false;
    }
}

#endif // SHELLFUNC_H
