#include "ramoverview.h"
#include "statpoller.h"
#include "chartobject.h"
#include <QDir>
#include <QHash>
#include <QSettings>
#include <hollywood/hollywood.h>

RAMOverview::RAMOverview(QWidget *parent)
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
    QFile file("/proc/meminfo");
    file.open(QFile::ReadOnly);
    auto mt_line = file.readLine();
    file.close();
    auto mt_total = mt_line.split(':').last().trimmed().split(' ').first().toULongLong();


    m_ramchart = new ChartObject(ChartObject::RAM, this, mt_total);
    m_poll = new StatPoller(StatPoller::Memory, this);
    m_chart = new QChartView(m_ramchart->chart(), this);
    vl_main->setSpacing(0);
    vl_main->setContentsMargins(2, 2, 2, 2);

    m_chart->setRenderHint(QPainter::Antialiasing);

    // build our top bar
    m_icon->setPixmap(QIcon::fromTheme("media-flash").pixmap(QSize(48,48)));
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
    m_cpuname->setText(tr("%1 System Memory").arg(QLocale::system().formattedDataSize(mt_total*1024)));

    QSettings settings("originull", "hollywood");
    settings.beginGroup("General");
    auto colorstr = settings.value("AccentColor", HOLLYWOOD_ACCENT_BLUE).toString();
    auto color = QColor(colorstr);
    if(color.isValid())
        m_ramchart->setChartAccentColor(color);

    connect(m_poll, &StatPoller::ramDataAvailable, this, &RAMOverview::ramDataAvailable);
}

void RAMOverview::poll()
{
    m_poll->poll();
}

void RAMOverview::setGraphPollTimeTitle(const QString &title)
{
    m_ramchart->updateXAxis(title);
}

void RAMOverview::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::PaletteChange)
    {
        QSettings settings("originull", "hollywood");
        settings.beginGroup("General");
        auto colorstr = settings.value("AccentColor", HOLLYWOOD_ACCENT_BLUE).toString();
        auto color = QColor(colorstr);
        if(color.isValid())
            m_ramchart->setChartAccentColor(color);
    }
    QWidget::changeEvent(event);
}

void RAMOverview::ramDataAvailable(qulonglong total, qulonglong free, qulonglong avail, qulonglong buf, qulonglong cache)
{
    m_ramchart->pushMemory(total,free,avail,buf,cache);
}
