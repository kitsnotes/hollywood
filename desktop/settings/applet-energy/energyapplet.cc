#include "energyapplet.h"

EnergyApplet::EnergyApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool EnergyApplet::init()
{
    setupWidget();
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
    m_mainssettings = new QGroupBox(tr("Settings on mains power..."));

    auto fl_mains = new QFormLayout(m_mainssettings);
    lbl_mains_disp_sleep = new QLabel(m_mainssettings);
    m_mains_disp_sleep = new QSlider(m_mainssettings);
    m_mains_disp_sleep->setOrientation(Qt::Horizontal);
    m_preventsleep = new QCheckBox(tr("Prevent computer from sleeping automatically when the display is off"), m_mainssettings);
    m_preventsleeplid = new QCheckBox(tr("Prevent computer from sleeping automatically when the lid is closed"), m_mainssettings);
    m_harddisks = new QCheckBox(tr("Put hard disks to sleep when possible"), m_mainssettings);
    fl_mains->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_mains->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    fl_mains->setWidget(0, QFormLayout::LabelRole, lbl_mains_disp_sleep);
    fl_mains->setWidget(0, QFormLayout::FieldRole, m_mains_disp_sleep);
    fl_mains->setWidget(1, QFormLayout::SpanningRole, m_preventsleep);
    fl_mains->setWidget(2, QFormLayout::SpanningRole, m_preventsleeplid);
    fl_mains->setWidget(3, QFormLayout::SpanningRole, m_harddisks);
    fl_mains->setItem(4, QFormLayout::FieldRole, new QSpacerItem(518, 60, QSizePolicy::Minimum, QSizePolicy::Expanding));


    fl_battery = new QFormLayout(m_batterysettings);
    lbl_batt_disp_sleep = new QLabel(m_batterysettings);
    m_batt_disp_sleep = new QSlider(m_batterysettings);
    vs_batt = new QSpacerItem(518, 60, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_batt_disp_sleep->setOrientation(Qt::Horizontal);
    m_showinmenu = new QCheckBox(m_host);

    fl_battery->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_battery->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    fl_battery->setWidget(0, QFormLayout::LabelRole, lbl_batt_disp_sleep);
    fl_battery->setWidget(0, QFormLayout::FieldRole, m_batt_disp_sleep);
    fl_battery->setWidget(1, QFormLayout::SpanningRole, m_showinmenu);
    fl_battery->setItem(2, QFormLayout::FieldRole, vs_batt);

    mainLayout->addWidget(m_batterysettings);
    mainLayout->addWidget(m_mainssettings);

    m_help = new QToolButton(m_host);
    hl_bottom = new QHBoxLayout();
    hs_bottom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl_bottom->addItem(hs_bottom);
    hl_bottom->addWidget(m_help);
    mainLayout->addLayout(hl_bottom);

    lbl_mains_disp_sleep->setText(QCoreApplication::translate("Energy", "Display sleep:", nullptr));
    lbl_batt_disp_sleep->setText(QCoreApplication::translate("Energy", "Display sleep:", nullptr));
    m_showinmenu->setText(QCoreApplication::translate("Energy", "Show battery status in stage", nullptr));
}

