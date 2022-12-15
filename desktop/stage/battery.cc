#include "battery.h"
#include "upower.h"

BatteryMonitor::BatteryMonitor(QWidget *parent)
    : QToolButton(parent)
    , m_upower(new UPowerInterface(this))
{
    if(!m_upower && !m_upower->isValid())
        qDebug() << "Invalid UPower state";
    else
    {
        for(auto dev : m_upower->enumerateDevices())
        {
           auto device = new UPowerDeviceInterface(dev.path(), this);
           if(device->type() == 2)
           {
               m_battery = device;
               batteryChanged();
               connect(m_battery, &UPowerDeviceInterface::changed,
                       this, &BatteryMonitor::batteryChanged);
               break;
           }
           else
               delete device;
        }
    }
    if(m_battery == nullptr)
        setVisible(false);
}

void BatteryMonitor::batteryChanged()
{
    setText(QString::number(m_battery->percent()));
    setIcon(QIcon::fromTheme(m_battery->displayIcon()));
}
