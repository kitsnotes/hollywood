// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

#include "libcmctl_global.h"

namespace HWCM
{
class ConnectionManager;
class TechnologyPrivate;
class Technology : public QObject
{
    Q_OBJECT
private:
    friend class ConnmanPrivate;
    explicit Technology(ConnectionManager *parent, arrayElement &data);
    ~Technology();
Q_SIGNALS:
    void technologyPropertiesUpdated();
private:
    TechnologyPrivate *m_d;
};

typedef QList<HWCM::Technology> TechnologyList;
}
#endif // TECHNOLOGY_H
