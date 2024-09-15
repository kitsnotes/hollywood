// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "appbutton.h"

AppButton::AppButton(const QString &desktopFile, const uint pid, QWidget *parent)
    : AbstractTaskViewItem()
{
    // We need either a valid desktop file or a valid pid to work with
    if(!desktopFile.isEmpty())
    {
        auto desktopPath =
            LSDesktopEntry::findDesktopFile(desktopFile);

        QFile f(desktopPath);
        if(f.exists() && f.isReadable())
        {
            // valid desktop file
            m_desktop = new LSDesktopEntry();
            m_desktop->load(desktopPath);
            m_displayName = m_desktop->value("Name").toString();
            setIcon(m_desktop->icon());
            setToolTip(m_displayName);
        }
    }

    if(pid > 0)
    {
        // a process is running
        QFile file(QString("/proc/%1/cmdline"));
        if(file.exists())
            m_pids.append(pid);
    }
}

void AppButton::attachPid(const uint pid)
{
    m_pids.append(pid);
}

void AppButton::activateLastWindow()
{

}

void AppButton::parseDesktopActions()
{
    if(!m_desktop)
        return;

    if(!m_desktop->isValid())
        return;

    auto items = m_desktop->value("Actions").toStringList();
    for(auto item : items)
    {
        auto name =
            m_desktop->value(QString("Desktop Action %1\\Name").arg(item)).toString();
        auto icon =
            m_desktop->value(QString("Desktop Action %1\\Icon").arg(item)).toString();
        auto exec =
            m_desktop->value(QString("Desktop Action %1\\Exec").arg(item)).toString();

        qDebug() << name << icon << exec;
    }
}

bool AppButton::running()
{
    return m_pids.count() == 0 ? false : true;
}

AppTaskView::AppTaskView()
{

}

TaskItems AppTaskView::taskItems()
{

}
