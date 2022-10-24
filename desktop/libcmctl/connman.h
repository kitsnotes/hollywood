// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "libcmctl_global.h"

namespace HWCM
{
class Service;
class DeviceModel;
class WifiModel;
class WifiMenu;
class ConnmanPrivate;
class ConfigDialog;
class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit LIBCMCTL_EXPORT ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();
    void setResponsive(bool responsive);
    bool responsive();
    DeviceModel* model() const;
    WifiModel* wifiModel() const;
    WifiMenu* wifiMenu() const;
    Service* serviceForName(QString svcName) const;
    ConfigDialog* getConfigDialog(Service *svc, QWidget *parent = nullptr);
    QList<Service*> services();
signals:

private:
    friend class Service;
    friend class DeviceModel;
    friend class WifiMenu;
    ConnmanPrivate *m_d = nullptr;
};
}

#endif // CONNECTIONMANAGER_H
