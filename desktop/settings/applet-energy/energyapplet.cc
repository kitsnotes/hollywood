#include "energyapplet.h"
#include "meter.h"

#include <QFile>
#include <QKeyEvent>
#include <QSettings>

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
    loadSettings();
    return true;
}

bool EnergyApplet::loadSettings()
{
    QSettings settings(QSettings::UserScope,
                       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup("Energy");
    bool preventSleepDisplay = settings.value("PreventSleepDisplayOffAC", false).toBool();
    bool preventSleepLid = settings.value("PreventSleepLidCloseAC", false).toBool();
    // TODO: global config
    //bool hardDiskSleep = settings.value("HardDiskSleepAC", true).toBool();

    uint displaySleepBattery = normalizeValue(settings.value("DisplaySleepBattery", 5).toUInt());
    uint displaySleepMains = normalizeValue(settings.value("DisplaySleepAC", 15).toUInt());

    settings.endGroup();
    settings.beginGroup("Stage");
    bool showInStage = settings.value("ShowBatteryMeter", true).toBool();

    m_showinmenu->setChecked(showInStage);
    m_preventsleep->setChecked(preventSleepDisplay);
    m_preventsleeplid->setChecked(preventSleepLid);

    m_batt_disp_sleep->setValue(m_batt_disp_sleep->valueToSliderPosition(displaySleepBattery));
    m_mains_disp_sleep->setValue(m_mains_disp_sleep->valueToSliderPosition(displaySleepMains));
    return true;
}

bool EnergyApplet::saveSettings()
{
    return true;
}

bool EnergyApplet::closeUp()
{
    delete m_host;
    m_host = nullptr;
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
    m_disp_sleep_val = new QLabel(m_mainssettings);
    connect(m_mains_disp_sleep, &SleepSlider::valueChanged, this, [this](){
        auto val = m_mains_disp_sleep->valueFromSliderPosition(m_mains_disp_sleep->value());
        m_disp_sleep_val->setText(m_mains_disp_sleep->formatTime(val));
    });

    m_preventsleep = new QCheckBox(tr("Prevent computer from sleeping automatically when the display is off"), m_mainssettings);
    m_preventsleeplid = new QCheckBox(tr("Prevent computer from sleeping automatically when the lid is closed"), m_mainssettings);
    m_harddisks = new QCheckBox(tr("Put hard disks to sleep when possible"), m_mainssettings);
    fl_mains->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_mains->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    fl_mains->setWidget(0, QFormLayout::LabelRole, lbl_mains_disp_sleep);
    fl_mains->setWidget(0, QFormLayout::FieldRole, m_disp_sleep_val);

    fl_mains->setWidget(1, QFormLayout::SpanningRole, m_mains_disp_sleep);
    fl_mains->setWidget(2, QFormLayout::SpanningRole, m_preventsleep);
    fl_mains->setWidget(3, QFormLayout::SpanningRole, m_preventsleeplid);
    fl_mains->setWidget(4, QFormLayout::SpanningRole, m_harddisks);
    fl_mains->setItem(5, QFormLayout::FieldRole, new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));


    fl_battery = new QFormLayout(m_batterysettings);
    lbl_batt_disp_sleep = new QLabel(m_batterysettings);
    m_batt_disp_sleep = new SleepSlider(m_batterysettings);
    vs_batt = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_disp_bat_sleep_val = new QLabel(m_mainssettings);
    connect(m_batt_disp_sleep, &SleepSlider::valueChanged, this, [this](){
        auto val = m_batt_disp_sleep->valueFromSliderPosition(m_batt_disp_sleep->value());
        m_disp_bat_sleep_val->setText(m_batt_disp_sleep->formatTime(val));
    });
    m_showinmenu = new QCheckBox(m_host);

    fl_battery->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_battery->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    fl_battery->setWidget(0, QFormLayout::LabelRole, lbl_batt_disp_sleep);
    fl_battery->setWidget(0, QFormLayout::FieldRole, m_disp_bat_sleep_val);
    fl_battery->setWidget(1, QFormLayout::SpanningRole, m_batt_disp_sleep);
    fl_battery->setWidget(2, QFormLayout::SpanningRole, m_showinmenu);
    fl_battery->setItem(3, QFormLayout::FieldRole, vs_batt);

    for(auto w : m_batteries)
        mainLayout->addWidget(w);

    mainLayout->addWidget(m_batterysettings);
    mainLayout->addWidget(m_mainssettings);

    m_help = new QToolButton(m_host);
    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(22,22));

    // TODO: implement and remove these
    m_help->setEnabled(false);

    m_warning_icon = new QLabel(m_host);
    m_energy_warning = new QLabel(m_host);

    m_warning_icon->setPixmap(QIcon::fromTheme("emblem-important").pixmap(22,22));
    m_energy_warning->setText(tr("The selected settings may increase energy consumption."));
    hl_bottom = new QHBoxLayout();
    hl_bottom->addWidget(m_warning_icon);
    hl_bottom->addWidget(m_energy_warning);

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

