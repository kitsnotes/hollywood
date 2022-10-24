#include "cpuoverview.h"
#include "chartobject.h"
#include "statpoller.h"
#include "lscpu-arm.h"

#include "gaugeobject.h"

#include <QButtonGroup>

#include <string.h>
#include <QDir>
#include <QHash>
#include <QSettings>
#include <hollywood/hollywood.h>
#ifdef __amd64__
#include <cpuid.h>
#endif

CPUOverview::CPUOverview(QWidget *parent)
    : QWidget(parent),
      m_cpuchart(new ChartObject(ChartObject::CPU, this)),
      m_poll(new StatPoller(StatPoller::CPU, this)),
      vl_main(new QVBoxLayout(this)),
      hl_top_main(new QHBoxLayout),
      m_icon(new QLabel(this)),
      vl_hostname(new QVBoxLayout),
      m_cpuname(new QLabel(this)),
      m_cpuinfo(new QLabel(this)),
      m_speed(new QGraphicsView(this)),
      vl_graphtoggle(new QHBoxLayout),
      m_average(new QToolButton(this)),
      m_percpu(new QToolButton(this)),
      m_chart(new QChartView(m_cpuchart->chart(), this)),
      hl_infocenter(new QHBoxLayout),
      fl_details(new QHBoxLayout)
{
    m_cpuinfo->setIndent(15);

    vl_main->setSpacing(0);
    vl_main->setContentsMargins(2, 2, 2, 2);
    hl_top_main->setSizeConstraint(QLayout::SetDefaultConstraint);
    hl_top_main->setContentsMargins(5, -1, 5, -1);
    m_icon->setPixmap(QIcon::fromTheme("cpu").pixmap(QSize(48,48)));
    m_icon->setScaledContents(false);
    hl_top_main->addWidget(m_icon);
    vl_hostname->addWidget(m_cpuname);
    vl_hostname->addWidget(m_cpuinfo);
    vl_hostname->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    hl_top_main->addLayout(vl_hostname);
    hl_top_main->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl_top_main->addWidget(m_speed);
    hl_top_main->addLayout(vl_graphtoggle);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_speed->sizePolicy().hasHeightForWidth());
    m_speed->setSizePolicy(sizePolicy);
    m_speed->setMaximumSize(QSize(90, 55));
    m_speed->setFrameShape(QFrame::NoFrame);
    QPalette p = m_speed->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    m_speed->setPalette(p);
    vl_main->addLayout(hl_top_main);

    m_chart->setRenderHint(QPainter::Antialiasing);

    m_average->setText(tr("Average"));
    m_percpu->setText(tr("Per CPU"));
    m_average->setCheckable(true);
    m_average->setChecked(true);
    m_percpu->setCheckable(true);
    m_percpu->setDisabled(true);
    vl_graphtoggle->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    vl_graphtoggle->addWidget(m_average);
    vl_graphtoggle->addWidget(m_percpu);
    vl_graphtoggle->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Expanding,QSizePolicy::Fixed));

    // buttons to toggle between average / per cpu graphs
    auto bg = new QButtonGroup(this);
    bg->addButton(m_average);
    bg->addButton(m_percpu);

    auto vl_chart = new QVBoxLayout();
    vl_chart->setSpacing(0);
    vl_chart->setContentsMargins(0,0,0,0);
    vl_chart->addWidget(m_chart,1);
    vl_main->addLayout(vl_chart, 1);
    hl_infocenter->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    m_sockets = new QLabel(this);
    m_cores = new QLabel(this);
    m_vm = new QLabel(this);
    m_caches = new QLabel(this);

    hl_infocenter->addWidget(m_sockets);
    hl_infocenter->addWidget(m_cores);
    hl_infocenter->addWidget(m_vm);
    hl_infocenter->addWidget(m_caches);
    hl_infocenter->setSpacing(5);
    m_caches->setVisible(false);
    m_vm->setVisible(false);

    hl_infocenter->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    vl_main->addLayout(hl_infocenter);

    QFont font(m_cpuname->font());
    font.setPointSize(font.pointSize()+2);
    font.setBold(true);
    m_cpuname->setFont(font);
    m_cpuname->setText(tr("Unknown Processor"));
    m_cpuinfo->setText(tr("Unknown Speed"));

    m_speed->setVisible(true);

    m_vm->setText(tr("Virtual Machine:"));
    m_caches->setText(tr("L1 Cache:"));

    connect(m_poll, &StatPoller::cpuAverageAvailable, this, &CPUOverview::cpuAverageAvailable);
    getCpuidInfo();

    QList<QByteArray> socketid;
    for(auto &cpu : m_cpus)
    {
        if(!socketid.contains(cpu.physicalPackage))
            socketid.append(cpu.physicalPackage);
    }

    QMap<QByteArray, uint> coreCounts;
    for(auto &socket : socketid)
    {
        int corecount = 0;
        for(auto &cpu : m_cpus)
        {
            if(cpu.physicalPackage == socket)
            {
                auto sl = cpu.coreSiblings.split('-');
                uint last = sl.last().toUInt();
                uint first = sl.first().toUInt();
                corecount = last-first+1;
            }
        }
        coreCounts.insert(socket, corecount);
    }

    bool same = true;
    uint count = coreCounts.first();
    QMap<QByteArray, uint>::iterator i;
    for(i = coreCounts.begin(); i != coreCounts.end(); ++i)
    {
        if(i.value() == count)
            continue;

        same = false;
    }

    if(same)
        m_cores->setText(tr("Cores: %1").arg(count));
    else
    {
        // TODO: I don't think a dual socket system can have a differing core count
        // per CPU - but if this can happen let's handle it here.
        m_cores->setText(tr("Cores: ????"));
    }
    m_sockets->setText(tr("Sockets: %1").arg(socketid.count()));

    //m_speed->setVisible(false);

