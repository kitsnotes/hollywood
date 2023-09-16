// Hollywood Network Applet
// (C) 2021, 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#include "netapplet.h"
#include "service.h"
#include "model.h"

#include <config_dialog.h>
#include <wifimenu.h>

using namespace HWCM;

NetworkApplet::NetworkApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool NetworkApplet::init()
{
    setupWidget();
    m_cm = new HWCM::ConnectionManager(this);
    m_interfaces->setModel(m_cm->model());

    connect(m_interfaces->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &NetworkApplet::interfaceSelectionChanged);

    connect(m_ip4config, &QComboBox::currentIndexChanged, this, &NetworkApplet::ip4ConfigSelectionChanged);

    if(m_cm->model()->hasIndex(0,0))
        m_interfaces->selectionModel()->select(m_cm->model()->index(0), QItemSelectionModel::ClearAndSelect);

    m_wifi->setMenu(m_cm->wifiMenu());
    connect(m_cm->wifiMenu(), &WifiMenu::triggeredIndex, this, &NetworkApplet::wifiMenuSelected);
    return true;
}

bool NetworkApplet::loadSettings()
{
    return true;
}

bool NetworkApplet::saveSettings()
{
    return true;
}

bool NetworkApplet::closeUp()
{
    delete m_host;
    m_host = nullptr;

    delete m_cm;
    m_cm = nullptr;

    return true;
}

QString NetworkApplet::id() const
{
    return QLatin1String("org.originull.network");
}

QString NetworkApplet::name() const
{
    return tr("Network");
}

QString NetworkApplet::description() const
{
    return tr("Network & Wi-Fi Settings");
}

QIcon NetworkApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-system-network"));
    return mine;
}

QWidget *NetworkApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category NetworkApplet::category() const
{
    return System;
}

QStringList NetworkApplet::searchTokens() const
{
    return QStringList();
}

void NetworkApplet::interfaceSelectionChanged()
{
    if(m_interfaces->selectionModel()->selectedIndexes().isEmpty())
        return;

    auto idx = m_interfaces->selectionModel()->selectedIndexes().first();
    if(!idx.isValid())
        return;

    auto sel = m_cm->model()->serviceForIndex(idx);
    if(sel == nullptr && !m_cm->model()->isIndexWifi(idx))
    {
        m_selectedService = nullptr;
        showEthernetUnplugged();
        return;
    }

    if(sel == nullptr && !m_cm->model()->isIndexWifi(idx))
    {
        m_selectedService = nullptr;
        showWifi();
        m_autoJoinWifi->setVisible(false);
        m_status->setText(tr("Disconnected"));
        lb_ip->setVisible(false);
        m_ipv4address->setVisible(false);
        lbl_ipv4->setVisible(false);
        m_ip4config->setVisible(false);
        m_advanced->setVisible(false);
        return;
    }

    m_selectedService = sel;
    if(sel->type() == "wifi")
    {
        m_wifi->setText(sel->nickName());
        m_autoJoinWifi->setChecked(sel->mapValue("AutoConnect").toBool());
        showWifi();
    }

    if(sel->type() == "ethernet")
        showEthernetPlugged();

    auto cboval = sel->ipv4Config();
    m_ip4config->setCurrentIndex((int)cboval);
    m_status->setText(sel->mapValue("State").toString());
    m_ipv4address->setText(sel->ipv4MapValue("Address").toString());
    m_subnet->setText(sel->ipv4MapValue("Netmask").toString());
    m_gateway->setText(sel->ipv4MapValue("Gateway").toString());
    m_dns->setText(sel->mapValue("Nameservers").toString());
    m_searchdom->setText(sel->mapValue("Domains").toString());
    ip4ConfigSelectionChanged();
}

void NetworkApplet::ip4ConfigSelectionChanged()
{
    int val = m_ip4config->currentIndex();
    if(val > 2)
        val = 2;

    HWCM::Service::IP4Config ip4cfg = (HWCM::Service::IP4Config)val;

    switch(ip4cfg)
    {
    case HWCM::Service::V4Manual:
        m_subnet->setEnabled(true);
        m_ipv4address->setEnabled(true);
        m_gateway->setEnabled(true);
        break;
    case HWCM::Service::V4DHCP:
    case HWCM::Service::V4Disabled:
    default:
        m_subnet->setEnabled(false);
        m_ipv4address->setEnabled(false);
        m_gateway->setEnabled(false);
    }
}

