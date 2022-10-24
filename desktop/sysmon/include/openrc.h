#ifndef OPENRCMONITOR_H
#define OPENRCMONITOR_H

#include <QObject>
#include <QMap>

// TODO: path scanning? eventually we are removing the /usr split
// ala fedora & others so these will change to /usr/bin/....
#define OPENRC_STATUS_BINARY        "/bin/rc-status"
#define OPENRC_SERVICE_BINARY       "/sbin/rc-service"

class QProcess;
class OpenRCMonitor;
class OpenRCProcess : public QObject
{
    Q_OBJECT
protected:
    friend class OpenRCMonitor;
    explicit OpenRCProcess(const QString &proc, QObject *parent = nullptr);
    void setStarted(const bool st);
    void addRunlevel(const QString &rl);
private slots:
    void readProcData();
public:
    QStringList runlevels() { return m_runlevels; }
    QString status() const;
    QString name() const;
    QString description() const;
private:
    bool m_started = false;
    QProcess *m_proc = nullptr;
    QString m_name;
    QString m_description;
    QString m_initpath;
    QList<QString> m_runlevels;
    QMap<QString,QString> m_extracmds;
    QMap<QString,QString> m_extracmds_started;
};

class OpenRCServiceModel;
class QProcess;
class OpenRCMonitor : public QObject
{
    Q_OBJECT
public:
    explicit OpenRCMonitor(QObject *parent = nullptr);
    static bool systemHasOpenRCInit();
    OpenRCServiceModel* model();
public slots:
    void refreshStatus();
private slots:
    void processFinished();
private:
    friend class OpenRCServiceModel;
    QProcess *m_statmon = nullptr;
    QMap<QString,OpenRCProcess*> m_processlist;
    OpenRCServiceModel *m_model = nullptr;
};

#endif // OPENRCMONITOR_H
