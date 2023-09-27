#include "audioapplet.h"

UsersGroupsApplet::UsersGroupsApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool UsersGroupsApplet::init()
{
    setupWidget();
    return true;
}

QString UsersGroupsApplet::id() const
{
    return QLatin1String("org.originull.audio");
}

QString UsersGroupsApplet::name() const
{
    return tr("Audio");
}

QString UsersGroupsApplet::description() const
{
    return tr("System software update");
}

QIcon UsersGroupsApplet::icon() const
{
    return QIcon::fromTheme("preferences-desktop-sound");
}

QWidget *UsersGroupsApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category UsersGroupsApplet::category() const
{
    return System;
}

QStringList UsersGroupsApplet::searchTokens() const
{
    QStringList tokens;
    tokens << tr("Check for software updates");
    tokens << tr("Set up automatic updates");

    return tokens;
}

void UsersGroupsApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("UsersGroups"));
    m_host->resize(625, 439);

    mainLayout = new QVBoxLayout(m_host);


}