void NetworkApplet::showServiceConfiguration()
{
    if(m_selectedService == nullptr)
        return;

    auto dlg = m_cm->getConfigDialog(m_selectedService, m_host);
    dlg->show();
}

void NetworkApplet::wifiMenuSelected(const QModelIndex &idx)
{
    auto svc = m_cm->wifiModel()->serviceForIndex(idx);
    if(svc == nullptr)
        return;

    svc->requestConnection();
}

void NetworkApplet::setupWidget()
{
    m_host = new QWidget(0);
    auto main_layout = new QHBoxLayout(m_host);
    m_interfaces = new QListView(m_host);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_interfaces->sizePolicy().hasHeightForWidth());
    m_interfaces->setSizePolicy(sizePolicy);

    m_details = new QFrame(m_host);
    m_details->setFrameShape(QFrame::StyledPanel);
    m_details->setFrameShadow(QFrame::Raised);
    auto frame_layout = new QVBoxLayout(m_details);

    auto hlMain = new QHBoxLayout();
    auto hs_top = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_end = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vs_main = new QSpacerItem(20, 180, QSizePolicy::Minimum, QSizePolicy::Expanding);
    auto flInterface = new QFormLayout();

    auto hl_bottom = new QHBoxLayout();
    m_wifiStatus = new QCheckBox(m_details);
    m_advanced = new QPushButton(m_details);
    m_help = new QToolButton(m_details);
    auto hs_bottom = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    l_plugcable = new QLabel(m_details);
    l_status = new QLabel(m_details);
    m_status = new QLabel(m_details);

    lbl_ipv4 = new QLabel(m_details);
    m_ip4config = new QComboBox(m_details);
    m_ip4config->setMinimumSize(QSize(220, 0));

    lb_ip = new QLabel(m_details);
    m_ipv4address = new QLineEdit(m_details);

    lb_subnet = new QLabel(m_details);
    m_subnet = new QLineEdit(m_details);
    lb_gateway = new QLabel(m_details);
    m_gateway = new QLineEdit(m_details);
    lb_dns = new QLabel(m_details);
    m_dns = new QLabel(m_details);
    lb_searchdom = new QLabel(m_details);
    m_searchdom = new QLabel(m_details);

    l_wifiname = new QLabel(m_details);
    m_wifi = new QToolButton(m_details);
    m_autoJoinWifi = new QCheckBox(m_details);

    m_wifi->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    flInterface->setLabelAlignment(Qt::AlignRight);
    flInterface->setWidget(0, QFormLayout::SpanningRole, l_plugcable);
    flInterface->setWidget(1, QFormLayout::LabelRole, l_status);
    flInterface->setWidget(1, QFormLayout::FieldRole, m_status);
    flInterface->setWidget(2, QFormLayout::LabelRole, l_wifiname);
    flInterface->setWidget(2, QFormLayout::FieldRole, m_wifi);
    flInterface->setWidget(3, QFormLayout::FieldRole, m_autoJoinWifi);

    flInterface->setWidget(4, QFormLayout::LabelRole, lbl_ipv4);
    flInterface->setWidget(4, QFormLayout::FieldRole, m_ip4config);
    flInterface->setWidget(5, QFormLayout::LabelRole, lb_ip);
    flInterface->setWidget(5, QFormLayout::FieldRole, m_ipv4address);
    flInterface->setWidget(6, QFormLayout::LabelRole, lb_subnet);
    flInterface->setWidget(6, QFormLayout::FieldRole, m_subnet);
    flInterface->setWidget(7, QFormLayout::LabelRole, lb_gateway);
    flInterface->setWidget(7, QFormLayout::FieldRole, m_gateway);
    flInterface->setWidget(8, QFormLayout::LabelRole, lb_dns);
    flInterface->setWidget(8, QFormLayout::FieldRole, m_dns);
    flInterface->setWidget(9, QFormLayout::LabelRole, lb_searchdom);
    flInterface->setWidget(9, QFormLayout::FieldRole, m_searchdom);

    hl_bottom->addWidget(m_wifiStatus);
    hl_bottom->addItem(hs_bottom);
    hl_bottom->addWidget(m_advanced);
    hl_bottom->addWidget(m_help);

    hlMain->addItem(hs_top);
    hlMain->addLayout(flInterface);
    hlMain->addItem(hs_end);

    frame_layout->addLayout(hlMain);
    frame_layout->addItem(vs_main);
    frame_layout->addLayout(hl_bottom);

    main_layout->addWidget(m_interfaces);
    main_layout->addWidget(m_details);

    m_ip4config->addItems(QStringList()
                          << QCoreApplication::translate("NetworkApplet", "Automatic (DHCP)", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Manual", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Disabled", nullptr));

    l_plugcable->setText(QCoreApplication::translate("NetworkApplet", "The network cable is unplugged. Plug in an Ethernet cable to this adapter for configuration options.", nullptr));
    l_status->setText(QCoreApplication::translate("NetworkApplet", "Status:", nullptr));
    lbl_ipv4->setText(QCoreApplication::translate("NetworkApplet", "IPv4 Configuration:", nullptr));
    lb_ip->setText(QCoreApplication::translate("NetworkApplet", "IPv4 Address:", nullptr));
    lb_subnet->setText(QCoreApplication::translate("NetworkApplet", "Subnet Mask:", nullptr));
    lb_gateway->setText(QCoreApplication::translate("NetworkApplet", "Gateway:", nullptr));
    lb_dns->setText(QCoreApplication::translate("NetworkApplet", "DNS:", nullptr));
    lb_searchdom->setText(QCoreApplication::translate("NetworkApplet", "Search Domain:", nullptr));
    m_wifiStatus->setText(QCoreApplication::translate("NetworkApplet", "Show Wi-Fi in menu bar", nullptr));
    m_advanced->setText(QCoreApplication::translate("NetworkApplet", "Advanced...", nullptr));
    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(22,22));
    l_wifiname->setText(QCoreApplication::translate("NetworkApplet", "Network Name:", nullptr));
    m_autoJoinWifi->setText(QCoreApplication::translate("NetworkApplet", "Automatically join this network", nullptr));

    m_wifiStatus->setVisible(false);

    connect(m_advanced, &QPushButton::pressed, this, &NetworkApplet::showServiceConfiguration);
}

