// Hollywood System Monitor
// (C) 2022 Cat Stevenson <cat@originull.org>
// (C) 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#include "window.h"
#include "procfsmodel.h"
#include "overview.h"
#include "logviewer.h"
#include "openrc.h"
#include "openrcmodel.h"
#include "infodialog.h"
#include "nicedialog.h"
#include "application.h"

#include <aboutdialog.h>
#include <hollywood/hollywood.h>
#include <signal.h>

SysmonWindow::SysmonWindow(QWidget *parent)
    : QMainWindow(parent),
      m_menubar(new QMenuBar(this)),
      m_toolbar(new QToolBar(this)),
      m_model(new LSProcfsModel(this)),
      m_overview(new OverviewWidget(this)),
      m_procview(new QTreeView(this)),
      m_serviceview(new QTreeView(this)),
      m_logs(new LogViewer(this)),
      m_timer(new QTimer(this)),
      m_host(new CentralWidgetHost(m_overview, m_procview, m_serviceview, m_logs, this)),
      m_procContext(new QMenu(this))
{
    setWindowTitle(tr("System Monitor"));
    setWindowIcon(QIcon::fromTheme("systemsettings"));
    setCentralWidget(m_host);

    setMinimumHeight(400);
    setMinimumWidth(400);
    // Menu Bar
    setMenuBar(m_menubar);

    auto process = m_menubar->addMenu(tr("&Process"));
    process->addAction(tr("&Run..."))->setDisabled(true);
    process->addSeparator();

    auto renice = process->addAction(tr("Set &Priority..."));
    renice->setDisabled(true);
    m_selectedProcessActions.append(renice);
    connect(renice, &QAction::triggered, this, &SysmonWindow::reniceProcess);

    auto kill = process->addAction(tr("&End Process..."));
    kill->setIcon(QIcon::fromTheme("process-stop"));
    kill->setDisabled(true);
    kill->setPriority(QAction::LowPriority);
    m_selectedProcessActions.append(kill);
    connect(kill, &QAction::triggered, this, &SysmonWindow::termProcess);

    auto kill9 = process->addAction(tr("&Force End Process..."));
    kill9->setDisabled(true);
    m_selectedProcessActions.append(kill9);
    connect(kill9, &QAction::triggered, this, &SysmonWindow::forceTermProcess);

    process->addSeparator();
    auto getinfo = process->addAction(tr("&Get Process Info..."));
    getinfo->setDisabled(true);
    m_selectedProcessActions.append(getinfo);
    connect(getinfo, &QAction::triggered, this, &SysmonWindow::getProcessInfo);
    process->addSeparator();

    m_procContext->addAction(renice);
    m_procContext->addAction(kill);
    m_procContext->addAction(kill9);
    m_procContext->addSeparator();
    m_procContext->addAction(getinfo);

    auto quit = process->addAction(tr("&Quit"));
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    // Toolbar (we use this as tabs)
    addToolBar(Qt::TopToolBarArea, m_toolbar);
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);
    m_toolbar->setWindowTitle(tr("Toolbar"));
    m_toolbar->toggleViewAction()->setDisabled(true);
    m_toolbar->setContextMenuPolicy(Qt::NoContextMenu);
    m_menubar->setContextMenuPolicy(Qt::NoContextMenu);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QWidget *left = new QWidget(this);
    left->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    QWidget *right = new QWidget(this);
    right->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // our views
    m_toolbar->addAction(kill);
    m_toolbar->addWidget(left);

    m_ovact = m_toolbar->addAction(tr("&Overview"));
    m_ovact->setCheckable(true);
    m_ovact->setIcon(QIcon::fromTheme("quickview"));
    m_ovact->setShortcut(QKeySequence("Ctrl+1"));

    m_processes = m_toolbar->addAction(tr("&Processes"));
    m_processes->setCheckable(true);
    m_processes->setChecked(true);
    m_processes->setIcon(QIcon::fromTheme("view-process-tree"));
    m_processes->setShortcut(QKeySequence("Ctrl+2"));

    auto users = m_toolbar->addAction(tr("&Users"));
    users->setIcon(QIcon::fromTheme("view-process-users"));
    users->setDisabled(true);
    users->setShortcut(QKeySequence("Ctrl+3"));
    users->setCheckable(true);

    m_services = m_toolbar->addAction(tr("&Services"));
    m_services->setIcon(QIcon::fromTheme("process-working-symbolic"));
    m_services->setDisabled(true);
    m_services->setShortcut(QKeySequence("Ctrl+4"));
    m_services->setCheckable(true);

    if(OpenRCMonitor::systemHasOpenRCInit())
    {
        m_svcmonitor = new OpenRCMonitor(this);
        m_services->setEnabled(true);
        m_serviceview->setModel(m_svcmonitor->model());
        m_serviceview->setRootIsDecorated(false);
    }

    m_logview = m_toolbar->addAction(tr("&Logs"));
    m_logview->setIcon(QIcon::fromTheme("viewlog"));
    m_logview->setShortcut(QKeySequence("Ctrl+5"));
    m_logview->setCheckable(true);
    m_toolbar->addWidget(right);

    // add all views to action group
    auto group = new QActionGroup(this);
    group->addAction(m_ovact);
    group->addAction(m_processes);
    group->addAction(users);
    group->addAction(m_services);
    group->addAction(m_logview);

    connect(m_ovact, &QAction::triggered, this, &SysmonWindow::triggerView);
    connect(m_processes, &QAction::triggered, this, &SysmonWindow::triggerView);
    connect(m_services, &QAction::triggered, this, &SysmonWindow::triggerView);
    connect(m_logview, &QAction::triggered, this, &SysmonWindow::triggerView);

    // View Menu
    auto view = m_menubar->addMenu(tr("&View"));
    view->addAction(m_ovact);
    view->addAction(m_processes);
    view->addAction(users);
    view->addAction(m_services);
    view->addAction(m_logview);
    view->addSeparator();
    auto update = view->addMenu(tr("Update &Speed"));
    m_ufrequent = update->addAction(tr("&Frequent"));
    m_unormal = update->addAction(tr("&Normal"));
    m_uslow = update->addAction(tr("&Slow"));
    m_ufrequent->setCheckable(true);
    m_uslow->setCheckable(true);
    m_unormal->setCheckable(true);

    connect(m_ufrequent, &QAction::triggered, this, &SysmonWindow::updateSpeedChanged);
    connect(m_uslow, &QAction::triggered, this, &SysmonWindow::updateSpeedChanged);
    connect(m_unormal, &QAction::triggered, this, &SysmonWindow::updateSpeedChanged);

    auto speeds = new QActionGroup(this);
    speeds->addAction(m_ufrequent);
    speeds->addAction(m_uslow);
    speeds->addAction(m_unormal);

    m_pause = view->addAction(tr("Pause &Updates"));
    m_pause->setCheckable(true);
    m_pause->setIcon(QIcon::fromTheme("media-playback-pause"));
    m_pause->setPriority(QAction::LowPriority);
    m_toolbar->addAction(m_pause);
    connect(m_pause, &QAction::triggered, this, &SysmonWindow::updatePauseTriggered);

    auto help = m_menubar->addMenu(tr("&Help"));
    help->addAction(tr("&Get System Help"))->setDisabled(true);
    auto about = help->addAction(tr("&About System Monitor"));
    connect(about, &QAction::triggered, this, &SysmonWindow::about);

    m_procview->setModel(m_model);
    m_procview->setRootIsDecorated(false);
    m_procview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_procview, &QTreeView::customContextMenuRequested, this, &SysmonWindow::processContextMenuRequested);
    connect(m_procview, &QTreeView::activated, this, &SysmonWindow::processItemActivated);
    changeCurrentView(Overview);
    QSettings persist("originull", "persistence");
    if(persist.contains("Cache/LastWindowPos"))
        restoreGeometry(persist.value("Cache/LastWindowPos").toByteArray());
    else
    {
        // fallback defaults
        resize(560,440);
        move(screen()->geometry().center() - frameGeometry().center());
    }

    QSettings settings("originull", "hollywood");
    settings.beginGroup("SystemMonitor");
    uint speed = settings.value("UpdateSpeed", 1).toUInt();
    bool paused = settings.value("UpdatesPaused", false).toBool();

    if(speed > 2)
        speed = 1;

    m_speed = (UpdateSpeed)speed;

    connect(m_timer, &QTimer::timeout, this, &SysmonWindow::poll);
    int msec = 0;
    switch(speed)
    {
    case Frequent:
        m_ufrequent->setChecked(true);
        msec = 500;
        break;
    case Slow:
        m_uslow->setChecked(true);
        msec = 4000;
        break;
    case Normal:
    default:
        m_unormal->setChecked(true);
        msec = 1000;
        break;
    }
    updateSpeedOnGraphs();
    m_timer->setInterval(msec);
    m_timer->setSingleShot(false);
    if(!paused)
        m_timer->start();
    else
        m_pause->setChecked(true);

    connect(m_procview->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &SysmonWindow::processRowChanged);
}

