#include "apkd.h"
#include "updated_adaptor.h"
#include "updated_interface.h"
#include <QSettings>
#include <QDBusConnection>
#include <QDBusError>
#include <ApkQt/QtApkFlags.h>
#include <unistd.h>
#include <iostream>

ApkUpdateDaemon::ApkUpdateDaemon(QObject *parent)
    : QObject(parent)
{
    new UpdatedAdaptor(this);
}

void ApkUpdateDaemon::update()
{
    if(m_currentTransaction)
    {
        emit errorOccured(tr("Can not proceed with updating package index. Transaction already in progress"));
        qWarning() << tr("Can not proceed with updating package index. Transaction already in progress");
        return;
    }

    if(!m_db)
        m_db = new QtApk::DatabaseAsync();

    if(m_db->isOpen())
        m_db->close();

    m_db->open(QtApk::QTAPK_OPENF_READWRITE | QtApk::QTAPK_OPENF_ENABLE_PROGRESSFD);

    m_currentTransaction = m_db->updatePackageIndex();
    connect(m_currentTransaction, &QtApk::Transaction::finished, [this]() {
        m_currentTransaction->deleteLater();
        m_currentTransaction = nullptr;
    });
    connect(m_currentTransaction, &QtApk::Transaction::descChanged, [this]() {
        emit transactionMessage(m_currentTransaction->desc());
    });
    connect(m_currentTransaction, &QtApk::Transaction::progressChanged, [this](float percent) {
        emit transactionProgress(percent);
    });
    connect(m_currentTransaction, &QtApk::Transaction::errorOccured, [this](QString msg) {
        emit errorOccured(msg);
    });
    m_currentTransaction->start();
    touchUpdateConf();
}

void ApkUpdateDaemon::startHelper()
{
    if(!QDBusConnection::systemBus().registerService("org.originull.updated"))
    {
        qDebug() << QDBusConnection::systemBus().lastError().message();
        qWarning() << tr("Failed to register service org.originull.updated - "
                         "there may be another instance running.");
        QCoreApplication::instance()->exit(1);
    }

    if(!QDBusConnection::systemBus().registerObject("/", this))
    {
        qDebug() << tr("Failed to register updated object on the system bus.");
        QCoreApplication::instance()->exit(2);
    }
    m_db = new QtApk::DatabaseAsync();
}

void ApkUpdateDaemon::touchUpdateConf()
{
    QSettings settings("/etc/originull/update.conf");
    settings.beginGroup("UpdateDaemon");
    settings.setValue("LastUpdateCheck", QDateTime::currentDateTime().toSecsSinceEpoch());
}

void ApkUpdateDaemon::upgrade(int flags)
{
    Q_UNUSED(flags)
    if(m_currentTransaction)
    {
        emit errorOccured(tr("Can not proceed with package installation. Transaction already in progress"));
        qWarning() << tr("Can not proceed with upgrade. Transaction already in progress");
        return;
    }

    if(!m_db)
        m_db = new QtApk::DatabaseAsync();

    if(m_db->isOpen())
        m_db->close();

    m_db->open(QtApk::QTAPK_OPENF_READWRITE | QtApk::QTAPK_OPENF_ENABLE_PROGRESSFD);

    m_currentTransaction = m_db->upgrade();
    connect(m_currentTransaction, &QtApk::Transaction::finished, [this]() {
        m_currentTransaction->deleteLater();
        m_currentTransaction = nullptr;
    });
    connect(m_currentTransaction, &QtApk::Transaction::descChanged, [this]() {
        emit transactionMessage(m_currentTransaction->desc());
    });
    connect(m_currentTransaction, &QtApk::Transaction::progressChanged, [this](float percent) {
        emit transactionProgress(percent);
    });
    connect(m_currentTransaction, &QtApk::Transaction::errorOccured, [this](QString msg) {
        emit errorOccured(msg);
    });
    m_currentTransaction->start();
}

void ApkUpdateDaemon::addPackage(QString package)
{
    if(m_currentTransaction)
    {
        emit errorOccured(tr("Can not proceed with package installation. Transaction already in progress"));
        qWarning() << tr("Can not proceed with package installation. Transaction already in progress");
        return;
    }

    if(!m_db)
        m_db = new QtApk::DatabaseAsync();

    if(m_db->isOpen())
        m_db->close();

    m_db->open(QtApk::QTAPK_OPENF_READWRITE | QtApk::QTAPK_OPENF_ENABLE_PROGRESSFD);

    m_currentTransaction = m_db->add(package);
    connect(m_currentTransaction, &QtApk::Transaction::finished, [this]() {
        m_currentTransaction->deleteLater();
        m_currentTransaction = nullptr;
    });
    connect(m_currentTransaction, &QtApk::Transaction::descChanged, [this]() {
        emit transactionMessage(m_currentTransaction->desc());
    });
    connect(m_currentTransaction, &QtApk::Transaction::progressChanged, [this](float percent) {
        emit transactionProgress(percent);
    });
    connect(m_currentTransaction, &QtApk::Transaction::errorOccured, [this](QString msg) {
        emit errorOccured(msg);
    });
    m_currentTransaction->start();
}

void ApkUpdateDaemon::delPackage(QString package)
{
    if(m_currentTransaction)
    {
        emit errorOccured(tr("Can not proceed with package installation. Transaction already in progress"));
        qWarning() << tr("Can not proceed with package removal. Transaction already in progress");
        return;
    }

    if(!m_db)
        m_db = new QtApk::DatabaseAsync();

    if(m_db->isOpen())
        m_db->close();

    m_db->open(QtApk::QTAPK_OPENF_READWRITE | QtApk::QTAPK_OPENF_ENABLE_PROGRESSFD);

    m_currentTransaction = m_db->del(package);
    connect(m_currentTransaction, &QtApk::Transaction::finished, [this]() {
        m_currentTransaction->deleteLater();
        m_currentTransaction = nullptr;
    });
    connect(m_currentTransaction, &QtApk::Transaction::descChanged, [this]() {
        emit transactionMessage(m_currentTransaction->desc());
    });
    connect(m_currentTransaction, &QtApk::Transaction::progressChanged, [this](float percent) {
        emit transactionProgress(percent);
    });
    connect(m_currentTransaction, &QtApk::Transaction::errorOccured, [this](QString msg) {
        emit errorOccured(msg);
    });
    m_currentTransaction->start();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    bool root = false;
    if(getuid() == 0)
        root = true;

    if(!root)
    {
        std::cout << "This application must run as root.";
        return 1;
    }

    ApkUpdateDaemon apkd;
    apkd.startHelper();
    return a.exec();
}