#ifdef __amd64__
    int cpuid_freq = 0;
    // lets do a cpuid
    uint eax, unused;
    __get_cpuid(0x16, &eax, &unused, &unused, &unused);
    cpuid_freq = eax;   // returned in mhz
    // check for cpufreq

    qDebug() << eax;
    QString arg = QString::number(eax);

    if(m_cpus.at(0).has_cpufreq)
    {
        m_cpufreq = true;
        m_cpuinfo->setText(tr("Base Speed: %1").arg(arg));
        // let's show our governor graph
        m_cpufreq_max_mhz = m_cpus.at(0).cpufreq_max/1000;
        m_cpufreq_min_mhz = m_cpus.at(0).cpufreq_min/1000;

        setupGovernorGraph();
    }
    else
        m_cpuinfo->setText(tr("Speed: %1").arg(arg));
#endif

    QSettings settings("originull", "hollywood");
    settings.beginGroup("General");
    auto colorstr = settings.value("AccentColor", HOLLYWOOD_ACCENT_BLUE).toString();
    auto color = QColor(colorstr);
    if(color.isValid())
        m_cpuchart->setChartAccentColor(color);
}

void CPUOverview::poll()
{
    m_poll->poll();
}

void CPUOverview::setGraphPollTimeTitle(const QString &title)
{
    m_cpuchart->updateXAxis(title);
}

void CPUOverview::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::PaletteChange)
    {
        QSettings settings("originull", "hollywood");
        settings.beginGroup("General");
        auto colorstr = settings.value("AccentColor", HOLLYWOOD_ACCENT_BLUE).toString();
        auto color = QColor(colorstr);
        if(color.isValid())
            m_cpuchart->setChartAccentColor(color);
    }
    QWidget::changeEvent(event);
}

void CPUOverview::cpuAverageAvailable(float data)
{
    m_cpuchart->pushSample(data);
}

void CPUOverview::setupGovernorGraph()
{
    m_speed->setVisible(true);
    m_scene = new QGraphicsScene(0,0,50,50);
    m_speed->setScene(m_scene);

    m_speedGauge = new GPUGauge(QRectF(0,0,50,50));
    m_scene->addItem(m_speedGauge);
}

void CPUOverview::getCpuidInfo()
{
    QFile f("/proc/cpuinfo");
    if(f.open(QFile::ReadOnly))
    {
        QList<QByteArray> data = f.readAll().split('\n');
        f.close();
        QList<QByteArray> lines;
        for(auto &line : data)
        {
            if(line.isEmpty())
            {
                processCpuidBlock(lines);
                lines.clear();
            }
            lines.append(line);
        }
        if(!lines.isEmpty())
            processCpuidBlock(lines);
    }
    if(m_cpus.count() > 0)
    {
        auto mn = m_cpus[0].modelname;
        if(mn.startsWith("Cortex"))
            mn.prepend("ARM ");
        QString cpu = QString("%1")
                .arg(QString(mn));
        m_cpuname->setText(cpu);
    }
}

