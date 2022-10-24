#include "diskoverview.h"
#include "statpoller.h"
#include "chartobject.h"
#include <QDir>
#include <QHash>
#include <QSettings>
#include <hollywood/hollywood.h>

DiskOverview::DiskOverview(QWidget *parent)
    : QWidget{parent}
    , vl_main(new QVBoxLayout(this))
    , hl_top_main(new QHBoxLayout)
    , m_icon(new QLabel(this))
    , vl_hostname(new QVBoxLayout)
    , m_cpuname(new QLabel(this))
    , m_cpuinfo(new QLabel(this))
    , vl_updates(new QVBoxLayout)
    , hl_top_graphs(new QHBoxLayout)
{
    m_diskchart = new ChartObject(ChartObject::Disk, this, 1000);
    m_poll = new StatPoller(StatPoller::Disk, this);
    m_chart = new QChartView(m_diskchart->chart(), this);
    vl_main->setSpacing(0);
    vl_main->setContentsMargins(2, 2, 2, 2);

    m_chart->setRenderHint(QPainter::Antialiasing);
    // build our top bar
    m_icon->setPixmap(QIcon::fromTheme("drive-harddisk-root").pixmap(QSize(48,48)));
    m_icon->setScaledContents(false);
    m_cpuinfo->setIndent(15);
    hl_top_main->setSizeConstraint(QLayout::SetDefaultConstraint);
    hl_top_main->setContentsMargins(5, -1, 5, -1);
    hl_top_main->addWidget(m_icon);
    vl_hostname->addWidget(m_cpuname);
    vl_hostname->addWidget(m_cpuinfo);
    //vl_hostname->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    hl_top_main->addLayout(vl_hostname);
    hl_top_main->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    vl_updates->addLayout(hl_top_graphs);
    hl_top_main->addLayout(vl_updates);
    vl_main->addLayout(hl_top_main);

    vl_main->addWidget(m_chart);

    //vl_main->addItem(new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    //vl_main->setStretch(2, 1);

    QFont font(m_cpuname->font());
    font.setPointSize(font.pointSize()+2);
    font.setBold(true);
    m_cpuname->setFont(font);
    m_cpuname->setText(tr("System Disk %1").arg("diskname"));

    auto disk = findRootDisk();
    QString mydisk = disk.split('/').last();
    if(mydisk.contains("nvme"))
    {
        // TODO: nvme - deal with namespaces
    }
    else
        mydisk = mydisk.remove(mydisk.length()-1, 1);

    m_poll->setDiskToMonitor(mydisk);
    m_disk_block_size = findDiskSectorSize(mydisk);
    connect(m_poll, &StatPoller::diskDataAvailable, this, &DiskOverview::diskDataAvailable);
}

void DiskOverview::poll()
{
    m_poll->poll();
}

void DiskOverview::setGraphPollTimeTitle(const QString &title)
{
    m_diskchart->updateXAxis(title);
}

void DiskOverview::diskDataAvailable(qulonglong read, qulonglong write)
{
    // this is all in bytes
    auto read_sz = read*m_disk_block_size;
    auto write_sz = write*m_disk_block_size;

    m_diskchart->pushDiskStat(read_sz/1024, write_sz/1024);
}

QString DiskOverview::findRootDisk()
{
    QFile f("/proc/mounts");
    if(f.open(QFile::ReadOnly))
    {
        auto lines = f.readAll().split('\n');
        f.close();
        for(auto &line : lines)
        {
            auto tokens = line.split(' ');
            if(tokens.count() < 2)
                continue;
            auto mount = tokens[1];
            auto disk = tokens[0];

            if(QString(mount) == QString("/"))
            {
                f.close();
                return QString(disk);
            }
        }
        f.close();
    }
    return QString();
}

uint DiskOverview::findDiskSectorSize(const QString &disk)
{
    QFile f(QString("/sys/block/%1/queue/logical_block_size").arg(disk));
    if(f.open(QFile::ReadOnly))
    {
        QString d(f.readAll());
        f.close();
        return d.trimmed().toUInt();
    }
    // Hasty Default
    return 512;
}
