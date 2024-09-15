// Hollywood Stage
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "windowlist.h"
#include "wndmgmt.h"
#include "stage.h"

#include <QMenu>
#include <QButtonGroup>

/*WindowButton::WindowButton(PlasmaWindow *wnd, QWidget *parent)
    : AbstractTaskViewItem()
    , m_window(wnd)
    , m_context(new QMenu(this))
{
    //setMinimumHeight(20);

    QFont myFont = font();
    auto size = myFont.pointSize()+1;
    size *= parent->window()->devicePixelRatio();
    myFont.setPointSize(qMax(10,size));
    setFont(myFont);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(wnd, &PlasmaWindow::themeIconChanged,
            this, &WindowButton::themeIconChanged);
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_restore = m_context->addAction(tr("&Restore"));
    m_move = m_context->addAction(tr("&Move"));
    m_size = m_context->addAction(tr("&Size"));
    m_minimize = m_context->addAction(tr("Mi&nimize"));
    m_maximize = m_context->addAction(tr("M&aximize"));
    m_context->addSeparator();
    m_close = m_context->addAction(tr("&Close Window"));

    m_move->setEnabled(false);
    m_size->setEnabled(false);


    connect(this, &WindowButton::customContextMenuRequested,
            this, &WindowButton::displayContextMenu);
}

void WindowTaskView::stackingOrderChanged()
{

}

void WindowTaskView::buttonClicked()
{
    auto btn = qobject_cast<WindowButton*>(sender());
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
s*/

WindowTaskList::WindowTaskList(QWidget *parent)
    : StageTaskList(parent)
{
    // see if we have any windows to create
    // (this would be if we change from app->window at runtime
    if(manager()->allWindows().count() > 0)
    {
        for(auto wnd : manager()->allWindows())
            windowCreated(wnd);
    }

    // connect up our signals
    if(manager())
    {
        connect(manager(), &PlasmaWindowManagement::activeChanged,
                this, &WindowTaskList::activeChanged);
        connect(manager(), &PlasmaWindowManagement::windowCreated,
                this, &WindowTaskList::windowCreated);
        connect(manager(), &PlasmaWindowManagement::stackingOrderChanaged,
                this, &WindowTaskList::stackingOrderChanaged);
    }

    connect(this, &WindowTaskList::tabBarClicked, this, &WindowTaskList::tabClicked);
}

WindowTaskList::~WindowTaskList()
{
    disconnect(manager(), &PlasmaWindowManagement::activeChanged,
            this, &WindowTaskList::activeChanged);
    disconnect(manager(), &PlasmaWindowManagement::windowCreated,
            this, &WindowTaskList::windowCreated);
    disconnect(manager(), &PlasmaWindowManagement::stackingOrderChanaged,
            this, &WindowTaskList::stackingOrderChanaged);
}

void WindowTaskList::activeChanged()
{

}

void WindowTaskList::windowCreated(PlasmaWindow *wnd)
{
    int id = addTab(wnd->icon(), wnd->windowTitle());
    setTabData(id, wnd->uuid());

    connect(wnd, &PlasmaWindow::iconChanged,
            this, &WindowTaskList::plasmaWindowIconChanged);
    connect(wnd, &PlasmaWindow::windowTitleChanged,
            this, &WindowTaskList::plasmaWindowTitleChanged);
    connect(wnd, &PlasmaWindow::minimizedChanged,
            this, &WindowTaskList::plasmaWindowMinimizedChanged);
    connect(wnd, &PlasmaWindow::windowClosed,
            this, &WindowTaskList::plasmaWindowClosed);
    connect(wnd, &PlasmaWindow::themeIconChanged,
            this, &WindowTaskList::plasmaThemeIconChanged);
    connect(wnd, &PlasmaWindow::activated,
            this, &WindowTaskList::activated);
    connect(wnd, &PlasmaWindow::deactivated,
            this, &WindowTaskList::deactivated);
    m_connectedWindows.append(wnd);
}

void WindowTaskList::stackingOrderChanaged()
{

}

void WindowTaskList::plasmaWindowIconChanged()
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    auto tabId = findTabByUuid(sndr->uuid());
    if(tabId >= 0)
        setTabIcon(tabId, sndr->icon());
}

void WindowTaskList::plasmaWindowTitleChanged(const QString &title)
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    auto tabId = findTabByUuid(sndr->uuid());
    if(tabId >= 0)
    {
        // double ampersands to distinguish them from mnemonics
        auto newtitle = title;
        newtitle.replace(QLatin1Char('&'), QLatin1String("&&"));
        setTabText(tabId, newtitle);
    }
}

void WindowTaskList::plasmaWindowMinimizedChanged()
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    auto tabId = findTabByUuid(sndr->uuid());
    if(tabId >= 0)
    {

    }
}

void WindowTaskList::plasmaWindowClosed()
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    auto tabId = findTabByUuid(sndr->uuid());
    if(tabId >= 0)
    {
        disconnect(sndr, &PlasmaWindow::iconChanged,
                this, &WindowTaskList::plasmaWindowIconChanged);
        disconnect(sndr, &PlasmaWindow::windowTitleChanged,
                this, &WindowTaskList::plasmaWindowTitleChanged);
        disconnect(sndr, &PlasmaWindow::minimizedChanged,
                this, &WindowTaskList::plasmaWindowMinimizedChanged);
        disconnect(sndr, &PlasmaWindow::windowClosed,
                this, &WindowTaskList::plasmaWindowClosed);
        disconnect(sndr, &PlasmaWindow::themeIconChanged,
                this, &WindowTaskList::plasmaThemeIconChanged);
        disconnect(sndr, &PlasmaWindow::activated,
                this, &WindowTaskList::activated);
        disconnect(sndr, &PlasmaWindow::deactivated,
                this, &WindowTaskList::deactivated);
        m_connectedWindows.removeOne(sndr);
        removeTab(tabId);
    }
}

void WindowTaskList::plasmaThemeIconChanged(const QString &icon)
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    auto tabId = findTabByUuid(sndr->uuid());
    if(tabId >= 0)
        setTabIcon(tabId, QIcon::fromTheme(icon, QIcon::fromTheme("wayland")));
}

void WindowTaskList::activated()
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    if(!sndr->uuid().isNull())
    {
        auto tab = findTabByUuid(sndr->uuid());
        setCurrentIndex(tab);
    }
}

void WindowTaskList::deactivated()
{
    auto sndr = qobject_cast<PlasmaWindow*>(sender());
    if(!sndr)
        return;

    setCurrentIndex(-1);
}

void WindowTaskList::tabClicked(int index)
{
    auto uuid = tabData(index).toUuid();
    if(!uuid.isNull())
    {
        auto wnd = manager()->windowByUUID(uuid);
        if(wnd)
            wnd->activate();
    }
}

PlasmaWindowManagement *WindowTaskList::manager()
{
    return StageApplication::instance()->windowManager();
}

int WindowTaskList::findTabByUuid(const QUuid &uuid) const
{
    for(int i = 0; i < count(); i++)
    {
        if(tabData(i).toUuid() == uuid)
            return i;
    }

    return -1;
}