void CPUOverview::processCpuidBlock(QList<QByteArray> d)
{
    CpuID cpu;
    for(auto &l : d)
    {
        auto pair = l.split(':');
        if(pair.count() < 2)
            continue;

        auto key = pair[0].trimmed().toLower();
        auto val = pair[1].trimmed();

        if(key == "processor")
            cpu.processor = val.toUInt();

        if(key == "model name")
            cpu.modelname = val.data();

        if(key == "cpu mhz")
            cpu.mhz = val.toFloat();

        if(key == "bogomips")
            cpu.mhz = val.toFloat();

        if(key == "vendor_id")
            cpu.x86vendor = val.data();

        if(key == "cache size")
            cpu.cache = val.data();

        if(key == "stepping")
            cpu.stepping = val.data();

        if(key == "cpu part")
            cpu.model = val.data();

        if(key == "cpu implementer")
            cpu.vendor = val.data();

        if(key == "cpu revision")
            cpu.revision = val.toInt();

        if(key == "cpu variant")
            cpu.stepping = val.data();

        if(key == "cpu cores")
            cpu.cores = val.toInt();

    }

    QFile ppid(QString("/sys/devices/system/cpu/cpu%1/topology/physical_package_id").arg(cpu.processor));
    if(ppid.open(QFile::ReadOnly))
    {
        cpu.physicalPackage = ppid.readAll();
        ppid.close();
    }

    QFile siblings(QString("/sys/devices/system/cpu/cpu%1/topology/core_siblings_list").arg(cpu.processor));
    if(siblings.open(QFile::ReadOnly))
    {
        cpu.coreSiblings = siblings.readAll();
        ppid.close();
    }

    if(!cpu.vendor.isEmpty())
        arm_cpu_decode(&cpu);

    // see if we have cpufreq
    QFile cpufreq(QString("/sys/devices/system/cpu/cpufreq/policy%1/cpuinfo_min_freq").arg(cpu.processor));
    if(cpufreq.open(QFile::ReadOnly))
    {
        cpu.has_cpufreq = true;
        auto speed = QString(cpufreq.readAll().trimmed()).toUInt();
        cpu.cpufreq_min = speed;
    }
    QFile cpufreqmax(QString("/sys/devices/system/cpu/cpufreq/policy%1/cpuinfo_max_freq").arg(cpu.processor));
    if(cpufreqmax.open(QFile::ReadOnly))
    {
        cpu.has_cpufreq = true;
        auto speed = QString(cpufreqmax.readAll().trimmed()).toUInt();
        cpu.cpufreq_max = speed;
    }
    m_cpus.append(cpu);
}

void CPUOverview::arm_cpu_decode(struct CpuID *desc)
{
    // taken from lscpu as implemented by Riku Voipio
    // https://suihkulokki.blogspot.com/2018/02/making-sense-of-proccpuinfo-on-arm.html
    // https://github.com/util-linux/util-linux/pull/564/files
    int j, impl, part;
    const struct id_part *parts = NULL;
    char buf[8];
    if (desc->vendor.isEmpty() || desc->model.isEmpty())
        return;
    if ((strncmp(desc->vendor.data(),"0x",2) ||
         strncmp(desc->model.data(),"0x",2) ))
        return;

    impl=(int)strtol(desc->vendor, NULL, 0);
    part=(int)strtol(desc->model, NULL, 0);

    for (j = 0; hw_implementer[j].id != -1; j++) {
    if (hw_implementer[j].id == impl) {
        parts = hw_implementer[j].parts;
        desc->vendor = QByteArray(hw_implementer[j].name);
        break;
        }
    }
    if ( parts == NULL)
        return;

    for (j = 0; parts[j].id != -1; j++) {
    if (parts[j].id == part) {
        desc->modelname = QByteArray(parts[j].name);
        break;
        }
    }

    /* Print out the rXpY string for ARM cores */
    if (impl == 0x41 && desc->revision != 0 &&
        desc->stepping.isEmpty())	{
        int variant = (int)strtol(desc->stepping, NULL, 0);
        snprintf(buf, sizeof(buf), "r%dp%d", variant, desc->revision );
        desc->stepping = strdup(buf);
    }
}