uint EnergyApplet::normalizeValue(uint minutes)
{
    if(minutes > 240)
        return 240;


    return minutes;
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

    connect(this, &QAbstractSlider::sliderMoved, this, [this](uint value) {
        if(value > 70 && value < 75)
            setValue(75);
    });
}


uint SleepSlider::valueFromSliderPosition(uint sliderPos)
{
    if(sliderPos == 0 || sliderPos == 1)
        return 1; // 1 minute, minimum value

    if(sliderPos > 1 && sliderPos <= 30)
        return sliderPos; // up to 30 minutes, we are a tick a min

    if(sliderPos > 30 && sliderPos <= 40)
        return 30+(sliderPos-30)*3; // 1 tick == 3 min until 1h

    if(sliderPos > 40 && sliderPos <= 70)
        return 60+(sliderPos-40)*6; // 1 tick == 6 min until 4h (end)

    return 0; // never (off)
}

uint SleepSlider::valueToSliderPosition(uint value)
{
    if(value == 0)
        return 75; // never

    if(value == 1)
        return 0; // 1 gets at the end

    if(value > 1 && value <= 30)
        return value; // tick == minute

    if(value > 30 && value <= 60)
        return 30+((value-30)/3);

    if(value > 240)
        value = 240; // normalization

    if(value > 60 && value <= 240)
        return 60+((value-60)/6);

    return 0; // should never come here
}

QString SleepSlider::formatTime(uint minutes)
{
    if(minutes == 0)
        return tr("Never");

    if(minutes == 1)
        return tr("1 minute");

    if(minutes < 60)
        return tr("%1 minutes").arg(minutes);

    int hours = minutes / 60;
    int min = minutes % 60;

    QString suf = tr("minutes");
    if(min == 1)
        suf = tr("minute");

    if(hours == 1)
    {
        if(min != 0)
            return tr("%1 hour %2 %3").arg(QString::number(hours), QString::number(min), suf);
        else
            return tr("%1 hour").arg(QString::number(hours));
    }

    if(min != 0)
        return tr("%1 hours %2 %3").arg(QString::number(hours), QString::number(min), suf);
    else
        return tr("%1 hours").arg(QString::number(hours));
}

void SleepSlider::keyPressEvent(QKeyEvent *ev)
{
    auto curval = value();
    if(curval > 70)
    {
        // we are on a "never" so lets do the snap
        if(ev->key() == Qt::Key_Left || ev->key() == Qt::Key_Down || ev->key() == Qt::Key_PageDown)
        {
            ev->accept();
            setValue(70);
            return;
        }
    }
    if(curval >= 70)
    {
        // we are on 4 hours so lets do the snap
        if(ev->key() == Qt::Key_Right || ev->key() == Qt::Key_Up || ev->key() == Qt::Key_PageUp)
        {
            ev->accept();
            setValue(75);
            return;
        }
    }

    LabeledSlider::keyPressEvent(ev);
}
