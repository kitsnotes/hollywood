#include "apkapplet.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QtApkChangeset.h>
#include <QtApkFlags.h>

#include "details.h"

APKUpdateApplet::APKUpdateApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

bool APKUpdateApplet::init()
{
    m_dbus =
        new QDBusInterface("org.originull.updated", "/", "org.originull.updated",
                 QDBusConnection::systemBus(), this);

    setupWidget();
    connect(m_details, &QPushButton::pressed, [this]() {
        DetailsDialog *d = new DetailsDialog(nullptr);
        connect(d, &DetailsDialog::finished, d, &DetailsDialog::deleteLater);
        d->exec();
    });

    if(isCurrentlyRunning())
        connectForUpdates();
    else
    {
        m_dbus->call("update");
        checkForUpdates();
    }
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

QStringList APKUpdateApplet::searchTokens() const
{
    QStringList tokens;
    tokens << tr("Check for software updates");
    tokens << tr("Set up automatic updates");

    return tokens;
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
    m_details = new QPushButton(m_host);
    lb_desc = new QLabel(m_host);
    m_progress = new QProgressBar(m_host);

    m_details->setText(tr("&Details..."));
    vl_main->addWidget(lb_updates);
    vl_main->addWidget(m_progress);
    vl_main->addWidget(lb_desc);
    vl_main->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hl_main->addLayout(vl_main);
    vl_leftbtns->addWidget(m_update);
    vl_leftbtns->addWidget(m_details);
    vl_leftbtns->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hl_main->addLayout(vl_leftbtns);
    hl_main->setStretch(0, 1);
    hl_main->setContentsMargins(110,0,110,0);

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

    disconnect(m_update, &QPushButton::pressed, this, &APKUpdateApplet::startUpdates);
    disconnect(m_update, &QPushButton::pressed, this, &APKUpdateApplet::doUpdateCheck);

    if(apkdb.upgradeablePackagesCount() > 0)
    {
        lb_updates->setText(tr("Updates available:"));
        lb_desc->setText(tr("%1 package update%2 are available for your computer.").arg(QString::number(apkdb.upgradeablePackagesCount()), apkdb.upgradeablePackagesCount() == 1 ? "" : "s"));
        m_update->setEnabled(true);
        m_update->setVisible(true);
        m_update->setText(tr("&Update Now"));
        connect(m_update, &QPushButton::pressed, this, &APKUpdateApplet::startUpdates);
        m_details->setVisible(true);
        m_details->setEnabled(true);
        m_progress->setVisible(false);
    }
    else
    {
        lb_updates->setText(tr("Up to date:"));
        lb_desc->setText(tr("Last check for updates: %1").arg(QDateTime::currentDateTime().toString()));
        m_update->setEnabled(true);
        m_update->setVisible(true);
        m_update->setText(tr("&Check Now"));
        connect(m_update, &QPushButton::pressed, this, &APKUpdateApplet::doUpdateCheck);
        m_details->setVisible(false);
        m_details->setEnabled(false);
        m_progress->setVisible(false);
    }
}

bool APKUpdateApplet::isCurrentlyRunning()
{
    QDBusReply<bool> busy = m_dbus->call("isBusy");
    return busy.value();
}

bool APKUpdateApplet::connectForUpdates()
{
    m_details->setVisible(false);
    m_update->setVisible(false);
    m_progress->setVisible(true);
    QDBusConnection::systemBus().connect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionProgress", this,
                                         SLOT(transactionProgress(double)));
    QDBusConnection::systemBus().connect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionMessage", this,
                                         SLOT(transactionMessage(QString)));
    QDBusConnection::systemBus().connect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionFinished", this,
                                         SLOT(transactionFinished()));
    lb_updates->setText(tr("Applying Updates..."));
    lb_desc->setText(tr("Starting process..."));
    return true;
}

void APKUpdateApplet::transactionFinished()
{
    // TODO: check for error
    QDBusConnection::systemBus().disconnect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionProgress", this,
                                         SLOT(transactionProgress(double)));
    QDBusConnection::systemBus().disconnect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionMessage", this,
                                         SLOT(transactionMessage(QString)));
    QDBusConnection::systemBus().disconnect("org.originull.updated",
                                         "/", "org.originull.updated",
                                         "transactionFinished", this,
                                         SLOT(transactionFinished()));
    m_dbus->call("update");
    checkForUpdates();
}

void APKUpdateApplet::transactionProgress(double progress)
{
    m_progress->setValue((int)progress);
}

void APKUpdateApplet::transactionMessage(QString message)
{
    lb_desc->setText(message);
}

void APKUpdateApplet::startUpdates()
{
    connectForUpdates();
    m_dbus->asyncCall("upgrade", 0);
}

void APKUpdateApplet::doUpdateCheck()
{
    m_dbus->call("update");
    checkForUpdates();
}
