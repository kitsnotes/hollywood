#ifndef STATPOLLER_H
#define STATPOLLER_H

#include <QThread>
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QTimer>

class StatPoller : public QObject
{
    Q_OBJECT
public:
    enum PollType {
        CPU,
        Memory,
        Disk,
        Network
    };
    explicit StatPoller(PollType type, QObject *parent = nullptr);
    ~StatPoller();
    void setDiskToMonitor(const QString &disk);
signals:
    void ramDataAvailable(qulonglong, qulonglong, qulonglong, qulonglong, qulonglong);
    void swapDataAvailable(qulonglong, qulonglong, qulonglong);
    void cpuAverageAvailable(float data);
    void diskDataAvailable(qulonglong,qulonglong);
public slots:
    void poll();
private:
    QString m_disk;
    QFile *m_file = nullptr;
    PollType m_type;
    QTimer *m_timer = nullptr;
    uint m_cpu_previdle = 0;
    uint m_cpu_prevtotal = 0;

    qulonglong m_disk_read_sector = 0;
    qulonglong m_disk_write_sector = 0;
};

#endif // STATPOLLER_H