SysmonWindow::~SysmonWindow()
{

}

void SysmonWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("originull", "persistence");
    // preserve our view states
    settings.setValue("Cache/LastWindowPos", saveGeometry());

    event->accept();
}

void SysmonWindow::changeCurrentView(ViewMode mode)
{
    m_host->setViewMode(mode);
    switch(mode)
    {
    case Processes:
        m_processes->setChecked(true);
        break;
    case Logs:
        m_logview->setChecked(true);
        break;
    case Services:
        m_services->setChecked(true);
        break;
    case Overview:
    default:
        m_ovact->setChecked(true);
        break;
    }
    m_mode = mode;
}

void SysmonWindow::poll()
{
    m_overview->poll();
    m_model->reload();
}

void SysmonWindow::triggerView()
{
    auto action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);

    if(action == m_ovact)
    {
        enableProcessActions(false);
        changeCurrentView(Overview);
    }

    if(action == m_processes)
        changeCurrentView(Processes);

    if(action == m_logview)
    {
        enableProcessActions(false);
        changeCurrentView(Logs);
    }

    if(action == m_services && m_serviceview)
    {
        enableProcessActions(false);
        changeCurrentView(Services);
    }
}

void SysmonWindow::updateSpeedChanged()
{
    auto action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);

    QSettings settings("originull", "hollywood");
    settings.beginGroup("SystemMonitor");

    action->setChecked(true);
    UpdateSpeed speed = Normal;
    if(action == m_ufrequent)
        speed = Frequent;

    if(action == m_uslow)
        speed = Slow;

    settings.setValue("UpdateSpeed", speed);
    settings.setValue("UpdatesPaused", false);
    m_pause->setChecked(false);

    int msec = 0;
    switch(speed)
    {
    case Frequent:
        msec = 500;
        break;
    case Slow:
        msec = 4000;
        break;
    case Normal:
    default:
        msec = 1000;
        break;
    }
    m_speed = speed;
    updateSpeedOnGraphs();

    m_timer->stop();
    m_timer->setInterval(msec);
    m_timer->start();
}

