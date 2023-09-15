#include "upower.h"
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusConnection>

UPowerInterface::UPowerInterface(QObject *parent)
    : QDBusAbstractInterface("org.freedesktop.UPower",
                             "/org/freedesktop/UPower",
                             "org.freedesktop.UPower",
                             QDBusConnection::systemBus(),
                             parent)
{}

QList<QDBusObjectPath> UPowerInterface::enumerateDevices()
{
    QDBusReply<QList<QDBusObjectPath>> reply = call("EnumerateDevices");
    return reply.value();
}

UPowerDeviceInterface::UPowerDeviceInterface(const QString &path, QObject *parent)
    : QDBusAbstractInterface("org.freedesktop.UPower",
                             path,
                             "org.freedesktop.UPower.Device",
                             QDBusConnection::systemBus(),
                             parent)
    , m_path(path)
{
    m_interface = new QDBusInterface("org.freedesktop.UPower", path,
                                     "org.freedesktop.DBus.Properties",
                                     QDBusConnection::systemBus(), this);
    updateProperties();
    if(!m_interface->connection().connect("org.freedesktop.UPower", path,
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged", this,
                                          SLOT(updateProperties())))
        qDebug() << "Error connecting to DBus.Properties" << connection().lastError().message();
}

UPowerDeviceInterface::~UPowerDeviceInterface()
{
    if(!m_interface->connection().disconnect("org.freedesktop.UPower", m_path,
                                            "org.freedesktop.DBus.Properties",
                                            "PropertiesChanged", this,
                                            SLOT(updateProperties())))
        qDebug() << "Error disconnecting DBus.Properties" << connection().lastError().message();

}

quint16 UPowerDeviceInterface::type()
{
    return m_props.value("Type", 0).toUInt();
}

quint16 UPowerDeviceInterface::state()
{
    return m_props.value("State", 0).toUInt();
}

QString UPowerDeviceInterface::stateString()
{
    switch(state())
    {
    case 1:
        return tr("Charging");
    case 2:
        return tr("Discharging");
    case 3:
        return tr("Empty");
    case 4:
        return tr("Fully charged");
    case 5:
        return tr("Pending charge");
    case 6:
        return tr("Pending discharge");
    case 0:
    default:
        return tr("Unknown");
    }
}

double UPowerDeviceInterface::percent()
{
    return m_props.value("Percentage", 0.0).toDouble();
}

qint64 UPowerDeviceInterface::timeUntilFull()
{
    return m_props.value("TimeToFull", 0).toLongLong();
}

quint64 UPowerDeviceInterface::timeUntilEmpty()
{
    return m_props.value("TimeToEmpty", 0).toULongLong();
}

QString UPowerDeviceInterface::displayIcon()
{
    return m_props.value("IconName").toString();
}

double UPowerDeviceInterface::energyFull()
{
    return m_props.value("EnergyFull", 0.0).toDouble();
}

double UPowerDeviceInterface::energyFullDesign()
{
    return m_props.value("EnergyFullDesign", 0.0).toDouble();
}

uint UPowerDeviceInterface::batteryHealthPercent()
{
    return (uint)energyFull()*100/energyFullDesign();
}

void UPowerDeviceInterface::updateProperties()
{
    if(m_interface && m_interface->isValid())
    {
        QDBusReply<QVariantMap> map = m_interface->call("GetAll", "org.freedesktop.UPower.Device");
        m_props = map;
        emit changed();
    }
}
