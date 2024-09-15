// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QScreen>
#include "wndmgmt.h"
#include <hollywood/layershellwindow.h>

class NotifierHost;
class StatusNotifierButton;
class StageClock;
class WindowTaskList;
class BatteryMonitor;
class AbstractTaskViewItem;
class StageHost : public QWidget
{
    Q_OBJECT
public:
    explicit StageHost(QScreen *screen, QWidget *parent = nullptr);

    void setAlignment(Qt::Orientation align);
    void setClock(StageClock *clock);
    void takeClock();
    void setBattery(BatteryMonitor *clock);
    void takeBattery();
    void setMainEnabled(bool enabled);
    void switchToWindowList();
    void switchToAppList();
public slots:
    void show();
    void createStatusButton(StatusNotifierButton *btn);
    void statusButtonRemoved(StatusNotifierButton *btn);
protected:
    void resizeEvent(QResizeEvent *event) override;
protected slots:
private:
    bool m_ready = false;
    LayerShellQt::Window *m_lswnd;
    QScreen *m_screen = nullptr;
    Qt::Orientation m_align = Qt::Horizontal;
    QVBoxLayout *m_vbox = nullptr;
    QHBoxLayout *m_hbox = nullptr;
    QSpacerItem *m_hspacer = nullptr;
    QSpacerItem *m_vspacer = nullptr;
    QToolButton *m_menu = nullptr;
    QToolButton *m_showdesktop = nullptr;

    WindowTaskList *m_windowList = nullptr;
    QSpacerItem *m_trayspacer = nullptr;
    StageClock *m_clock = nullptr;
    BatteryMonitor *m_battery = nullptr;
};