void SysmonWindow::updatePauseTriggered()
{
    QAction *m_act = qobject_cast<QAction*>(sender());
    QSettings settings("originull", "hollywood");
    settings.beginGroup("SystemMonitor");

    settings.setValue("UpdatesPaused", m_act->isChecked());

    if(m_act->isChecked())
        m_timer->stop();
    else
    {
        uint msec = 0;
        switch(m_speed)
        {
        case Frequent:
            msec = 500;
            break;
        case Slow:
            msec = 4000;
            break;
        case Normal:
        default:
            msec = 1000;
            break;
        }
        m_timer->start(msec);
    }
}

void SysmonWindow::about()
{
    auto about = new ADAboutDialog(this);
    about->setAppDescription(tr("API Version %1").arg(QLatin1String(QT_VERSION_STR)));
    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();
}

void SysmonWindow::termProcess()
{
    auto idx = m_procview->selectionModel()->selectedRows().first();
    if(!m_model->checkIndex(idx))
        return;

    auto name = m_model->valueForIndex(LSProcfsModel::ProcessName, idx).toString();
    pid_t pid = m_model->pidForIndex(idx);
    SysmonApplication::instance()->promptForSignal(pid, name, SIGTERM);
}

void SysmonWindow::forceTermProcess()
{
    auto idx = m_procview->selectionModel()->selectedRows().first();
    if(!m_model->checkIndex(idx))
        return;

    auto name = m_model->valueForIndex(LSProcfsModel::ProcessName, idx).toString();
    pid_t pid = m_model->pidForIndex(idx);

    SysmonApplication::instance()->promptForSignal(pid, name, SIGKILL);
}

