// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#include "private/technology_p.h"
#include "technology.h"
#include "connman.h"

#include <QDBusConnection>
#include <QDBusMessage>

namespace HWCM
{

TechnologyPrivate::TechnologyPrivate(Technology *parent, arrayElement &data)
    : QObject(parent)
    , m_q(parent)
    , m_dbusData(data)
{
    QDBusConnection::systemBus().
    connect(DBUS_CON_SERVICE,
            m_dbusData.objpath.path(),
            "net.connman.Technology",
            "PropertyChanged",
            this,
            SLOT(dbusTechnologyPropertyChanged(QString, QDBusVariant, QDBusMessage)));
}

TechnologyPrivate::~TechnologyPrivate()
{
    QDBusConnection::systemBus().
    disconnect(DBUS_CON_SERVICE,
               m_dbusData.objpath.path(),
               "net.connman.Technology",
               "PropertyChanged",
               this,
               SLOT(dbusTechnologyPropertyChanged(QString, QDBusVariant, QDBusMessage)));
}

void TechnologyPrivate::dbusTechnologyPropertyChanged(QString name, QDBusVariant dbvalue, QDBusMessage msg)
{
    QString s_path = msg.path();

    // replace the old values with the changed ones.
    QMap<QString,QVariant> map = m_dbusData.objmap;
    map.remove(name);
    map.insert(name, dbvalue.variant() );
    arrayElement ae = {m_dbusData.objpath, map};
    m_dbusData = ae;

    Q_EMIT m_q->technologyPropertiesUpdated();
    return;
}

Technology::Technology(ConnectionManager *parent, arrayElement &data)
    : QObject(parent)
    , m_d(new TechnologyPrivate(this, data))
{}

Technology::~Technology()
{
    delete m_d;
}


} // namespace HWCM
