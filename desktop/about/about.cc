#include "about.h"
#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QSvgRenderer>
#include <QFile>
#include <QStorageInfo>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <unistd.h>
#include <sys/utsname.h>

#include <GLES3/gl3.h>

#include "../sysmon/include/lscpu-arm.h"

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent)
    , m_lightBg(new QPixmap(700,480))
    , m_darkBg(new QPixmap(700,480))
{
    setMaximumSize(700,480);
    setMinimumSize(700,480);
    setWindowTitle(tr("About This Computer"));

    QSvgRenderer renderer(this);
    QPainter pl(m_lightBg);
    QPainter pd(m_darkBg);

    QRect r(0,0,700,480);
    renderer.load(QString(":/BGLight"));
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(&pl, r);
    renderer.load(QString(":/BGDark"));
    renderer.render(&pd, r);

    setupUi();
    /*QPixmap logo(230,52);
    QPainter pmp(&logo);
    renderer.load(QString(":/Logo"));
    renderer.render(&pmp, QRect(10,10,220,42));
    logo_icon->setPixmap(logo);*/
    QPixmap orl(":/Originull");
    orl = orl.scaledToWidth(220,Qt::SmoothTransformation);
    logo_icon->setPixmap(orl);

    QFont font = hw_train->font();
    font.setPixelSize(35);
    hw_train->setFont(font);
    font.setBold(true);
    hollywood_label->setFont(font);

    populateKernel();
    populateApi();
    populateSMBIOS();

    populateCPUID();
    populateMemory();
    populateSysDisk();
    populateGPU();
    populateDeviceTree();

    if(m_proc->text().isEmpty())
        m_proc->setText(tr("Unknown CPU"));

    if(m_hardware->text().isEmpty())
        m_hardware->setText(tr("Generic System"));
}

void AboutWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPalette pal = qApp->palette();
    auto tc = pal.color(QPalette::Active, QPalette::WindowText);

    bool dark = false;
    if(tc.name() == "#ffffff")
    {
        qDebug() << "dark mode";
        dark = true;
    }
    if(dark)
    {
        QPainter p(this);
        p.drawPixmap(QPointF(0,0), *m_darkBg, QRectF(0,0,700,480));
    }
    else
    {
        QPainter p(this);
        p.drawPixmap(QPointF(0,0), *m_lightBg, QRectF(0,0,700,480));
    }
}

