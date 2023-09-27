#ifndef APKUPDATEDAEMON_H
#define APKUPDATEDAEMON_H

#include <QObject>
#include <QCoreApplication>
#include <QTimer>
#include <QString>
#include <QtDBus/QDBusContext>
#include <ApkQt/QtApk>

class ApkUpdateDaemon : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.originull.updated")
public:
    explicit ApkUpdateDaemon(QObject *parent = nullptr);
public slots:
    void update();
    void upgrade(int flags);
    void closeWhenReady();
    void addPackage(QString package);
    void delPackage(QString package);
    void startHelper();
    bool isBusy() { return m_busy; }
signals:
    void transactionProgress(float progress);
    void transactionMessage(QString msg);
    void errorOccured(QString msg);
    void transactionFinished();
private:
    void touchUpdateConf();
private:
    bool m_busy = false;
    bool m_close = false;
    QtApk::DatabaseAsync *m_db = nullptr;
    QtApk::Transaction *m_currentTransaction = nullptr;
};

#endif // APKUPDATEDAEMON_H
