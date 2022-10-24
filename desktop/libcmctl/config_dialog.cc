// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on CMST
// Copyright (C) 2013-2022 Andrew J. Bibb
// SPDX-License-Identifier: MIT

#include "config_dialog.h"
#include "private/config_dialog_p.h"
#include "service.h"

HWCM::ConfigDialogPrivate::ConfigDialogPrivate(Service *service, ConfigDialog *parent)
    : m_q(parent)
    , m_service(service)
{
    ui.setupUi(parent);
    parent->setWindowTitle(tr("Configuring Interface: %1").arg(service->nickName()));
    ui.btn_help->setIcon(QIcon::fromTheme("help-contextual"));
    ui.btn_help->setIconSize(QSize(20,20));

    // Setup comboboxes
    ui.comboBox_ipv4method->addItems(QStringList()
                          << QCoreApplication::translate("NetworkApplet", "Automatic (DHCP)", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Manual", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Disabled", nullptr));

    ui.comboBox_ipv6method->addItems(QStringList()
                          << QCoreApplication::translate("NetworkApplet", "Automatic", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Manual", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Disabled", nullptr));

    ui.comboBox_ipv6privacy->addItems(QStringList()
                          << QCoreApplication::translate("NetworkApplet", "Disabled", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Enabled", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Preferred", nullptr));

    ui.comboBox_proxymethod->addItems(QStringList()
                          << QCoreApplication::translate("NetworkApplet", "Direct Connection (No Proxy)", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Automatic Proxy Configuration", nullptr)
                          << QCoreApplication::translate("NetworkApplet", "Manual Proxy Configuration", nullptr));

    readConfig();
    ip4ConfigSelectionChanged();
    ip6ConfigSelectionChanged();
    proxyConfigSelectionChanged();
    connect(ui.comboBox_ipv4method, &QComboBox::currentIndexChanged, this, &ConfigDialogPrivate::ip4ConfigSelectionChanged);
    connect(ui.comboBox_ipv6method, &QComboBox::currentIndexChanged, this, &ConfigDialogPrivate::ip6ConfigSelectionChanged);
    connect(ui.comboBox_proxymethod, &QComboBox::currentIndexChanged, this, &ConfigDialogPrivate::proxyConfigSelectionChanged);

    //connect(ui.pushButton_ok, &QPushButton::clicked, this, &ConfigDialogPrivate::saveChanges);
}

void HWCM::ConfigDialogPrivate::readConfig()
{
    // Using the ValidatingDialog only to obtain the validator. This
    // allows all the validating code to be in a single location.
    // QLineEdits (validated)  that allow single address
    /* QRegularExpressionValidator* qrex_val4= new QRegularExpressionValidator(QRegularExpression(shared::ValidatingDialog(this).getPattern(CMST::ValDialog_IPv4)) );
       ui.lineEdit_ipv4address->setValidator(qrex_val4);
       ui.lineEdit_ipv4netmask->setValidator(qrex_val4);
       ui.lineEdit_ipv4gateway->setValidator(qrex_val4);

    QRegularExpressionValidator* qrex_val6= new QRegularExpressionValidator(QRegularExpression(shared::ValidatingDialog(this).getPattern(CMST::ValDialog_IPv6)) );
       ui.lineEdit_ipv6address->setValidator(qrex_val6);
       ui.lineEdit_ipv6gateway->setValidator(qrex_val6);

     // now QLineEdits (validated)   that allow multiple addresses
    QRegularExpressionValidator* qrex_val46= new QRegularExpressionValidator(QRegularExpression(shared::ValidatingDialog(this).getPattern(CMST::ValDialog_46, true)) );
       ui.lineEdit_nameservers->setValidator(qrex_val46);
       ui.lineEdit_timeservers->setValidator(qrex_val46);*/

    /*qrex_val4->deleteLater();
    qrex_val6->deleteLater();
    qrex_val46->deleteLater();*/

    // Seed initial values in the dialog.
    ui.checkBox_autoconnect->setChecked(m_service->mapValue("AutoConnect").toBool());
    ui.checkBox_mdns->setChecked(m_service->mapValue("mDNS").toBool());

    auto nameservers = m_service->mapValue("Nameservers.Configuration").toStringList();
    auto timeservers = m_service->mapValue("Timeservers.Configuration").toStringList();
    auto dhcpnameservers = m_service->mapValue("Nameservers").toStringList();
    auto dhcptimeservers = m_service->mapValue("Timeservers").toStringList();

    ui.lineEdit_nameservers->setPlaceholderText(dhcpnameservers.join(' '));
    ui.lineEdit_nameservers->setText(nameservers.join(' '));

    ui.lineEdit_timeservers->setPlaceholderText(dhcptimeservers.join(' '));
    ui.lineEdit_timeservers->setText(timeservers.join(' '));

    auto domains = m_service->mapValue("Domains.Configuration").toStringList();
    auto dhcpdomains = m_service->mapValue("Domains").toStringList();
    ui.lineEdit_domains->setPlaceholderText(dhcpdomains.join(' '));
    ui.lineEdit_domains->setText(domains.join(' '));

    // ipv4 page
    auto cboval = m_service->ipv4Config();
    ui.comboBox_ipv4method->setCurrentIndex((int)cboval);
    if(cboval == 0)
    {
        ui.lineEdit_ipv4address->setText(m_service->ipv4MapValue("Address").toString());
        ui.lineEdit_ipv4netmask->setText(m_service->ipv4MapValue("Netmask").toString());
        ui.lineEdit_ipv4gateway->setText(m_service->ipv4MapValue("Gateway").toString());
    }
    if(cboval == 1)
    {
        ui.lineEdit_ipv4address->setText(m_service->ipv4ConfigMapValue("Address").toString());
        ui.lineEdit_ipv4netmask->setText(m_service->ipv4ConfigMapValue("Netmask").toString());
        ui.lineEdit_ipv4gateway->setText(m_service->ipv4ConfigMapValue("Gateway").toString());
    }

    // ipv6 page
    auto cbo6val = m_service->ipv6Config();
    ui.comboBox_ipv6method->setCurrentIndex((int)cbo6val);
    ui.spinBox_ipv6prefixlength->setValue(m_service->ipv6MapValue("PrefixLength").toUInt());
    ui.lineEdit_ipv6address->setText(m_service->ipv6MapValue("Address").toString());
    ui.lineEdit_ipv6gateway->setText(m_service->ipv6MapValue("Gateway").toString());
    auto privacy = m_service->ipv6PrivacyConfig();
    ui.comboBox_ipv6privacy->setCurrentIndex((int)privacy);

    // proxy page
    auto proxyval = m_service->proxyConfig();
    ui.comboBox_proxymethod->setCurrentIndex((int)proxyval);
    ui.lineEdit_proxyservers->setText(m_service->proxyMapValue("Servers").toStringList().join("\n"));
    ui.lineEdit_proxyexcludes->setText(m_service->proxyMapValue("Excludes").toStringList().join("\n"));
    ui.lineEdit_proxyurl->setText(m_service->proxyMapValue("URL").toString());

    // disable pages not needed for a service (mainly vpn)
    /*if (m_service->type() == "vpn")
    {
        ui.->setDisabled(true);
        ui.ipv6->setDisabled(true);
    }*/

}

void HWCM::ConfigDialogPrivate::saveChanges()
{

}

void HWCM::ConfigDialogPrivate::ip4ConfigSelectionChanged()
{
    int val = ui.comboBox_ipv4method->currentIndex();
    if(val > 2)
        val = 2;

    HWCM::Service::IP4Config ip4cfg = (HWCM::Service::IP4Config)val;

    switch(ip4cfg)
    {
    case HWCM::Service::V4Manual:
        ui.lineEdit_ipv4netmask->setVisible(true);
        ui.lineEdit_ipv4address->setVisible(true);
        ui.lineEdit_ipv4gateway->setVisible(true);

        ui.label_ipv4netmask->setVisible(true);
        ui.label_ipv4address->setVisible(true);
        ui.label_ipv4gateway->setVisible(true);

        ui.lineEdit_ipv4netmask->setEnabled(true);
        ui.lineEdit_ipv4address->setEnabled(true);
        ui.lineEdit_ipv4gateway->setEnabled(true);
        break;
    case HWCM::Service::V4DHCP:
        ui.lineEdit_ipv4netmask->setVisible(true);
        ui.lineEdit_ipv4address->setVisible(true);
        ui.lineEdit_ipv4gateway->setVisible(true);

        ui.label_ipv4netmask->setVisible(true);
        ui.label_ipv4address->setVisible(true);
        ui.label_ipv4gateway->setVisible(true);

        ui.lineEdit_ipv4netmask->setEnabled(false);
        ui.lineEdit_ipv4address->setEnabled(false);
        ui.lineEdit_ipv4gateway->setEnabled(false);
        break;
    case HWCM::Service::V4Disabled:
    default:
        ui.lineEdit_ipv4netmask->setVisible(false);
        ui.lineEdit_ipv4address->setVisible(false);
        ui.lineEdit_ipv4gateway->setVisible(false);

        ui.label_ipv4netmask->setVisible(false);
        ui.label_ipv4address->setVisible(false);
        ui.label_ipv4gateway->setVisible(false);

        ui.lineEdit_ipv4netmask->setEnabled(false);
        ui.lineEdit_ipv4address->setEnabled(false);
        ui.lineEdit_ipv4gateway->setEnabled(false);
    }
}

void HWCM::ConfigDialogPrivate::ip6ConfigSelectionChanged()
{
    int val = ui.comboBox_ipv6method->currentIndex();
    if(val > 2)
        val = 2;

    HWCM::Service::IP6Config ip6cfg = (HWCM::Service::IP6Config)val;

    switch(ip6cfg)
    {
    case HWCM::Service::V6Manual:
        ui.lineEdit_ipv6address->setVisible(true);
        ui.lineEdit_ipv6gateway->setVisible(true);
        ui.spinBox_ipv6prefixlength->setVisible(true);
        ui.comboBox_ipv6privacy->setVisible(true);

        ui.label_v6address->setVisible(true);
        ui.label_v6gateway->setVisible(true);
        ui.label_v6length->setVisible(true);
        ui.label_v6privacy->setVisible(true);

        ui.lineEdit_ipv6address->setEnabled(true);
        ui.lineEdit_ipv6gateway->setEnabled(true);
        ui.spinBox_ipv6prefixlength->setEnabled(true);
        ui.comboBox_ipv6privacy->setEnabled(true);
        break;
    case HWCM::Service::V6Automatic:
        ui.lineEdit_ipv6address->setVisible(true);
        ui.lineEdit_ipv6gateway->setVisible(true);
        ui.spinBox_ipv6prefixlength->setVisible(true);
        ui.comboBox_ipv6privacy->setVisible(true);

        ui.label_v6address->setVisible(true);
        ui.label_v6gateway->setVisible(true);
        ui.label_v6length->setVisible(true);
        ui.label_v6privacy->setVisible(true);

        ui.lineEdit_ipv6address->setEnabled(false);
        ui.lineEdit_ipv6gateway->setEnabled(false);
        ui.spinBox_ipv6prefixlength->setEnabled(false);
        ui.comboBox_ipv6privacy->setEnabled(true);
        break;
    case HWCM::Service::V6Disabled:
    default:
        ui.lineEdit_ipv6address->setVisible(false);
        ui.lineEdit_ipv6gateway->setVisible(false);
        ui.spinBox_ipv6prefixlength->setVisible(false);
        ui.comboBox_ipv6privacy->setVisible(false);

        ui.label_v6address->setVisible(false);
        ui.label_v6gateway->setVisible(false);
        ui.label_v6length->setVisible(false);
        ui.label_v6privacy->setVisible(false);

        ui.lineEdit_ipv6address->setEnabled(false);
        ui.lineEdit_ipv6gateway->setEnabled(false);
        ui.spinBox_ipv6prefixlength->setEnabled(false);
        ui.comboBox_ipv6privacy->setEnabled(false);
    }
}

void HWCM::ConfigDialogPrivate::proxyConfigSelectionChanged()
{
    int val = ui.comboBox_proxymethod->currentIndex();
    if(val > 2)
        val = 2;

    HWCM::Service::ProxyConfig proxycfg = (HWCM::Service::ProxyConfig)val;

    switch(proxycfg)
    {
    case HWCM::Service::AutomaticProxy:
        ui.label_proxyUrl->setVisible(true);
        ui.lineEdit_proxyurl->setVisible(true);

        ui.label_proxyServers->setVisible(false);
        ui.lineEdit_proxyservers->setVisible(false);

        ui.label_proxyexcludes->setVisible(false);
        ui.lineEdit_proxyexcludes->setVisible(false);
        break;
    case HWCM::Service::ManualProxy:
        ui.label_proxyUrl->setVisible(false);
        ui.lineEdit_proxyurl->setVisible(false);

        ui.label_proxyServers->setVisible(true);
        ui.lineEdit_proxyservers->setVisible(true);

        ui.label_proxyexcludes->setVisible(true);
        ui.lineEdit_proxyexcludes->setVisible(true);
        break;
    case HWCM::Service::NoProxy:
    default:
        ui.label_proxyUrl->setVisible(false);
        ui.lineEdit_proxyurl->setVisible(false);

        ui.label_proxyServers->setVisible(false);
        ui.lineEdit_proxyservers->setVisible(false);

        ui.label_proxyexcludes->setVisible(false);
        ui.lineEdit_proxyexcludes->setVisible(false);
    }
}


HWCM::ConfigDialog::ConfigDialog(Service *service, QWidget *parent)
    : QDialog(parent)
    , m_p(new ConfigDialogPrivate(service, this)) {}


