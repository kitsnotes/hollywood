#include "battery.h"
#include "upower.h"
#include <QMenu>
#include <QAction>

BatteryMonitor::BatteryMonitor(QWidget *parent)
    : QToolButton(parent)
    , m_menu(new QMenu(this))
    , m_upower(new UPowerInterface(this))
{
    QFont myFont = font();
    myFont.setPointSize(myFont.pointSize()+1);
    setFont(myFont);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setAutoRaise(true);
    setPopupMode(QToolButton::InstantPopup);
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
    setArrowType(Qt::NoArrow);

    setIcon(QIcon::fromTheme("battery-missing"));
    m_state = m_menu->addAction(tr("No Battery Available"));
    m_state->setEnabled(false);
    m_time = m_menu->addAction(tr("No timing available"));
    m_time->setVisible(false);
    m_time->setDisabled(true);
    m_menu->addSeparator();
    m_menu->addAction(tr("Energy Settings..."));
    setMenu(m_menu);
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
    //if(m_battery == nullptr)
     //   setVisible(false);
}

void BatteryMonitor::batteryChanged()
{
    auto str = QString(" %1%").arg(QString::number(m_battery->percent(),'G',2));
    setText(str);
    setIcon(QIcon::fromTheme(m_battery->displayIcon()));
    QString state;
    QString time;
    switch(m_battery->state())
    {
    case 1:
        state = tr("Charging: %1");
        time = tr("%1 until fully charged");
        break;
    case 2:
        state = tr("Discharging: %1");
        time = tr("%1 until empty");
        break;
    case 3:
        state = tr("Empty: %1");
        break;
    case 4:
        state = tr("Fully charged: %1");
        break;
    case 5:
        state = tr("Pending charge: %1");
        break;
    case 6:
        state = tr("Pending discharge: %1");
        break;
    default:
        state = tr("Unknown: %1");
        break;
    }
    state = state.arg(QString::number(m_battery->percent(),'G',2) + "%");
    m_state->setText(state);
    if(time.isEmpty())
        m_time->setVisible(false);
    else
    {
        uint minutes = m_battery->timeUntilEmpty();
        if(m_battery->state() == 1)
            minutes = m_battery->timeUntilFull();

        if(minutes == 0)
            return;

        m_time->setVisible(true);
        m_time->setDisabled(true);

        minutes = minutes / 60;

        QString timestr;
        if(minutes < 59)
            timestr = QString("%1 minute%2").arg(QString::number(minutes),
                              minutes == 1 ? "" : "s");
        else
        {
            auto hours = minutes / 60;
            auto min = minutes % 60;
            timestr = QString("%1 hour%2 %3 minute%4").arg(
                              QString::number(hours),
                              hours == 1 ? "" : "s",
                              QString::number(min),
                              min == 1 ? "" : "s");
        }
        time = time.arg(timestr);
        m_time->setText(time);
    }
}
