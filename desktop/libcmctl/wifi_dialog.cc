#include "private/wifi_dialog_p.h"

HWCM::WifiDialog::WifiDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.label_icon->setPixmap(QIcon::fromTheme("network-wireless").pixmap(QSize(64,64)));
    auto font = ui.label_action->font();
    font.setBold(true);
    ui.label_action->setFont(font);
    setWindowTitle(QString(""));
    ui.toolButton_whatsthis->setIcon(QIcon::fromTheme("help-contextual"));
    ui.toolButton_whatsthis->setIconSize(QSize(20,20));
    ui.lineEdit_password->setEchoMode(QLineEdit::Password);

    connect (ui.cb_show_pw, &QCheckBox::toggled, this, &WifiDialog::showPassphrase);
    connect (ui.checkBox_wps_no_pin, &QCheckBox::toggled, this, &WifiDialog::useWPSPushButton);

    connect(ui.pushButton_accept, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui.pushButton_cancel, &QPushButton::clicked, this, &QDialog::reject);
}

int HWCM::WifiDialog::showWithMap(QMap<QString, QString> input_map)
{
    hideElements();

    auto ssid = input_map.value("Name");

    // A standard WPA2/3 Network with a passphrase
    if (input_map.contains("Passphrase"))
    {
        m_radius = false;
        ui.cb_show_pw->setVisible(true);
        ui.label_password->setVisible(true);
        ui.lineEdit_password->setVisible(true);
        ui.lineEdit_password->setText(input_map.value("Passphrase"));
        ui.label_action->setVisible(true);
        ui.label_action->setText(tr("The wireless network \"%1\" requires a password.").arg(ssid));
    }

    if (input_map.contains("PreviousPassphrase"))
    {
        if(ui.lineEdit_password->text().isEmpty())
            ui.lineEdit_password->setText(input_map.value("PreviousPassphrase"));
    }

    // Name is requested SSID (the SSID field is for bytes)
    if (input_map.contains("Name"))
    {
        m_hidden = true;
        ui.label_ssid->setVisible(true);
        ui.lineEdit_ssid->setVisible(true);
        ui.lineEdit_ssid->setText(input_map.value("Name"));
        ui.label_action->setVisible(true);
        ui.label_description->setVisible(true);
        ui.label_action->setText(tr("Connect to a Hidden Network."));
        ui.label_action->setText(tr("Enter the network name (SSID) as well as the passphrase."));
    }

    // This is for RADIUS
    if (input_map.contains("Identity"))
    {
        m_radius = true;
        ui.lineEdit_username->setVisible(true);
        ui.lineEdit_username->setVisible(true);
        ui.lineEdit_username->setText(input_map.value("Identity"));
        ui.label_action->setVisible(true);
        ui.label_action->setText(tr("The wireless network \"%1\" requires a username and password.").arg(ssid));
    }

    // This is for WISPr
    if (input_map.contains("Userame"))
    {
        m_wispr = true;
        ui.label_username->setVisible(true);
        ui.lineEdit_username->setVisible(true);
        ui.lineEdit_username->setText(input_map.value("Username"));
        ui.label_action->setVisible(true);
        ui.label_action->setText(tr("The wireless network \"%1\" requires a username and password.").arg(ssid));
    }
    // This is for WISPr
    // This will never be used in conjunction with 'passphrase' above
    if (input_map.contains("Password"))
    {
        m_wispr = true;
        ui.label_password->setVisible(true);
        ui.lineEdit_password->setVisible(true);
        ui.lineEdit_password->setText(input_map.value("Password"));
        // This password is likely a domain/SSO password so lets
        // not show this in the open for security reasons
        ui.cb_show_pw->setVisible(false);
    }

    // This is for Wireless Protected Setup (ew)
    if (input_map.contains("WPS"))
    {
        ui.label_wps_pin->setVisible(true);
        ui.lineEdit_wps_pin->setVisible(true);
        ui.lineEdit_wps_pin->setText(input_map.value("WPS") );
        ui.checkBox_wps_no_pin->setVisible(true);
        ui.label_action->setVisible(true);
        ui.label_action->setText(tr("The wireless network \"%1\" requires a WPS PIN.").arg(ssid));
    }

    return this->exec();
}

void HWCM::WifiDialog::createDict(QMap<QString, QVariant> &r_map)
{
    // Initialize the map
    r_map.clear();

    // Create the dict entries

    // this is used for RADIUS as well as WPA
    if (! ui.lineEdit_password->text().isEmpty() && !m_wispr)
        r_map["Passphrase"] = ui.lineEdit_password->text();

    // Identity for RADIUS
    if (! ui.lineEdit_username->text().isEmpty() && m_radius)
        r_map["Identity"] = ui.lineEdit_username->text();

    if (! ui.lineEdit_ssid->text().isEmpty() && m_hidden)
        r_map["Name"] = ui.lineEdit_ssid->text();

    // add the WISPr username and PW
    if (! ui.lineEdit_username->text().isEmpty() && m_wispr)
        r_map["Username"] = ui.lineEdit_username->text();
    if (! ui.lineEdit_password->text().isEmpty() && m_wispr)
        r_map["Password"] = ui.lineEdit_password->text();

    // WPS?
    if (! ui.lineEdit_wps_pin->text().isEmpty())
        r_map["WPS"] = ui.lineEdit_wps_pin->text();

    return;
}

void HWCM::WifiDialog::showPassphrase(bool show)
{
    if(show)
        ui.lineEdit_password->setEchoMode(QLineEdit::Password);
    else
        ui.lineEdit_password->setEchoMode(QLineEdit::Normal);
}

void HWCM::WifiDialog::useWPSPushButton(bool checked)
{
    if (checked) ui.lineEdit_wps_pin->clear();
    ui.lineEdit_wps_pin->setDisabled(checked);
}

void HWCM::WifiDialog::hideElements()
{
    m_hidden = false;
    m_radius = false;

    ui.label_action->setVisible(false);
    ui.label_description->setVisible(false);

    ui.label_ssid->setVisible(false);
    ui.lineEdit_ssid->setVisible(false);

    ui.label_username->setVisible(false);
    ui.lineEdit_username->setVisible(false);

    ui.label_password->setVisible(false);
    ui.lineEdit_password->setVisible(false);

    ui.label_wps_pin->setVisible(false);
    ui.lineEdit_wps_pin->setVisible(false);

    ui.checkBox_wps_no_pin->setVisible(false);
    ui.cb_show_pw->setVisible(false);
}