void AboutWindow::setupUi()
{
    vl_main = new QVBoxLayout(this);
    hl_branding = new QHBoxLayout();
    logo_icon = new QLabel(this);
    hollywood_label = new QLabel(this);
    hw_train = new QLabel(this);
    auto hl_boxspacer = new QHBoxLayout();
    auto hs_logo = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vs_brand = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Maximum);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(logo_icon->sizePolicy().hasHeightForWidth());
    logo_icon->setSizePolicy(sizePolicy);
    sizePolicy.setHeightForWidth(hollywood_label->sizePolicy().hasHeightForWidth());
    hollywood_label->setSizePolicy(sizePolicy);
    sizePolicy.setHeightForWidth(hw_train->sizePolicy().hasHeightForWidth());
    hw_train->setSizePolicy(sizePolicy);

    hl_branding->addWidget(logo_icon);
    hl_branding->addWidget(hollywood_label);
    hl_branding->addWidget(hw_train);
    hl_branding->addItem(hs_logo);

    vl_main->addLayout(hl_branding);
    vl_main->addItem(vs_brand);

    auto hsbr = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    l_hwsh = new QLabel(this);
    m_shellVersion = new QLabel(this);
    l_kv = new QLabel(this);
    m_kernelVersion = new QLabel(this);
    l_api = new QLabel(this);
    m_apiVersions = new QLabel(this);
    l_opengl = new QLabel(this);
    m_opengl = new QLabel(this);

    auto line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    m_shellVersion->setText(qApp->applicationVersion());

    l_hw = new QLabel(this);
    m_hardware = new QLabel(this);
    l_proc = new QLabel(this);
    m_proc = new QLabel(this);
    l_mem = new QLabel(this);
    m_memory = new QLabel(this);
    l_sysdisk = new QLabel(this);
    m_systemdisk = new QLabel(this);
    l_gpu = new QLabel(this);
    m_gpu = new QLabel(this);

    auto hs = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto vs = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    fl_versions = new QFormLayout();

    fl_versions->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_versions->setWidget(0, QFormLayout::LabelRole, l_hwsh);
    fl_versions->setWidget(0, QFormLayout::FieldRole, m_shellVersion);
    fl_versions->setWidget(1, QFormLayout::LabelRole, l_kv);
    fl_versions->setWidget(1, QFormLayout::FieldRole, m_kernelVersion);
    fl_versions->setWidget(2, QFormLayout::LabelRole, l_api);
    fl_versions->setWidget(2, QFormLayout::FieldRole, m_apiVersions);
    fl_versions->setWidget(3, QFormLayout::LabelRole, l_opengl);
    fl_versions->setWidget(3, QFormLayout::FieldRole, m_opengl);

    fl_versions->setWidget(4, QFormLayout::SpanningRole, line);
    fl_versions->setWidget(5, QFormLayout::LabelRole, l_hw);
    fl_versions->setWidget(5, QFormLayout::FieldRole, m_hardware);
    fl_versions->setWidget(6, QFormLayout::LabelRole, l_proc);
    fl_versions->setWidget(6, QFormLayout::FieldRole, m_proc);
    fl_versions->setWidget(7, QFormLayout::LabelRole, l_mem);
    fl_versions->setWidget(7, QFormLayout::FieldRole, m_memory);
    fl_versions->setWidget(8, QFormLayout::LabelRole, l_gpu);
    fl_versions->setWidget(8, QFormLayout::FieldRole, m_gpu);
    fl_versions->setWidget(9, QFormLayout::LabelRole, l_sysdisk);
    fl_versions->setWidget(9, QFormLayout::FieldRole, m_systemdisk);

    hl_boxspacer->addItem(hsbr);
    hl_boxspacer->addLayout(fl_versions);
    hl_boxspacer->addItem(hs);

    lbl_copyright = new QLabel(this);
    lbl_copyright->setTextFormat(Qt::RichText);
    lbl_copyright->setAlignment(Qt::AlignCenter);
    lbl_copyright->setWordWrap(true);

    vl_main->addLayout(hl_boxspacer);
    vl_main->addItem(vs);
    vl_main->addWidget(lbl_copyright);

    hl_buttons = new QHBoxLayout();
    m_sysreport = new QPushButton(this);
    hl_buttons->addWidget(m_sysreport);
    m_update = new QPushButton(this);
    hl_buttons->addWidget(m_update);
    m_rights = new QPushButton(this);
    hl_buttons->addWidget(m_rights);
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hl_buttons->addItem(horizontalSpacer);
    m_help = new QPushButton(this);
    hl_buttons->addWidget(m_help);
    vl_main->addLayout(hl_buttons);

    hollywood_label->setText(QCoreApplication::translate("AboutWindow", "Hollywood", nullptr));
    hw_train->setText(QCoreApplication::translate("AboutWindow", "Edge", nullptr));
    l_hwsh->setText(QCoreApplication::translate("AboutWindow", "Shell:", nullptr));
    l_kv->setText(QCoreApplication::translate("AboutWindow", "Kernel:", nullptr));
    l_api->setText(QCoreApplication::translate("AboutWindow", "API:", nullptr));
    l_opengl->setText(QCoreApplication::translate("AboutWindow", "OpenGL:", nullptr));
    l_hw->setText(QCoreApplication::translate("AboutWindow", "Hardware:", nullptr));
    l_proc->setText(QCoreApplication::translate("AboutWindow", "Processor:", nullptr));
    l_mem->setText(QCoreApplication::translate("AboutWindow", "Memory:", nullptr));
    l_gpu->setText(QCoreApplication::translate("AboutWindow", "Graphics:", nullptr));
    l_sysdisk->setText(QCoreApplication::translate("AboutWindow", "System Disk:", nullptr));
    lbl_copyright->setText(QCoreApplication::translate("AboutWindow", "&copy; 2022 Originull Software and Contributors to the Hollywood Operating System. Hollywood is free software; you have rights available to you under license. ", nullptr));
    m_sysreport->setText(QCoreApplication::translate("AboutWindow", "Full System Report...", nullptr));
    m_update->setText(QCoreApplication::translate("AboutWindow", "Software Update...", nullptr));
    m_rights->setText(QCoreApplication::translate("AboutWindow", "About My Rights...", nullptr));
    m_help->setText(QCoreApplication::translate("AboutWindow", "Get Help...", nullptr));
}

void AboutWindow::populateKernel()
{
    struct utsname unb;
    if(uname(&unb) != 0)
        m_kernelVersion->setText(tr("Error in uname lookup"));
    else
        m_kernelVersion->setText(unb.release);
}

void AboutWindow::populateApi()
{
    m_apiVersions->setText(QLatin1String(QT_VERSION_STR));
}

