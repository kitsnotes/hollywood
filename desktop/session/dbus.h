#ifndef DBUS_H
#define DBUS_H

#include <QtDBus/QDBusAbstractAdaptor>
#include "session.h"

#include <hollywood/hollywood.h>

class SessionDBus : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", HOLLYWOOD_SESSION_DBUSPATH)

public:
    SessionDBus(SMApplication *parent)
        : QDBusAbstractAdaptor(parent)
        , m_parent(parent) {}
public slots:
    bool canLogout()
    {
        return true;
    }

    bool canReboot()
    {
        return m_parent->canReboot();
    }

    bool canPowerOff()
    {
        return m_parent->canShutdown();
    }

    bool startedByDisplayManager()
    {
        return m_parent->displayManagerStart();
    }

    Q_NOREPLY void logout()
    {
        m_parent->logout();
    }

    Q_NOREPLY void reboot()
    {
        m_parent->reboot();
    }

    Q_NOREPLY void powerOff()
    {
        m_parent->shutdown();
    }

private:
    SMApplication *m_parent;
};

#endif // DBUS_H