void SysmonWindow::getProcessInfo()
{
    auto idx = m_procview->selectionModel()->selectedRows().first();
    if(!m_model->checkIndex(idx))
        return;

    pid_t pid = m_model->pidForIndex(idx);

    auto dti = m_model->desktopEntryIconForIndex(idx);

    auto infownd = new ProcessInfoDialog(pid, dti, this);
    connect(infownd, &QDialog::finished, infownd, &QDialog::deleteLater);
    infownd->show();
}

void SysmonWindow::reniceProcess()
{
    auto idx = m_procview->selectionModel()->selectedRows().first();
    if(!m_model->checkIndex(idx))
        return;

    pid_t pid = m_model->pidForIndex(idx);
    auto dti = m_model->desktopEntryIconForIndex(idx);

    auto nicewnd = new NiceDialog(pid, dti, this);
    connect(nicewnd, &QDialog::finished, nicewnd, &QDialog::deleteLater);
    nicewnd->show();
}

void SysmonWindow::enableProcessActions(bool enabled)
{
    for(auto a : m_selectedProcessActions)
        a->setEnabled(enabled);
}

void SysmonWindow::processRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(current.isValid())
        enableProcessActions(true);
    else
        enableProcessActions(false);
}

void SysmonWindow::processContextMenuRequested(const QPoint &pos)
{
    m_procContext->popup(mapToGlobal(pos));
}

void SysmonWindow::processItemActivated(const QModelIndex &index)
{
    pid_t pid = m_model->pidForIndex(index);
    auto dti = m_model->desktopEntryIconForIndex(index);
    auto infownd = new ProcessInfoDialog(pid, dti, this);
    connect(infownd, &QDialog::finished, infownd, &QDialog::deleteLater);
    infownd->show();
}

void SysmonWindow::updateSpeedOnGraphs()
{
    QString str;
    switch(m_speed)
    {
    case Frequent:
        str = tr("30 seconds");
        break;
    case Slow:
        str = tr("4 Minutes");
        break;
    case Normal:
    default:
        str = tr("60 seconds");
        break;
    }
    m_overview->setGraphPollTimeTitle(str);
}


CentralWidgetHost::CentralWidgetHost(OverviewWidget *ov, QTreeView *pc, QTreeView *sc, LogViewer *l, QWidget *parent)
    :QWidget(parent)
{
    Q_ASSERT(pc);
    m_procview = pc;
    pc->setParent(this);
    Q_ASSERT(ov);
    m_overview = ov;
    ov->setParent(this);
    Q_ASSERT(sc);
    m_serviceview = sc;
    sc->setParent(this);
    Q_ASSERT(l);
    m_logs = l;
    l->setParent(this);
}

void CentralWidgetHost::setViewMode(SysmonWindow::ViewMode vm)
{
    switch(vm)
    {
    case SysmonWindow::Logs:
        m_overview->setVisible(false);
        m_procview->setVisible(false);
        m_serviceview->setVisible(false);

        m_logs->resize(size());
        m_logs->setVisible(true);
        m_visible = m_logs;
        break;
    case SysmonWindow::Processes:
        m_overview->setVisible(false);
        m_logs->setVisible(false);
        m_serviceview->setVisible(false);
        m_procview->resize(size());
        m_procview->setVisible(true);
        m_visible = m_procview;
        break;
    case SysmonWindow::Services:
        m_overview->setVisible(false);
        m_procview->setVisible(false);
        m_logs->setVisible(false);

        m_serviceview->resize(size());
        m_serviceview->setVisible(true);
        m_visible = m_serviceview;
        break;
    case SysmonWindow::Overview:
    default:
        m_procview->setVisible(false);
        m_logs->setVisible(false);
        m_serviceview->setVisible(false);

        m_overview->resize(size());
        m_overview->setVisible(true);
        m_visible = m_overview;
        break;
    }
}

void CentralWidgetHost::resizeEvent(QResizeEvent *e)
{
    if(m_visible)
    {
        m_visible->resize(e->size());
        e->setAccepted(true);
    }
}

