// Hollywood Network Applet
// (C) 2021, 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETAPPLET_H
#define NETAPPLET_H

#include <hollywood/appletinterface.h>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <connman.h>

class NetworkApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet-network.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit NetworkApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool canExit() const { return true; }
    bool closeUp();
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
private slots:
    void interfaceSelectionChanged();
    void ip4ConfigSelectionChanged();
    void showServiceConfiguration();
    void wifiMenuSelected(const QModelIndex &idx);
private:
    void setupWidget();
    void showWifi();
    void showEthernetPlugged();
    void showEthernetUnplugged();
    QWidget* m_host = nullptr;
    HWCM::ConnectionManager *m_cm = nullptr;
    HWCM::Service *m_selectedService = nullptr;

    QListView *m_interfaces;
    QFrame *m_details;

    QLabel *l_plugcable;

    QLabel *l_status;
    QLabel *m_status;
    QLabel *l_wifiname;
    QCheckBox *m_autoJoinWifi;
    QToolButton *m_wifi;

    QLabel *lbl_ipv4;
    QComboBox *m_ip4config;
    QLabel *lb_ip;
    QLineEdit *m_ipv4address;
    QLabel *lb_subnet;
    QLineEdit *m_subnet;
    QLabel *lb_gateway;
    QLineEdit *m_gateway;
    QLabel *lb_dns;
    QLabel *m_dns;
    QLabel *lb_searchdom;
    QLabel *m_searchdom;
    QCheckBox *m_wifiStatus;
    QPushButton *m_advanced;
    QToolButton *m_help;
};

#endif // NETAPPLET_H
