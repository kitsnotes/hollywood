#include "infodialog.h"
#include <application.h>

#include <QFile>
#include <sys/stat.h>
#include <QRegularExpression>

#include <signal.h>

ProcessInfoDialog::ProcessInfoDialog(pid_t process, const QString &icon, QWidget *parent)
    : QDialog(parent)
    , m_process(process)
    , m_icon(icon)
    , mainLayout(new QVBoxLayout(this))
    , lb_icon(new QLabel(this))
    , lb_procname(new QLabel(this))
    , m_cmdline(new QLineEdit(this))
    , m_cwd(new QLineEdit(this))
    , m_tabs(new QTabWidget(this))
    , m_env(new QTableView(this))
    , m_files(new QTableView(this))
    , m_memory(new QTableView(this))
    , m_help(new QToolButton(this))
    , m_signal(new QToolButton(this))
    , m_kill(new QPushButton(this))
    , m_close(new QPushButton(this))
{

    auto hl_proctop = new QHBoxLayout();
    auto vl_process = new QVBoxLayout();
    auto hl_procspacedetails = new QHBoxLayout();
    auto fl_procdetails = new QFormLayout();
    auto ld_cmdline = new QLabel(this);
    auto ld_cwd = new QLabel(this);
    auto hl_buttons = new QHBoxLayout();

    lb_icon->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    m_cmdline->setReadOnly(true);
    m_cwd->setReadOnly(true);

    lb_procname->setMaximumWidth(660);
    auto font = lb_procname->font();
    font.setBold(true);
    lb_procname->setFont(font);

    m_tabs->addTab(m_env, tr("Environment"));
    m_tabs->addTab(m_files, tr("Open Files"));
    m_tabs->addTab(m_memory, tr("Memory Map"));

    m_signal->setPopupMode(QToolButton::InstantPopup);
    m_signal->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_signal->setArrowType(Qt::DownArrow);

    hl_proctop->addWidget(lb_icon);
    vl_process->addWidget(lb_procname);
    hl_procspacedetails->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

    fl_procdetails->setLabelAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    fl_procdetails->setWidget(0, QFormLayout::LabelRole, ld_cmdline);
    fl_procdetails->setWidget(0, QFormLayout::FieldRole, m_cmdline);
    fl_procdetails->setWidget(1, QFormLayout::LabelRole, ld_cwd);
    fl_procdetails->setWidget(1, QFormLayout::FieldRole, m_cwd);
    hl_procspacedetails->addLayout(fl_procdetails);
    vl_process->addLayout(hl_procspacedetails);
    hl_proctop->addLayout(vl_process);


    hl_buttons->addWidget(m_help);
    hl_buttons->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl_buttons->addWidget(m_signal);
    hl_buttons->addWidget(m_kill);
    hl_buttons->addWidget(m_close);

    mainLayout->addLayout(hl_proctop);
    mainLayout->addWidget(m_tabs);
    mainLayout->addLayout(hl_buttons);

    setWindowTitle(QCoreApplication::translate("ProcessInfoDialog", "ProcessInfoDialog", nullptr));
    ld_cmdline->setText(QCoreApplication::translate("ProcessInfoDialog", "Command line:", nullptr));
    ld_cwd->setText(QCoreApplication::translate("ProcessInfoDialog", "In directory:", nullptr));
    m_signal->setText(QCoreApplication::translate("ProcessInfoDialog", "Send Signal", nullptr));
    m_kill->setText(QCoreApplication::translate("ProcessInfoDialog", "Terminate", nullptr));
    m_close->setText(QCoreApplication::translate("ProcessInfoDialog", "Close", nullptr));

    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(20,20));

    QFont bfont = m_close->font();
    m_signal->setFont(bfont);

    connect(m_kill, &QPushButton::pressed, this, &ProcessInfoDialog::terminateProcess);
    connect(m_close, &QPushButton::pressed, this, &QDialog::accept);

    resize(500,540);
    setMaximumWidth(1000);
    setMaximumHeight(1000);
    QIcon ico = QIcon::fromTheme(m_icon, QIcon::fromTheme("application-x-executable"));
    lb_icon->setPixmap(ico.pixmap(QSize(64,64)));
    populateDetails();
}

void ProcessInfoDialog::terminateProcess()
{
    auto result = SysmonApplication::instance()->promptForSignal(m_process, m_procname, SIGTERM, this);
    if(result)
        accept();
}

void ProcessInfoDialog::populateDetails()
{
    QString fn = QString("/proc/%1/stat").arg(m_process);
    if(!QFile::exists(fn))
        return;

    QFile file(fn);
    file.open(QFile::ReadOnly);
    QString data = QString(file.readAll());
    file.close();

    struct stat statinfo;
    stat(fn.toUtf8().data(), &statinfo);

    // Filter out our title from stat data
    QRegularExpression regex("\\((.*)\\)");
    if(regex.match(data).hasMatch() &&
            regex.match(data).capturedTexts().count() == 2)
    {
        m_procname = regex.match(data).capturedTexts().at(1).trimmed();
        lb_procname->setText(tr("%1 (PID: %2)").arg(m_procname, QString::number(m_process)));
        setWindowTitle(tr("Process Info: %1 (PID: %2)").arg(m_procname, QString::number(m_process)));
    }

    /*QFile mfile(QString("/proc/%1/statm").arg(m_process));
    mfile.open(QFile::ReadOnly);
    QString mdata = QString(mfile.readAll());
    mfile.close();*/
    //auto mFilteredData = mdata.split(' ');


    /*ppid = filteredData[1].toUInt();
    tty = filteredData[4];

    utime = filteredData[11].toInt();
    stime = filteredData[12].toInt();
    cutime = filteredData[13].toInt();
    cstime = filteredData[14].toInt();

    priority = filteredData[15].toInt();
    nice = filteredData[16].toInt();
    threads = filteredData[17].toULongLong();
    starttime = filteredData[19].toULongLong();
    vsize = filteredData[20].toULongLong();
    rss = filteredData[21].toULongLong();
    shmem = mFilteredData[2].toULongLong();
    user = statinfo.st_uid;*/

    QFile cmdline(QString("/proc/%1/cmdline").arg(m_process));
    cmdline.open(QFile::ReadOnly);
    m_cmdline->setText(cmdline.readAll().replace('\0', ' ').trimmed());
    cmdline.close();
}
