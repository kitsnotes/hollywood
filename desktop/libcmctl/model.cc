// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#include "model.h"
#include "connman.h"
#include "private/connman_p.h"
#include "service.h"
#include "technology.h"

#include <QIcon>
#include <QDebug>
#include <QDir>

namespace HWCM
{


WifiModel::WifiModel(ConnmanPrivate *parent)
    : QAbstractListModel(parent)
    , m_parent(parent) {}

Service *WifiModel::serviceForIndex(const QModelIndex &index)
{
    return m_parent->m_wifiNetworks[index.row()];
}

QVariant WifiModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        return m_parent->m_wifiNetworks[index.row()]->nickName();
    }

    return QVariant();
}

int WifiModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_parent->m_wifiNetworks.count();
}

DeviceModel::DeviceModel(ConnmanPrivate *parent)
    : QAbstractListModel(parent)
    , m_parent(parent) {}

void DeviceModel::rescanSys()
{
    QStringList currentDevices;
    QDir dir("/sys/class/net/");
    auto devices = dir.entryInfoList();
    for(auto &fi : devices)
    {
        auto dev = fi.fileName();
        if(dev == ".." || dev == ".")
            continue;

        // ignore bridges, vnets, localhost, etc
        if(fi.symLinkTarget().contains("virtual"))
            continue;

        currentDevices << dev;
        if(hasDevice(dev))
            continue;

        auto di = new DeviceModelItem;
        di->devName = dev;
        if(dev.startsWith("wlan"))
            di->type = DeviceModelItem::WiFi;

        for(auto i : m_parent->m_services)
        {
            if(dev.startsWith("eth") || dev.startsWith("en"))
            {
                if(i->interface() == dev)
                    di->service = i;
            }

            if(dev.startsWith("wifi") || dev.startsWith("wl"))
            {
                auto state = i->mapValue("State").toString();
                if(state != QLatin1String("idle"))
                {
                    if(i->interface() == dev)
                        di->service = i;
                }
            }
        }
        if(di->service != nullptr)
        {
            if(di->service->type() == "wifi")
                di->type = DeviceModelItem::WiFi;
            if(di->service->type() == "ethernet")
                di->type = DeviceModelItem::Ethernet;
            if(di->service->type() == "bluetooth")
                di->type = DeviceModelItem::Bluetooth;

        }
        m_devices.append(di);
    }

    // TODO: add vpn/bluetooth
}

bool DeviceModel::hasDevice(const QString &dev)
{
    for(auto i : m_devices)
    {
        if(i->devName == dev)
            return true;
    }
    return false;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if(m_parent == nullptr)
        return QVariant();

    auto row = index.row();

    switch(role)
    {
    case Qt::DisplayRole:
        return m_devices.at(row)->devName;
    case Qt::DecorationRole:
        return m_devices.at(row)->icon();
    }
    return QVariant();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    if(m_parent == nullptr)
        return 0;
    Q_UNUSED(parent);
    return m_devices.count();
}

Service *DeviceModel::serviceForIndex(const QModelIndex &index)
{
    if(!index.isValid())
        return nullptr;

    int row = index.row();
    if(row > m_devices.count()-1)
        return nullptr;

    return m_devices[row]->service;
}

QString DeviceModel::devNameForIndex(const QModelIndex &index)
{
    if(!index.isValid())
        return QString();

    int row = index.row();
    if(row > m_devices.count()-1)
        return QString();

    return m_devices[row]->devName;
}

bool DeviceModel::isIndexWifi(const QModelIndex &index)
{
    if(!index.isValid())
        return false;

    int row = index.row();
    if(row > m_devices.count()-1)
        return false;

    if(m_devices[row]->type == DeviceModelItem::WiFi)
        return true;

    return false;
}

QIcon DeviceModelItem::icon() const
{
    if(type == Bluetooth)
        return QIcon::fromTheme("network-bluetooth");

    if(type == WiFi)
        return QIcon::fromTheme("network-wireless");

    return QIcon::fromTheme("network-wired");
}



} // namespace HWCM
