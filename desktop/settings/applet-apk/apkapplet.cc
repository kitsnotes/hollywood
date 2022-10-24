#include "apkapplet.h"

APKUpdateApplet::APKUpdateApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool APKUpdateApplet::init()
{
    setupWidget();
    checkForUpdates();
    return true;
}

QString APKUpdateApplet::id() const
{
    return QLatin1String("org.originull.swupdate");
}

QString APKUpdateApplet::name() const
{
    return tr("Software Update");
}

QString APKUpdateApplet::description() const
{
    return tr("System software update");
}

QIcon APKUpdateApplet::icon() const
{
    return QIcon::fromTheme("system-software-update");
}

QWidget *APKUpdateApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category APKUpdateApplet::category() const
{
    return System;
}

void APKUpdateApplet::setupWidget()
{
    m_host = new QWidget(0);
    if (m_host->objectName().isEmpty())
        m_host->setObjectName(QString::fromUtf8("SoftwareUpdate"));
    m_host->resize(625, 439);

    mainLayout = new QVBoxLayout(m_host);

    auto l_title = new QHBoxLayout();
    auto titleicon = new QLabel(m_host);
    titleicon->setMaximumSize(QSize(64,64));
    titleicon->setMinimumSize(QSize(64,64));
    titleicon->setPixmap(QIcon::fromTheme("system-software-update").pixmap(QSize(64,64)));

    auto l_ttext = new QVBoxLayout();
    auto titletext = new QLabel(m_host);
    titletext->setText(tr("Software Update downloads and installs Hollywood system & Application Market updates from Originull servers."));
    auto titlesubtext = new QLabel(m_host);
    titlesubtext->setText(tr("Usage of software update servers is subject to the Originull Privacy Policy."));
    auto f = titlesubtext->font();
    f.setPointSize(f.pointSize()-2);
    titlesubtext->setFont(f);
    l_ttext->addWidget(titletext, 0, Qt::AlignTop);
    l_ttext->addWidget(titlesubtext, 0, Qt::AlignTop);
    l_ttext->setContentsMargins(0,0,0,0);
    l_ttext->setSpacing(0);
    l_title->addWidget(titleicon);
    l_title->addLayout(l_ttext, 1);

    auto hl_main = new QHBoxLayout();
    auto vl_main = new QVBoxLayout();
    lb_updates = new QLabel(m_host);
    auto vl_leftbtns = new QVBoxLayout();
    m_update = new QPushButton(m_host);
    m_select = new QPushButton(m_host);
    lb_desc = new QLabel(m_host);

    vl_main->addWidget(lb_updates);
    vl_main->addWidget(lb_desc);
    vl_main->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hl_main->addLayout(vl_main);
    vl_leftbtns->addWidget(m_update);
    vl_leftbtns->addWidget(m_select);
    vl_leftbtns->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hl_main->addLayout(vl_leftbtns);
    hl_main->setStretch(0, 1);

    mainLayout->addLayout(l_title);
    mainLayout->addLayout(hl_main);
    mainLayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));
}

void APKUpdateApplet::checkForUpdates()
{
    QtApk::DatabaseAsync apkdb;

    if (!apkdb.open()) {
        qWarning() << "Failed to open apk database!";
        return;
    }

    if(apkdb.upgradeablePackagesCount() > 0)
    {
        lb_updates->setText(tr("Updates are available for your computer:"));
        lb_desc->setText(tr("Test"));
        m_update->setEnabled(true);
        m_update->setText(tr("Update Now"));
        m_select->setVisible(true);
        m_select->setEnabled(true);
    }
    else
    {
        lb_updates->setText(tr("Your system is up to date."));
        lb_desc->setText(tr("Last checked: %1"));
        m_update->setEnabled(true);
        m_update->setText(tr("Check Now"));
        m_select->setVisible(false);
        m_select->setEnabled(false);
    }
}

