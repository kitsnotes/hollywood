// Hollywood Stage
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "stagetasklist.h"

class PlasmaWindowManagement;
class PlasmaWindow;
class WindowTaskList : public StageTaskList
{
    Q_OBJECT
public:
    explicit WindowTaskList(QWidget *parent);
    ~WindowTaskList();
private slots:
    void activeChanged();
    void windowCreated(PlasmaWindow *wnd);
    void stackingOrderChanaged();

    void plasmaWindowIconChanged();
    void plasmaWindowTitleChanged(const QString &title);
    void plasmaWindowMinimizedChanged();
    void plasmaWindowClosed();
    void plasmaThemeIconChanged(const QString &icon);

    void activated();
    void deactivated();

    void tabClicked(int index);
private:
    PlasmaWindowManagement* manager();
    int findTabByUuid(const QUuid &uuid) const;
private:
    QList<PlasmaWindow*> m_connectedWindows;
};
