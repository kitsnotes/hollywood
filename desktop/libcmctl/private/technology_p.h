
#ifndef TECHNOLOGY_P_H
#define TECHNOLOGY_P_H

#include "libcmctl_global.h"
#include <QDBusMessage>
#include <QDBusVariant>

namespace HWCM
{
class Technology;
class TechnologyPrivate : public QObject
{
    Q_OBJECT
private:
    friend class Technology;
    TechnologyPrivate(Technology *parent, arrayElement &data);
    ~TechnologyPrivate();
private Q_SLOTS:
    void dbusTechnologyPropertyChanged(QString, QDBusVariant, QDBusMessage);
private:
    Technology *m_q;
    arrayElement m_dbusData;
    bool m_online;
};
}

#endif // TECHNOLOGY_P_H