void NetworkApplet::showWifi()
{
    l_plugcable->setVisible(false);

    lbl_ipv4->setVisible(true);
    m_ip4config->setVisible(true);
    l_status->setVisible(true);
    m_status->setVisible(true);


    // hide ethernet things
    lb_subnet->setVisible(false);
    m_subnet->setVisible(false);
    lb_gateway->setVisible(false);
    m_gateway->setVisible(false);
    lb_dns->setVisible(false);
    m_dns->setVisible(false);
    lb_searchdom->setVisible(false);
    m_searchdom->setVisible(false);

    m_advanced->setVisible(true);

    // show wifi things
    lb_ip->setVisible(true);
    m_ipv4address->setVisible(true);
    m_wifiStatus->setVisible(true);
    l_wifiname->setVisible(true);
    m_wifi->setVisible(true);
    m_autoJoinWifi->setVisible(true);

}

void NetworkApplet::showEthernetPlugged()
{
    l_plugcable->setVisible(false);

    l_status->setVisible(true);
    m_status->setVisible(true);

    lbl_ipv4->setVisible(true);
    m_ip4config->setVisible(true);

    lb_ip->setVisible(true);
    m_ipv4address->setVisible(true);
    lb_subnet->setVisible(true);
    m_subnet->setVisible(true);
    lb_gateway->setVisible(true);
    m_gateway->setVisible(true);
    lb_dns->setVisible(true);
    m_dns->setVisible(true);
    lb_searchdom->setVisible(true);
    m_searchdom->setVisible(true);

    m_advanced->setVisible(true);

    // hide wifi things
    m_wifiStatus->setVisible(false);
    l_wifiname->setVisible(false);
    m_wifi->setVisible(false);
    m_autoJoinWifi->setVisible(false);

}

void NetworkApplet::showEthernetUnplugged()
{
    l_plugcable->setVisible(true);

    l_status->setVisible(false);
    m_status->setVisible(false);

    lbl_ipv4->setVisible(false);
    m_ip4config->setVisible(false);
    lb_ip->setVisible(false);
    m_ipv4address->setVisible(false);

    l_wifiname->setVisible(false);
    m_wifi->setVisible(false);
    m_autoJoinWifi->setVisible(false);

    lb_subnet->setVisible(false);
    m_subnet->setVisible(false);
    lb_gateway->setVisible(false);
    m_gateway->setVisible(false);
    lb_dns->setVisible(false);
    m_dns->setVisible(false);
    lb_searchdom->setVisible(false);
    m_searchdom->setVisible(false);
    m_wifiStatus->setVisible(false);
    m_advanced->setVisible(false);
}
