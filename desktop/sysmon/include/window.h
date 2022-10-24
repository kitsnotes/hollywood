// Hollywood System Monitor
// (C) 2022 Cat Stevenson <cat@originull.org>
// (C) 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QTreeView>

class OpenRCMonitor;
class LogViewer;
class LSProcfsModel;
class OverviewWidget;
class CentralWidgetHost;
class ServiceView;
class SysmonWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ViewMode {
        Overview,
        Processes,
        Services,
        Logs
    };
    enum UpdateSpeed {
        Frequent,
        Normal,
        Slow
    };
    SysmonWindow(QWidget *parent = nullptr);
    ~SysmonWindow();
protected:
    void closeEvent(QCloseEvent* event);
    void changeCurrentView(ViewMode mode);
private slots:
    void poll();
    void triggerView();
    void updateSpeedChanged();
    void updatePauseTriggered();
    void about();
    void termProcess();
    void forceTermProcess();
    void getProcessInfo();
    void reniceProcess();
    void enableProcessActions(bool enabled);
    void processRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void processContextMenuRequested(const QPoint &pos);
    void processItemActivated(const QModelIndex &index);
private:
    void updateSpeedOnGraphs();
private:
    UpdateSpeed m_speed = Normal;
    ViewMode m_mode = Processes;
    QMenuBar *m_menubar = nullptr;
    QToolBar *m_toolbar = nullptr;
    LSProcfsModel *m_model = nullptr;
    OverviewWidget *m_overview = nullptr;
    QTreeView *m_procview = nullptr;
    QTreeView *m_serviceview = nullptr;
    LogViewer *m_logs = nullptr;
    QTimer *m_timer = nullptr;
    CentralWidgetHost *m_host = nullptr;
    OpenRCMonitor *m_svcmonitor = nullptr;

    QAction *m_ovact = nullptr;
    QAction *m_processes = nullptr;
    QAction *m_services = nullptr;
    QAction *m_logview = nullptr;

    QAction *m_ufrequent = nullptr;
    QAction *m_uslow = nullptr;
    QAction *m_unormal = nullptr;
    QAction *m_pause = nullptr;

    QList<QAction*> m_selectedProcessActions;

    QMenu *m_procContext = nullptr;
};

class CentralWidgetHost : public QWidget
{
Q_OBJECT
public:
    CentralWidgetHost(OverviewWidget *ov, QTreeView *pc, QTreeView *sc, LogViewer *l, QWidget *parent = nullptr);
    void setViewMode(SysmonWindow::ViewMode vm);
protected:
    void resizeEvent(QResizeEvent *e);

private:
    LogViewer *m_logs = nullptr;
    OverviewWidget *m_overview = nullptr;
    QTreeView *m_serviceview = nullptr;
    QTreeView *m_procview = nullptr;
    QWidget *m_visible = nullptr;
};

#endif // WINDOW_H
