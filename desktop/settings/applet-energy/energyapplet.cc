#include "energyapplet.h"
#include "meter.h"

#include <QFile>

EnergyApplet::EnergyApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
    m_upower = new UPowerInterface(this);
    enumerateUPower();
}

bool EnergyApplet::init()
{
    setupWidget();
    if(!queryForPortability())
    {
        m_batterysettings->setVisible(false);
        m_preventsleeplid->setVisible(false);
    }
    return true;
}

QString EnergyApplet::id() const
{
    return QLatin1String("org.originull.energy");
}

QString EnergyApplet::name() const
{
    return tr("Energy");
}

QString EnergyApplet::description() const
{
    return tr("Configure & monitor battery and energy settings.");
}

QIcon EnergyApplet::icon() const
{
    return QIcon::fromTheme("preferences-system-power-management");
}

QWidget *EnergyApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category EnergyApplet::category() const
{
    return System;
}

void EnergyApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("EnergyAppletHost"));

    mainLayout = new QVBoxLayout(m_host);

    m_batterysettings = new QGroupBox(tr("Settings on battery power..."));
    m_mainssettings = new QGroupBox(tr("Settings on power adapter..."));

    auto fl_mains = new QFormLayout(m_mainssettings);
    lbl_mains_disp_sleep = new QLabel(m_mainssettings);
    m_mains_disp_sleep = new SleepSlider(m_mainssettings);


    m_preventsleep = new QCheckBox(tr("Prevent computer from sleeping automatically when the display is off"), m_mainssettings);
    m_preventsleeplid = new QCheckBox(tr("Prevent computer from sleeping automatically when the lid is closed"), m_mainssettings);
    m_harddisks = new QCheckBox(tr("Put hard disks to sleep when possible"), m_mainssettings);
    fl_mains->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_mains->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    fl_mains->setWidget(0, QFormLayout::LabelRole, lbl_mains_disp_sleep);
    fl_mains->setWidget(1, QFormLayout::SpanningRole, m_mains_disp_sleep);
    fl_mains->setWidget(2, QFormLayout::SpanningRole, m_preventsleep);
    fl_mains->setWidget(3, QFormLayout::SpanningRole, m_preventsleeplid);
    fl_mains->setWidget(4, QFormLayout::SpanningRole, m_harddisks);
    fl_mains->setItem(5, QFormLayout::FieldRole, new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));


    fl_battery = new QFormLayout(m_batterysettings);
    lbl_batt_disp_sleep = new QLabel(m_batterysettings);
    m_batt_disp_sleep = new SleepSlider(m_batterysettings);
    vs_batt = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_batt_disp_sleep->setOrientation(Qt::Horizontal);
    m_showinmenu = new QCheckBox(m_host);

    fl_battery->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_battery->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    fl_battery->setWidget(0, QFormLayout::LabelRole, lbl_batt_disp_sleep);
    fl_battery->setWidget(0, QFormLayout::FieldRole, m_batt_disp_sleep);
    fl_battery->setWidget(1, QFormLayout::SpanningRole, m_showinmenu);
    fl_battery->setItem(2, QFormLayout::FieldRole, vs_batt);

    for(auto w : m_batteries)
        mainLayout->addWidget(w);

    mainLayout->addWidget(m_batterysettings);
    mainLayout->addWidget(m_mainssettings);

    m_help = new QToolButton(m_host);
    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(22,22));

    // TODO: implement and remove these
    m_help->setEnabled(false);

    hl_bottom = new QHBoxLayout();
    hs_bottom = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl_bottom->addItem(hs_bottom);
    hl_bottom->addWidget(m_help);
    mainLayout->addLayout(hl_bottom);

    lbl_mains_disp_sleep->setText(QCoreApplication::translate("Energy", "Put display to sleep after:", nullptr));
    lbl_batt_disp_sleep->setText(QCoreApplication::translate("Energy", "Put display to sleep after:", nullptr));
    m_showinmenu->setText(QCoreApplication::translate("Energy", "Show battery status in stage", nullptr));
}

bool EnergyApplet::queryForPortability()
{
    QByteArray val("0");
    QFile file("/sys/class/dmi/id/chassis_type");
    if(file.open(QFile::ReadOnly))
    {
        val=file.readAll().trimmed();
        file.close();
    }

    QVector<int> acceptable;
    acceptable << 8 << 9 << 10 << 11 << 14 << 30 << 31 << 32;
    if(acceptable.contains(val.toInt()))
        return true;

    return false;
}

void EnergyApplet::enumerateUPower()
{
    if(!m_upower && !m_upower->isValid())
        qWarning() << "Invalid UPower state";
    else
    {
        for(auto dev : m_upower->enumerateDevices())
        {
            auto device = new UPowerDeviceInterface(dev.path(), this);
            if(device->type() == 2)
            {
                auto widget = new BatteryMeterWidget(device);
                m_batteries.append(widget);
            }
            else if(device->type() == 3)
            {
                auto widget = new BatteryMeterWidget(device);
                m_ups_batteries.append(widget);
            }
            else
                delete device;
        }
    }
}

SleepSlider::SleepSlider(QWidget *parent)
    :LabeledSlider(parent)
{
    setOrientation(Qt::Horizontal);
    setRange(0,75);
    setPageStep(5);
    setTickInterval(5);
    setTickPosition(QSlider::TicksBelow);
    QMap<int,QString> tickLabels;
    tickLabels.insert(0,tr("1m"));
    tickLabels.insert(15,tr("15m"));
    tickLabels.insert(30,tr("30m"));
    tickLabels.insert(40,tr("1h"));
    tickLabels.insert(50,tr("2h"));
    tickLabels.insert(60,tr("3h"));
    tickLabels.insert(70,tr("4h"));
    tickLabels.insert(75,tr("Never"));
    setTickLabels(tickLabels);
}
