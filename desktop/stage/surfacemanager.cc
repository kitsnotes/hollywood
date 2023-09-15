#include "surfacemanager.h"
#include "taskbutton.h"
#include "wndmgmt.h"
#include "stagehost.h"

SurfaceManager::SurfaceManager(StageHost *parent)
    : QObject(nullptr)
    , m_parent(parent)
    , m_wndmgr(new PlasmaWindowManagement())
    , m_group(new QButtonGroup(this))
{
    connect(m_wndmgr, &PlasmaWindowManagement::windowCreated,
            this, &SurfaceManager::newPlasmaWindow);
    connect(m_wndmgr, &PlasmaWindowManagement::stackingOrderChanaged,
            this, &SurfaceManager::stackingOrderChanged);

    m_group->setExclusive(true);
}

void SurfaceManager::newPlasmaWindow(PlasmaWindow *wnd)
{
    auto btn = new TaskButton(wnd, m_parent);
    btn->setText(wnd->windowTitle());
    btn->setToolTip(wnd->windowTitle());

    m_group->addButton(btn);
    m_windows.append(btn);

    connect(wnd, &PlasmaWindow::windowClosed, this, &SurfaceManager::windowClosed);
    connect(wnd, &PlasmaWindow::minimizedChanged, this, &SurfaceManager::minimizedChanged);
    connect(wnd, &PlasmaWindow::destroyed, btn, &TaskButton::deleteLater);

    connect(btn, &QAbstractButton::pressed, this, &SurfaceManager::buttonClicked);

    emit windowCreated(btn);
}

void SurfaceManager::stackingOrderChanged()
{
    //qDebug() << "stacking order chanaged" << StageApplication::instance()->windowManager()->stackingOrder();

    for (auto &uuid : m_wndmgr->stackingOrder())
    {
        for(auto btn : m_windows)
        {
            auto btnuuid = btn->window()->uuid().toString(QUuid::WithoutBraces);
            if(btnuuid == uuid)
            {
                //qDebug() << btnuuid << "selected" << btn->window()->windowTitle();
                btn->setDown(true);
                return;
            }
        }
    }
}

void SurfaceManager::buttonClicked()
{
    auto btn = qobject_cast<TaskButton*>(sender());
    if(btn->window()->minimized())
    {
        btn->window()->toggleMinimize();
        btn->window()->activate();
    }
    else
    {
        if(!btn->window()->isActive())
            btn->window()->activate();
        else
            btn->window()->toggleMinimize();
    }
}

void SurfaceManager::windowClosed()
{
    auto pm = qobject_cast<PlasmaWindow*>(sender());

    if(!pm)
        return;

    for(auto btn : m_windows)
    {
        if(btn->window() == pm)
        {
            emit windowDestroyed(btn);
            m_group->removeButton(btn);
            disconnect(pm, &PlasmaWindow::windowTitleChanged, btn, &QToolButton::setText);
            m_windows.removeOne(btn);
            btn->deleteLater();
        }
    }
}

void SurfaceManager::windowMinimized()
{

}

void SurfaceManager::minimizedChanged()
{

}
