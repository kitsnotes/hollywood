// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// SPDX-License-Identifier: MIT

#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QObject>

#include "service.h"
#include "technology.h"

namespace HWCM
{

class Technology;
class DeviceModelItem
{
public:
    enum DeviceType {
        Ethernet,
        WiFi,
        Bluetooth
    };
    QString devName;
    DeviceType type = Ethernet;
    Service *service = nullptr;
    QIcon icon() const;
};

class ConnmanPrivate;
class WifiModel : public QAbstractListModel
{
    Q_OBJECT
public:
    Service* serviceForIndex(const QModelIndex &index);
protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
    friend class WifiMenu;
    friend class WifiMenuPrivate;
    friend class ConnmanPrivate;
    WifiModel(ConnmanPrivate *parent);
    ConnmanPrivate *m_parent = nullptr;
};

class Service;
class ConnmanPrivate;
class ConnectionManager;
class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    Service* serviceForIndex(const QModelIndex &index);
    QString devNameForIndex(const QModelIndex &index);
    bool isIndexWifi(const QModelIndex &index);
public slots:
    void rescanSys();
protected:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:    
    friend class ConnmanPrivate;
    DeviceModel(ConnmanPrivate *parent);
    bool hasDevice(const QString &dev);
    ConnmanPrivate *m_parent = nullptr;
    QList<DeviceModelItem*> m_devices;
};
} // namespace HWCM
#endif // MODEL_H
