#include "nicedialog.h"

#include <QFile>

NiceDialog::NiceDialog(pid_t process, const QString &icon, QWidget *parent)
    : QDialog(parent)
    , m_process(process)
    , m_icon(icon)
    , verticalLayout(new QVBoxLayout(this))
    , lb_icon(new QLabel(this))
    , lb_procname(new QLabel(this))
    , sb_nice(new QSpinBox(this))
    , sld_nice(new QSlider(this))
    , m_help(new QToolButton(this))
    , m_cancel(new QPushButton(this))
    , m_ok(new QPushButton(this))
{
    resize(450, 210);
    auto hl_proctop = new QHBoxLayout();
    auto vl_process = new QVBoxLayout();
    auto hl_procspacedetails = new QHBoxLayout();

    auto lbl_nicedesc = new QLabel(this);
    auto fl_nicesp = new QFormLayout();
    auto lb_nice = new QLabel(this);
    auto line = new QFrame(this);

    auto lb_high = new QLabel(this);
    auto lb_low = new QLabel(this);
    auto horizontalLayout = new QHBoxLayout();
    auto vl_slider_info = new QVBoxLayout();
    vl_slider_info->setSpacing(0);

    sb_nice->setMinimum(-20);
    sb_nice->setMaximum(20);
    sld_nice->setMinimum(-20);
    sld_nice->setMaximum(20);
    sld_nice->setOrientation(Qt::Horizontal);
    sld_nice->setTickPosition(QSlider::TicksBelow);

    lb_icon->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    hl_proctop->addWidget(lb_icon);
    vl_process->addWidget(lb_procname);
    hl_procspacedetails->addItem(new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
    hl_procspacedetails->addWidget(lbl_nicedesc);
    vl_process->addLayout(hl_procspacedetails);
    vl_process->addWidget(line);
    fl_nicesp->setFormAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);
    fl_nicesp->setWidget(0, QFormLayout::LabelRole, lb_nice);
    fl_nicesp->setWidget(0, QFormLayout::FieldRole, sb_nice);
    vl_process->addLayout(fl_nicesp);


    vl_slider_info->addWidget(sld_nice);
    auto hl_prio = new QHBoxLayout();
    hl_prio->setSpacing(0);
    hl_prio->addWidget(lb_high);
    hl_prio->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hl_prio->addWidget(lb_low);
    vl_slider_info->addLayout(hl_prio);
    vl_process->addLayout(vl_slider_info);
    hl_proctop->addLayout(vl_process);
    verticalLayout->addLayout(hl_proctop);
    verticalLayout->addItem(new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    horizontalLayout->addWidget(m_help);
    horizontalLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    horizontalLayout->addWidget(m_cancel);
    horizontalLayout->addWidget(m_ok);
    verticalLayout->addLayout(horizontalLayout);

    lbl_nicedesc->setWordWrap(true);
    lbl_nicedesc->setText(QCoreApplication::translate("NiceDialog", "A higher nice value yields more CPU time to other processes; a lower value consumes more for itself.", nullptr));
    lb_nice->setText(QCoreApplication::translate("NiceDialog", "New niceness:", nullptr));
    lb_high->setText(QCoreApplication::translate("NiceDialog", "High priority", nullptr));
    lb_low->setText(QCoreApplication::translate("NiceDialog", "Low priority", nullptr));
    m_cancel->setText(QCoreApplication::translate("NiceDialog", "Cancel", nullptr));
    m_ok->setText(QCoreApplication::translate("NiceDialog", "Renice", nullptr));

    QIcon ico = QIcon::fromTheme(m_icon, QIcon::fromTheme("application-x-executable"));
    lb_icon->setPixmap(ico.pixmap(QSize(64,64)));

    auto font = lb_procname->font();
    font.setBold(true);
    lb_procname->setFont(font);

    m_help->setIcon(QIcon::fromTheme("help-contextual"));
    m_help->setIconSize(QSize(20,20));

    setMaximumSize(451,211);

    QString fn = QString("/proc/%1/stat").arg(m_process);
    if(!QFile::exists(fn))
        return;

    QFile file(fn);
    file.open(QFile::ReadOnly);
    QString data = QString(file.readAll());
    file.close();

    // Filter out our title from stat data
    QRegularExpression regex("\\((.*)\\)");
    if(regex.match(data).hasMatch() &&
            regex.match(data).capturedTexts().count() == 2)
    {
        auto text = regex.match(data).capturedTexts().at(1);
        lb_procname->setText(tr("Renice process: %1 (PID: %2)").arg(text.trimmed(), QString::number(m_process)));
        setWindowTitle(tr("Process priority for: %1 (PID: %2)").arg(text.trimmed(), QString::number(m_process)));
    }
}


