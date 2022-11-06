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
    m_tabs = new QTabWidget(m_host);
    m_tabs->setObjectName(QString::fromUtf8("m_tabs"));
    m_mains = new QWidget();
    m_mains->setObjectName(QString::fromUtf8("m_mains"));
    formLayout = new QFormLayout(m_mains);
    formLayout->setObjectName(QString::fromUtf8("formLayout"));
    formLayout->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    lbl_mains_comp_sleep = new QLabel(m_mains);
    lbl_mains_comp_sleep->setObjectName(QString::fromUtf8("lbl_mains_comp_sleep"));

    formLayout->setWidget(0, QFormLayout::LabelRole, lbl_mains_comp_sleep);

    m_mains_comp_sleep = new QSlider(m_mains);
    m_mains_comp_sleep->setObjectName(QString::fromUtf8("m_mains_comp_sleep"));
    m_mains_comp_sleep->setOrientation(Qt::Horizontal);

    formLayout->setWidget(0, QFormLayout::FieldRole, m_mains_comp_sleep);

    lbl_mains_disp_sleep = new QLabel(m_mains);
    lbl_mains_disp_sleep->setObjectName(QString::fromUtf8("lbl_mains_disp_sleep"));

    formLayout->setWidget(1, QFormLayout::LabelRole, lbl_mains_disp_sleep);

    m_mains_disp_sleep = new QSlider(m_mains);
    m_mains_disp_sleep->setObjectName(QString::fromUtf8("m_mains_disp_sleep"));
    m_mains_disp_sleep->setOrientation(Qt::Horizontal);

    formLayout->setWidget(1, QFormLayout::FieldRole, m_mains_disp_sleep);

    vs_mains = new QSpacerItem(518, 60, QSizePolicy::Minimum, QSizePolicy::Expanding);

    formLayout->setItem(2, QFormLayout::FieldRole, vs_mains);

    m_tabs->addTab(m_mains, QString());
    m_battery = new QWidget();
    m_battery->setObjectName(QString::fromUtf8("m_battery"));
    formLayout_3 = new QFormLayout(m_battery);
    formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
    formLayout_3->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    formLayout_3->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    lbl_batt_comp_sleep = new QLabel(m_battery);
    lbl_batt_comp_sleep->setObjectName(QString::fromUtf8("lbl_batt_comp_sleep"));

    formLayout_3->setWidget(0, QFormLayout::LabelRole, lbl_batt_comp_sleep);

    m_batt_comp_sleep = new QSlider(m_battery);
    m_batt_comp_sleep->setObjectName(QString::fromUtf8("m_batt_comp_sleep"));
    m_batt_comp_sleep->setOrientation(Qt::Horizontal);

    formLayout_3->setWidget(0, QFormLayout::FieldRole, m_batt_comp_sleep);

    lbl_batt_disp_sleep = new QLabel(m_battery);
    lbl_batt_disp_sleep->setObjectName(QString::fromUtf8("lbl_batt_disp_sleep"));

    formLayout_3->setWidget(1, QFormLayout::LabelRole, lbl_batt_disp_sleep);

    m_batt_disp_sleep = new QSlider(m_battery);
    m_batt_disp_sleep->setObjectName(QString::fromUtf8("m_batt_disp_sleep"));
    m_batt_disp_sleep->setOrientation(Qt::Horizontal);

    formLayout_3->setWidget(1, QFormLayout::FieldRole, m_batt_disp_sleep);

    vs_batt = new QSpacerItem(518, 60, QSizePolicy::Minimum, QSizePolicy::Expanding);

    formLayout_3->setItem(2, QFormLayout::FieldRole, vs_batt);

    m_tabs->addTab(m_battery, QString());

    mainLayout->addWidget(m_tabs);

    hl_bottom = new QHBoxLayout();
    m_showinmenu = new QCheckBox(m_host);
    hl_bottom->addWidget(m_showinmenu);

    hs_bottom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hl_bottom->addItem(hs_bottom);

    m_help = new QToolButton(m_host);

    hl_bottom->addWidget(m_help);


    mainLayout->addLayout(hl_bottom);

    lbl_mains_comp_sleep->setText(QCoreApplication::translate("Energy", "Computer sleep:", nullptr));
    lbl_mains_disp_sleep->setText(QCoreApplication::translate("Energy", "Display sleep:", nullptr));
    m_tabs->setTabText(m_tabs->indexOf(m_mains), QCoreApplication::translate("Energy", "Mains Power", nullptr));
    lbl_batt_comp_sleep->setText(QCoreApplication::translate("Energy", "Computer sleep:", nullptr));
    lbl_batt_disp_sleep->setText(QCoreApplication::translate("Energy", "Display sleep:", nullptr));
    m_tabs->setTabText(m_tabs->indexOf(m_battery), QCoreApplication::translate("Energy", "Battery", nullptr));
    m_showinmenu->setText(QCoreApplication::translate("Energy", "Show energy status in menu bar", nullptr));
}