void AboutWindow::populateSMBIOS()
{
    QString productName, sysVendor;
    QFile f("/sys/class/dmi/id/product_name");
    if(f.open(QFile::ReadOnly))
    {
        productName = f.readAll().trimmed();
        f.close();
    }

    QFile f2("/sys/class/dmi/id/sys_vendor");
    if(f2.open(QFile::ReadOnly))
    {
        sysVendor = f2.readAll().trimmed();
        f2.close();
    }

    if(productName.contains(sysVendor, Qt::CaseInsensitive))
        m_hardware->setText(productName);
    else
        m_hardware->setText(QString("%1 %2").arg(sysVendor, productName));
}

void AboutWindow::populateCPUID()
{
    QFile f("/proc/cpuinfo");
    if(f.open(QFile::ReadOnly))
    {
        QList<QByteArray> data = f.readAll().split('\n');
        f.close();
        QList<QByteArray> lines;
        for(auto line : data)
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
        m_proc->setText(cpu);
    }
}

void AboutWindow::processCpuidBlock(QList<QByteArray> d)
{
    CpuID cpu;
    for(auto l : d)
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

    QFile ppid(QString("/sys/devices/system/cpu%1/topology/physical_package_id").arg(cpu.processor));
    if(ppid.open(QFile::ReadOnly))
    {
        cpu.physicalPackage = ppid.readAll();
        ppid.close();
    }

    QFile siblings(QString("/sys/devices/system/cpu%1/topology/core_siblings_list").arg(cpu.processor));
    if(siblings.open(QFile::ReadOnly))
    {
        cpu.coreSiblings = siblings.readAll();
        ppid.close();
    }

    if(!cpu.vendor.isEmpty())
        arm_cpu_decode(&cpu);

    m_cpus.append(cpu);
}

void AboutWindow::arm_cpu_decode(struct CpuID *desc)
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

void AboutWindow::populateMemory()
{
    QFile file("/proc/meminfo");
    file.open(QFile::ReadOnly);
    auto mt_line = file.readLine();
    file.close();
    auto mt_total = mt_line.split(':').last().trimmed().split(' ').first().toULongLong();
    m_memory->setText(QLocale::system().formattedDataSize(mt_total*1024));
}

void AboutWindow::populateSysDisk()
{
    QStorageInfo si = QStorageInfo::root();
    bool is_ssd = false;

    auto sz = QLocale::system().formattedDataSize(si.bytesTotal());
    auto free = QLocale::system().formattedDataSize(si.bytesAvailable());
    QString system = QString("%1 %2 (%3)\n%4 available")
            .arg(sz, is_ssd ? tr("solid state disk") : tr("hard disk"),
                 si.fileSystemType(), free);
    m_systemdisk->setText(system);
}

void AboutWindow::populateDeviceTree()
{
    // for things like pi, etc this should be here
    QFile f("/sys/firmware/devicetree/base/model");
    if(f.open(QFile::ReadOnly))
    {
        auto data = f.readAll();
        if(m_hardware->text().isEmpty())
            m_hardware->setText(data);
        f.close();
        return;
    }

    // that doesn't exist lets see if we're in KVM/QEMU
    QFile f2("/sys/firmware/devicetree/base/compatible");
    if(f2.open(QFile::ReadOnly))
    {
        auto data = f2.readAll();
        f2.close();
        if(data.contains("dummy-virt"))
        {
            if(m_hardware->text().isEmpty())
                m_hardware->setText(tr("QEMU Virtual Machine"));

            if(m_proc->text().isEmpty())
                m_proc->setText(tr("Virtual 64-Bit ARM"));

            return;
        }
    }
    // lets check for apple virtualization
    // that doesn't exist lets see if we're in KVM/QEMU
    QFile f3("/sys/firmware/devicetree/base/hypervisor/compatible");
    if(f3.open(QFile::ReadOnly))
    {
        auto data = f3.readAll();
        f3.close();
        if(data.contains("apple,virt"))
        {
            if(m_hardware->text().isEmpty())
                m_hardware->setText(tr("Apple Virtual Machine"));

            if(m_proc->text().isEmpty())
                m_proc->setText(tr("Apple Silicon (Virtual M1x/M2 CPU)"));
            return;
        }
    }
}

void AboutWindow::populateGPU()
{
    QWindow surface;
    surface.setSurfaceType(QSurface::OpenGLSurface);
    surface.create();
    QOpenGLContext ctx;
    ctx.create();
    ctx.makeCurrent(&surface);

    ctx.functions()->initializeOpenGLFunctions();

    const char* renderer = reinterpret_cast<const char*>(ctx.functions()->glGetString(GL_RENDERER));
    const char* version = reinterpret_cast<const char*>(ctx.functions()->glGetString(GL_VERSION));
    m_opengl->setText(version);
    m_gpu->setText(renderer);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion("1.0");
    AboutWindow w;
    w.show();
    return a.exec();
}
