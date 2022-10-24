// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef SERVICE_P_H
#define SERVICE_P_H

#include "libcmctl_global.h"
#include <QDBusMessage>
#include <QDBusVariant>

namespace HWCM
{
class Service;
class ServicePrivate : public QObject
{
    Q_OBJECT
private:
    friend class Service;
    ServicePrivate(Service *parent, arrayElement &data);
    ~ServicePrivate();
private Q_SLOTS:
    void dbusServicePropertyChanged(QString, QDBusVariant, QDBusMessage);
private:
    Service *m_q;
    arrayElement m_dbusData;
    arrayElement m_ipv4config;
    bool m_online;
};
}

#endif // SERVICE_P_H
