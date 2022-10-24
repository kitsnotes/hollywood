#include "overview.h"
#include "chartobject.h"
#include "cpuoverview.h"
#include "ramoverview.h"
#include "diskoverview.h"
#include <unistd.h>
#include <sys/utsname.h>

OverviewWidget::OverviewWidget(QWidget *parent)
    : QWidget(parent),
      vl_main(new QVBoxLayout(this)),
      hl_top_main(new QHBoxLayout()),
      m_icon(new QLabel(this)),
      vl_hostname(new QVBoxLayout()),
      m_hostname(new QLabel(this)),
      m_subtitle(new QLabel(this)),
      sp_topLayout(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)),
      vl_updates(new QVBoxLayout()),
      hl_updates(new QHBoxLayout()),
      m_updateIcon(new QLabel(this)),
      m_updatelabel(new QLabel(this)),
      m_applyUpdates(new QPushButton(this)),
      m_splitter(new QSplitter(this)),
      m_selection(new SelectorWidget(m_splitter)),
      m_selmodel(new QStandardItemModel(this)),
      m_cpuoverview(new CPUOverview(m_splitter)),
      m_ramoverview(new RAMOverview(this)),
      m_diskoverview(new DiskOverview(this)),
      m_cpu(new QStandardItem(tr("CPU"))),
      m_ram(new QStandardItem(tr("Memory"))),
      m_disk(new QStandardItem(tr("Disk"))),
      m_network(new QStandardItem(tr("Network")))
{
    m_selection->setFrameShape(QFrame::NoFrame);
    QPalette p = m_selection->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    m_selection->setPalette(p);
    m_selection->viewport()->setAutoFillBackground(false);
    // assemble our hostname info
    m_icon->setPixmap(QIcon::fromTheme("computer").pixmap(QSize(64,64)));
    m_icon->setScaledContents(false);

    QFont font(m_hostname->font());
    font.setPointSize(font.pointSize()+3);
    font.setBold(true);
    m_hostname->setFont(font);

    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(m_selection);
    m_selection->setMaximumWidth(160);
    m_splitter->addWidget(m_cpuoverview);
    m_subtitle->setIndent(15);
    m_updatelabel->setIndent(2);

    m_applyUpdates->setText(tr("Apply Updates"));

    // setup our side selection
    m_cpu->setIcon(QIcon::fromTheme("cpu"));
    m_ram->setIcon(QIcon::fromTheme("media-flash"));
    m_disk->setIcon(QIcon::fromTheme("drive-multidisk"));
    m_network->setIcon(QIcon::fromTheme("network-rj45-female"));

    m_selmodel->appendRow(m_cpu);
    m_selmodel->appendRow(m_ram);
    m_selmodel->appendRow(m_disk);
    m_selmodel->appendRow(m_network);
    m_selection->setModel(m_selmodel);
    m_selection->setIconSize(QSize(32,32));
    m_selection->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_selection->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_selection->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &OverviewWidget::selectionActivated);
    // layout
    vl_hostname->addWidget(m_hostname);
    vl_hostname->addWidget(m_subtitle);
    vl_hostname->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));

    hl_updates->addWidget(m_updateIcon);
    hl_updates->addWidget(m_updatelabel);
    vl_updates->addLayout(hl_updates);
    vl_updates->addWidget(m_applyUpdates);
    vl_updates->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vl_updates->setContentsMargins(0,5,0,0);

    hl_top_main->addWidget(m_icon);
    hl_top_main->addLayout(vl_hostname);
    hl_top_main->addItem(sp_topLayout);
    hl_top_main->addLayout(vl_updates);
    hl_top_main->setContentsMargins(5,0,5,0);

    vl_main->setSpacing(0);
    vl_main->setContentsMargins(0, 0, 0, 0);
    vl_main->addLayout(hl_top_main);
    vl_main->addWidget(m_splitter);
    vl_main->setStretch(1, 1);

    m_network->setEnabled(false);

    m_selection->setFrameShape(QFrame::NoFrame);
    setUpdatesOk();
    updateHostUname();
    selectionActivated(m_selmodel->indexFromItem(m_cpu), QModelIndex());
}

void OverviewWidget::poll()
{
    m_cpuoverview->poll();
    m_ramoverview->poll();
    m_diskoverview->poll();
}

void OverviewWidget::setGraphPollTimeTitle(const QString &title)
{
    m_cpuoverview->setGraphPollTimeTitle(title);
    m_ramoverview->setGraphPollTimeTitle(title);
    m_diskoverview->setGraphPollTimeTitle(title);
}

void OverviewWidget::selectionActivated(const QModelIndex &newIdx, const QModelIndex &old)
{
    Q_UNUSED(old);
    auto item = m_selmodel->itemFromIndex(newIdx);
    if(item == m_network)
        return; // currently not supported

    m_cpuoverview->setVisible(false);
    m_ramoverview->setVisible(false);
    m_diskoverview->setVisible(false);

    if(item == m_cpu)
    {
        m_selection->selectionModel()->clearSelection();
        m_selection->selectionModel()->select(newIdx, QItemSelectionModel::SelectCurrent);
        m_splitter->replaceWidget(1, m_cpuoverview);
        m_cpuoverview->setVisible(true);
    }
    if(item == m_ram)
    {
        m_selection->selectionModel()->clearSelection();
        m_selection->selectionModel()->select(newIdx, QItemSelectionModel::SelectCurrent);
        m_splitter->replaceWidget(1, m_ramoverview);
        m_ramoverview->setVisible(true);
    }
    if(item == m_disk)
    {
        m_selection->selectionModel()->clearSelection();
        m_selection->selectionModel()->select(newIdx, QItemSelectionModel::SelectCurrent);
        m_splitter->replaceWidget(1, m_diskoverview);
        m_diskoverview->setVisible(true);
    }
    if(item == m_network)
    {
        m_selection->selectionModel()->select(newIdx, QItemSelectionModel::SelectCurrent);
    }
}

void OverviewWidget::updateHostUname()
{
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    m_hostname->setText(hostname);

    struct utsname unb;
    if(uname(&unb) != 0)
        m_subtitle->setText(tr("Error in uname lookup"));
    else
        m_subtitle->setText(tr("Hollywood %1, Kernel %2").arg("v.Next").arg(unb.release));

}

void OverviewWidget::setUpdatesOk()
{
    m_updateIcon->setPixmap(QIcon::fromTheme("emblem-checked").pixmap(QSize(16,16)));
    m_updatelabel->setText(tr("System up to date"));
    m_applyUpdates->setVisible(false);
}

void OverviewWidget::setUpdatesRequired()
{
    m_updateIcon->setPixmap(QIcon::fromTheme("emblem-important").pixmap(QSize(16,16)));
    m_updatelabel->setText(tr("Updates available"));
    m_applyUpdates->setVisible(true);
}


SelectorWidget::SelectorWidget(QWidget *parent)
    :QListView(parent) {}

void SelectorWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers().testFlag(Qt::ControlModifier))
        e->accept();

    QListView::keyPressEvent(e);
}
