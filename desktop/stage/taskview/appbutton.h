// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "taskview.h"
#include "desktopentry.h"

class AppButton : public AbstractTaskViewItem
{
    Q_OBJECT
public:
    AppButton(const QString &desktopFile, const uint pid, QWidget *parent);
    void attachPid(const uint pid);
    bool running();
public slots:
    void activateLastWindow();
private:
    void parseDesktopActions();
private:
    bool m_pinned = false;
    QList<uint> m_pids;
    LSDesktopEntry* m_desktop = nullptr;
    QString m_displayName;

    QList<QAction*> m_desktop_actions;
};

class AppTaskView : public AbstractTaskView
{
    Q_OBJECT
    explicit AppTaskView();
    TaskItems taskItems() override;
};
