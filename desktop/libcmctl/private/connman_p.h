// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef CONNMAN_P_H
#define CONNMAN_P_H

#include <QObject>
#include <QDBusVariant>
#include <QDBusMessage>
#include <QDBusInterface>

#include "libcmctl_global.h"

namespace HWCM
{
class DeviceModel;
class WifiModel;
class WifiMenu;
class Technology;
class Service;
class ConnmanAgent;
class ConnectionManager;
class ConnmanPrivate : public QObject
{
    Q_OBJECT
public:
    static bool extractMapData(QMap<QString,QVariant>&,const QVariant&);
private:
    friend class ConnectionManager;
    friend class ServicePrivate;
    friend class DeviceModel;
    friend class WifiModel;
    ConnmanPrivate(ConnectionManager* p);
    ~ConnmanPrivate();
    void logErrors(const quint16&);
    bool getProperties();
    bool getTechnologies();
    bool getServices();
    bool getArray(QList<arrayElement>&, const QDBusMessage&);
    bool getMap(QMap<QString,QVariant>&, const QDBusMessage&);
    QDBusMessage::MessageType processReply(const QDBusMessage& reply);
    void queryVersion();
private slots:
    void dbusPropertyChanged(QString,QDBusVariant);
    void dbusServicesChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage);
    void dbusPeersChanged(QList<QVariant>, QList<QDBusObjectPath>, QDBusMessage);
    //void dbusVPNPropertyChanged(QString, QDBusVariant, QDBusMessage);
    void dbusTechnologyAdded(QDBusObjectPath, QVariantMap);
    void dbusTechnologyRemoved(QDBusObjectPath);
private:
    friend class WifiMenuPrivate;
    ConnectionManager *m_q = nullptr;
    ConnmanAgent *m_agent = nullptr;
    DeviceModel *m_model = nullptr;
    WifiModel *m_wifiModel = nullptr;
    WifiMenu *m_wifiMenu = nullptr;
    QDBusInterface* con_manager = nullptr;
    QDBusInterface* vpn_manager = nullptr;

    float f_connmanversion;
    quint16 q16_errors;
    QMap<QString,QVariant>  properties_map;
    QList<arrayElement>  services_list;
    QList<Service*> m_services;
    QList<Service*> m_wifiNetworks;
    QList<arrayElement> technologies_list;
    QList<Technology*> m_technologies;
    QList<arrayElement> wifi_list;
    QList<arrayElement> peer_list;
    QList<arrayElement> vpn_list;       // extracted from our services list
    QList<arrayElement> vpnconn_list;   // from vpn manager used for signals and slots
};
}
#endif // CONNMAN_P_H
