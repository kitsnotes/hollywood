// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#include "private/connman_p.h"
#include "private/service_p.h"
#include "service.h"
#include "connman.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

namespace HWCM
{

ServicePrivate::ServicePrivate(Service *parent, arrayElement &data)
    : QObject(parent)
    , m_q(parent)
    , m_dbusData(data)
{
    QDBusConnection::systemBus().
    connect(DBUS_CON_SERVICE,
            m_dbusData.objpath.path(),
            "net.connman.Service",
            "PropertyChanged",
            this,
            SLOT(dbusServicePropertyChanged(QString, QDBusVariant, QDBusMessage)));
}

ServicePrivate::~ServicePrivate()
{
    QDBusConnection::systemBus().
    disconnect(DBUS_CON_SERVICE,
               m_dbusData.objpath.path(),
               "net.connman.Service",
               "PropertyChanged",
               this,
               SLOT(dbsServicePropertyChanged(QString, QDBusVariant, QDBusMessage)));
}

void ServicePrivate::dbusServicePropertyChanged(QString property, QDBusVariant dbvalue, QDBusMessage msg)
{
    QString s_path = msg.path();
    QVariant value = dbvalue.variant();
    QString s_state;

    // replace the old values with the changed ones.
    QMap<QString,QVariant> map = m_dbusData.objmap;
    map.remove(property);
    map.insert(property, value );
    arrayElement ae = {m_dbusData.objpath, map};
    s_state = map.value("State").toString();
    m_dbusData = ae;

    // process errrors   - errors only valid when service is in the failure state
    // TODO: fix notifications with a signal
    if (property =="Error" && s_state == "failure") {
       /* notifyclient->init();
       notifyclient->setSummary(QString(tr("Service Error: %1")).arg(value.toString()) );
       notifyclient->setBody(QString(tr("Object Path: %1")).arg(s_path) );
       notifyclient->setIcon(iconman->getIconName("state_error") );
       notifyclient->setUrgency(Nc::UrgencyCritical);
       this->sendNotifications(); */
    }

    // if state property changed sync the online data members.
    if (property == "State")
    {
        bool old_online = m_online;
       if (value.toString() == "online")
          m_online = true;
       else
           m_online = false;

       if(m_online != old_online)
           Q_EMIT m_q->onlineChanged(m_online);
    }

    Q_EMIT m_q->servicePropertiesUpdated();
    return;
}

Service::Service(ConnectionManager *parent, arrayElement &data)
    : QObject(parent)
    , m_d(new ServicePrivate(this, data))
{}

Service::~Service()
{
    delete m_d;
}

bool Service::online()
{
    return m_d->m_online;
}

QString Service::nickName() const
{
    QMap<QString,QVariant> submap;

    if (m_d->m_dbusData.objmap.value("Type").toString() == "ethernet")
    {
        ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("Ethernet") );
        if (submap.value("Interface").toString().isEmpty() )
            return m_d->m_dbusData.objmap.value("Name").toString();
        else
            return QString(m_d->m_dbusData.objmap.value("Name").toString() + " [%1]").arg(submap.value("Interface").toString());
    } // if type ethernet
    else
    {
     if ( m_d->m_dbusData.objmap.value("Type").toString() == "wifi"
          && m_d->m_dbusData.objmap.value("Name").toString().isEmpty())
        return tr("[Hidden Wifi]");
     else
        return m_d->m_dbusData.objmap.value("Name").toString();
    }

    // fallback
    return QString();
}

QVariant Service::mapValue(const QString &key) const
{
    return m_d->m_dbusData.objmap.value(key);
}

QString Service::interface() const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("Ethernet"));

    if(!d)
        return QString("");

    return submap.value("Interface").toString();
}

QString Service::type() const
{
    return m_d->m_dbusData.objmap.value("Type").toString();
}

void Service::requestConnection()
{
    qDebug() << "service connection requested";
    QDBusInterface* iface_serv = NULL;

    iface_serv = new QDBusInterface(DBUS_CON_SERVICE, m_d->m_dbusData.objpath.path(),
                                    "net.connman.Service", QDBusConnection::systemBus(), this);
    iface_serv->setTimeout(5); // need a short timeout to get the Agent
    QDBusMessage reply = iface_serv->call(QDBus::AutoDetect, "Connect");
    if (reply.errorName() != "org.freedesktop.DBus.Error.NoReply")
        Q_EMIT dbusError(reply);

    iface_serv->deleteLater();
}

Service::IP4Config Service::ipv4Config() const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv4.Configuration"));

    if(!d)
        return V4Disabled;

    if(submap.value("Method").toString() == "dhcp")
        return V4DHCP;

    if(submap.value("Method").toString() == "manual")
        return V4Manual;

    return V4Disabled;
}

Service::IP6Config Service::ipv6Config() const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv6.Configuration"));

    if(!d)
        return V6Disabled;

    if(submap.value("Method").toString() == "auto")
        return V6Automatic;

    if(submap.value("Method").toString() == "manual")
        return V6Manual;

    return V6Disabled;
}

Service::IP6PrivacyConfig Service::ipv6PrivacyConfig() const
{
    auto val = ipv6ConfigMapValue("Privacy").toString();
    if(val.isEmpty())
        return PrivacyDisabled;

    if(val == "enabled")
        return PrivacyEnabled;

    if(val == "prefered")   // spelling error in connman API do NOT correct this
        return PrivacyPrefered;

    return PrivacyDisabled;
}

Service::ProxyConfig Service::proxyConfig() const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("Proxy.Configuration"));

    if(!d)
        return NoProxy;

    if(submap.value("Method").toString() == "auto")
        return AutomaticProxy;

    if(submap.value("Method").toString() == "manual")
        return ManualProxy;

    return NoProxy;
}

QVariant Service::ipv4MapValue(const QString &key) const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv4"));

    qDebug() << submap;
    if(!d)
        return QVariant();

    return submap.value(key);
}

QVariant Service::ipv6MapValue(const QString &key) const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv6"));

    qDebug() << submap;
    if(!d)
        return QVariant();

    return submap.value(key);
}

QVariant Service::ipv4ConfigMapValue(const QString &key) const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv4.Configuration"));

    qDebug() << submap;
    if(!d)
        return QVariant();

    return submap.value(key);
}

QVariant Service::ipv6ConfigMapValue(const QString &key) const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("IPv6.Configuration"));

    qDebug() << submap;
    if(!d)
        return QVariant();

    return submap.value(key);
}

QVariant Service::proxyMapValue(const QString &key) const
{
    QMap<QString,QVariant> submap;
    auto d = ConnmanPrivate::extractMapData(submap, m_d->m_dbusData.objmap.value("Proxy.Configuration"));

    qDebug() << submap;
    if(!d)
        return QVariant();

    return submap.value(key);
}

} // namespace HWCM
