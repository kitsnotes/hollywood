#include "btapplet.h"

BluetoothApplet::BluetoothApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool BluetoothApplet::init()
{
    setupWidget();
    return true;
}

QString BluetoothApplet::id() const
{
    return QLatin1String("org.originull.bluetooth");
}

QString BluetoothApplet::name() const
{
    return tr("Bluetooth");
}

QString BluetoothApplet::description() const
{
    return tr("Manage Bluetooth devices");
}

QIcon BluetoothApplet::icon() const
{
    return QIcon::fromTheme("preferences-system-bluetooth");
}

QWidget *BluetoothApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category BluetoothApplet::category() const
{
    return System;
}

QStringList BluetoothApplet::searchTokens() const
{
    QStringList tokens;
    tokens << tr("Set up bluetooth keyboard");
    tokens << tr("Pair bluetooth device");

    return tokens;
}

void BluetoothApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("BluetoothApplet"));
    m_host->resize(625, 439);

    mainLayout = new QVBoxLayout(m_host);


}

