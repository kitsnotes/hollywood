// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef SERVICE_H
#define SERVICE_H

#include "libcmctl_global.h"

namespace HWCM
{
class ConnectionManager;
class ServicePrivate;
class Service : public QObject
{
    Q_OBJECT
private:
    friend class ConnmanPrivate;
    explicit Service(ConnectionManager *parent, arrayElement &data);
    ~Service();
public:
    enum IP4Config
    {
        V4DHCP,
        V4Manual,
        V4Disabled
    };
    enum IP6Config
    {
        V6Automatic,
        V6Manual,
        V6Disabled
    };
    enum IP6PrivacyConfig
    {
        PrivacyDisabled,
        PrivacyEnabled,
        PrivacyPrefered     // spelling error in connman API
    };

    enum ProxyConfig
    {
        NoProxy,
        AutomaticProxy,
        ManualProxy
    };
    bool online();
    QString nickName() const;
    QVariant mapValue(const QString &key) const;
    QVariant ipv4MapValue(const QString &key) const;
    QVariant ipv6MapValue(const QString &key) const;
    QVariant ipv4ConfigMapValue(const QString &key) const;
    QVariant ipv6ConfigMapValue(const QString &key) const;
    QVariant proxyMapValue(const QString &key) const;
    IP4Config ipv4Config() const;
    IP6Config ipv6Config() const;
    IP6PrivacyConfig ipv6PrivacyConfig() const;
    ProxyConfig proxyConfig() const;
    QString interface() const;
    QString type() const;
    void requestConnection();
Q_SIGNALS:
    void onlineChanged(bool);
    void servicePropertiesUpdated();
    void dbusError(QDBusMessage&);
private:
    ServicePrivate *m_d;
};

typedef QList<HWCM::Service> ServiceList;
}

#endif // SERVICE_H
