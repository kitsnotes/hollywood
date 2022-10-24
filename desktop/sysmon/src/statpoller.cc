#include "statpoller.h"

#include <QVariant>

StatPoller::StatPoller(PollType type, QObject *parent)
    : QObject(parent),
      m_file(new QFile(this)),
      m_type(type)
{
    if(m_type == CPU)
        m_file->setFileName("/proc/stat");
    if(m_type == Memory)
        m_file->setFileName("/proc/meminfo");
    if(m_type == Disk)
        m_file->setFileName("/proc/diskstats");
}

StatPoller::~StatPoller()
{
    if(m_file)
    {
        if(m_file->isOpen())
            m_file->close();

        delete m_file;
    }
}

void StatPoller::setDiskToMonitor(const QString &disk)
{
    m_disk = disk;

    qDebug() << "disk set to" << disk;
}

void StatPoller::poll()
{
    if(m_type == CPU)
    {
        m_file->open(QFile::ReadOnly);
        auto cpudata = m_file->readLine().simplified().split(' ').mid(1);
        int wait = cpudata[3].toInt();
        int total = 0;
        for(auto &time : cpudata)
            total += time.toInt();

        auto cpu = (1 - (1.0*wait-m_cpu_previdle)
                    / (total-m_cpu_prevtotal)) * 100.0;

        emit cpuAverageAvailable(cpu);

        m_cpu_previdle = wait;
        m_cpu_prevtotal = total;
        m_file->close();
        return;
    }
    if(m_type == Memory)
    {
        qulonglong mem_total = 0;
        qulonglong mem_free = 0;
        qulonglong mem_avail = 0;
        qulonglong buffers = 0;
        qulonglong cached = 0;
        qulonglong swcached = 0;

        qulonglong swaptotal = 0;
        qulonglong swapfree = 0;
        if(m_file->open(QFile::ReadOnly))
        {
            m_file->seek(0);
            int count = 0;
            for(;;)
            {
                auto ln = m_file->readLine();
                auto pc = ln.split(':');

                // done reading file here
                if(pc.first() == "Slab")
                    break;

                count++;
                if(count > 50)
                    break;

                auto f = pc.first();
                auto val = pc.last().trimmed().split(' ').first(); // remove the kB

                if(f == "MemTotal")
                    mem_total = QVariant(val).toULongLong();
                if(f == "MemFree")
                    mem_free = QVariant(val).toULongLong();
                if(f == "MemAvailable")
                    mem_avail = QVariant(val).toULongLong();
                if(f == "Buffers")
                    buffers = QVariant(val).toULongLong();
                if(f == "Cached")
                    cached = QVariant(val).toULongLong();
                if(f == "SwapCached")
                    swcached = QVariant(val).toULongLong();
                if(f == "SwapTotal")
                    swaptotal = QVariant(val).toULongLong();
                if(f == "SwapFree")
                    swapfree = QVariant(val).toULongLong();

            }
            Q_EMIT ramDataAvailable(mem_total, mem_free, mem_avail, buffers, cached);
            Q_EMIT swapDataAvailable(swaptotal, swapfree, swcached);
            m_file->close();
        }
    }
    if(m_type == Disk)
    {
        if(m_file->open(QFile::ReadOnly))
        {
            m_file->seek(0);
            int count = 0;
            for(;;)
            {
                auto line = QString(m_file->readLine());
                if(!line.contains(m_disk))
                    continue;

                auto idx = line.indexOf(m_disk);
                line.remove(0, idx);
                line = line.trimmed();
                auto lp = line.split(' ');
                auto sectorr = QString(lp[3]).toULongLong();
                auto sectorw = QString(lp[7]).toULongLong();
                if(m_disk_read_sector != 0 && m_disk_write_sector != 0)
                {
                    auto read = sectorr-m_disk_read_sector;
                    auto write = sectorw-m_disk_write_sector;
                    Q_EMIT diskDataAvailable(read,write);

                }
                m_disk_read_sector = sectorr;
                m_disk_write_sector = sectorw;
                break;
                count++;
                if(count > 50)
                    break;
            }
            m_file->close();
        }
    }
}
