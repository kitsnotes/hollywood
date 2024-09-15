// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "stagehost.h"
#include "taskview/windowlist.h"
#include "stage.h"
#include "stageclock.h"
#include "surfacemanager.h"
#include "statusnotifier/notifierhost.h"
#include "battery.h"
#include "taskview/windowlist.h"
#include "taskview/appbutton.h"
#include <QtWaylandClient/private/qwaylandwindow_p.h>

StageHost::StageHost(QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_menu(new QToolButton(this))
{
    m_hspacer = new QSpacerItem(1,1,QSizePolicy::Minimum);
    m_vspacer = new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding);
    m_trayspacer = new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::Fixed);

    auto app = StageApplication::instance();
    setWindowFlag(Qt::FramelessWindowHint, true);
    auto iconsize = QSize(32,32);
    iconsize *= window()->devicePixelRatio();
    m_menu->setIcon(QIcon::fromTheme("hollywood-logo"));
    m_menu->setAutoRaise(true);
    m_menu->setPopupMode(QToolButton::InstantPopup);
    //m_menu->setToolTip(tr("Launch applications and control your system."));
    m_menu->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    m_menu->setMenu(app->systemMenu());
    m_menu->setArrowType(Qt::NoArrow);
    /* csz = QSize(textWidth + iconSize.width() + hframe + widgetWidth + padding,
                qMax(maxWidgetHeight, qMax(fm.height(), iconSize.height())) + vframe); */

    QFont font = m_menu->font();
    auto size = font.pointSize()+1;
    size *= window()->devicePixelRatio();    
    font.setPointSize(size);

    if(m_clock)
        m_clock->setFont(font);
    m_menu->setFont(font);

    setAlignment(Qt::Horizontal);
    switchToWindowList();
}

void StageHost::setAlignment(Qt::Orientation align)
{
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    switch(align)
    {
    case Qt::Vertical:
        if(m_hbox)
        {
            m_hbox->removeWidget(m_menu);
            m_hbox->removeWidget(m_windowList);

            m_hbox->deleteLater();
        }

        m_align = align;
        m_vbox = new QVBoxLayout(this);
        if(!StageApplication::instance()->isSouthernMode())
            m_vbox->addWidget(m_menu);
        m_vbox->addItem(m_vspacer);
        m_menu->setText("Hollywood");
        if(m_windowList)
        {
            m_vbox->insertWidget(m_vbox->indexOf(m_vspacer), m_windowList);
            m_windowList->setVertical(true);
        }

        setMaximumWidth(160);
        setMaximumHeight(m_screen->availableSize().height());
        setMinimumHeight(m_screen->availableSize().height());
        resize(650, m_screen->availableSize().height());
        if(StageApplication::instance()->dockEdge() == Qt::LeftEdge)
            m_lswnd->setAnchors(LayerShellQt::Window::AnchorLeft);
        else
            m_lswnd->setAnchors(LayerShellQt::Window::AnchorRight);
        break;
        setLayout(m_vbox);
    case Qt::Horizontal:
    default:
        if(m_vbox)
            m_vbox->deleteLater();

        m_align = align;
        m_hbox = new QHBoxLayout(this);
        m_hbox->setSpacing(1);
        m_hbox->setContentsMargins(0,0,0,0);
        m_menu->setText("");
        if(StageApplication::instance()->dockEdge() == Qt::TopEdge)
            m_lswnd->setAnchors(LayerShellQt::Window::AnchorTop);
        else
            m_lswnd->setAnchors(LayerShellQt::Window::AnchorBottom);
        if(m_windowList)
            m_windowList->setVertical(false);

        if(!StageApplication::instance()->isSouthernMode())
        {
            m_hbox->addWidget(m_menu);
            m_hbox->addItem(m_hspacer);
            m_hbox->addSpacerItem(m_trayspacer);
            m_hbox->addWidget(m_battery);
            m_hbox->addWidget(m_clock);
            m_menu->setVisible(true);
        }
        else
        {
            //m_hbox->addWidget(m_menu);
            m_hbox->addItem(m_hspacer);
            m_hbox->addSpacerItem(m_trayspacer);
            m_menu->setVisible(false);
            m_hbox->addWidget(new QWidget());
        }
        setLayout(m_hbox);
        auto minheight = 30;
        minheight *= window()->devicePixelRatio();
        setMinimumHeight(minheight);
        setMaximumWidth(99999);
        setMinimumWidth(650);
        break;
    }
}

void StageHost::setClock(StageClock *clock)
{
    m_clock = clock;
}

void StageHost::takeClock()
{
    m_clock->setParent(this);
    if(m_align == Qt::Horizontal)
        m_hbox->addWidget(m_clock);
    else
        m_vbox->addWidget(m_clock);
    m_clock->updateDpiAwareSettings();
}

void StageHost::setBattery(BatteryMonitor *battery)
{
    m_battery = battery;
}

void StageHost::takeBattery()
{
    m_battery->setParent(this);
    if(m_align == Qt::Horizontal)
        m_hbox->addWidget(m_battery);
    else
        m_vbox->addWidget(m_battery);
    m_battery->updateDpiAwareSettings();
}

void StageHost::setMainEnabled(bool enabled)
{
    if(m_menu)
        m_menu->setVisible(enabled);
}

void StageHost::switchToWindowList()
{
    m_windowList = new WindowTaskList(this);
    if(m_align == Qt::Horizontal)
    {
        m_windowList->setVertical(false);
        m_hbox->insertWidget(m_hbox->indexOf(m_hspacer), m_windowList);
    }
    else
    {
        m_windowList->setVertical(true);
        m_vbox->insertWidget(m_vbox->indexOf(m_vspacer), m_windowList);
    }
}

void StageHost::show()
{
    setMinimumWidth(StageApplication::primaryScreen()->geometry().width());
    QWidget::show();
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    //m_lswnd->setAnchors((LayerShellQt::Window::Anchors)LayerShellQt::Window::AnchorBottom|LayerShellQt::Window::AnchorLeft|LayerShellQt::Window::AnchorRight);
    if(m_align == Qt::Horizontal)
    {
        m_lswnd->setAnchors(LayerShellQt::Window::AnchorBottom);
        m_lswnd->setExclusiveZone(size().height());
        m_lswnd->setSize(QSize(size().width(), size().height()));
    }
    else
    {
        m_lswnd->setAnchors(LayerShellQt::Window::AnchorRight);
        m_lswnd->setExclusiveZone(160);
        m_lswnd->setSize(QSize(160, size().height()));
    }

    m_ready = true;
}

void StageHost::createStatusButton(StatusNotifierButton *btn)
{
    if(m_align == Qt::Horizontal)
        m_hbox->insertWidget(m_hbox->indexOf(m_hspacer)+1, btn);
    else
        m_vbox->insertWidget(m_vbox->indexOf(m_vspacer)+1, btn);
}

void StageHost::statusButtonRemoved(StatusNotifierButton *btn)
{
    if(m_align == Qt::Horizontal)
        m_hbox->removeWidget(btn);
    else
        m_vbox->removeWidget(btn);
}

void StageHost::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(!m_ready)
        return;
    m_lswnd->setSize(size());

}
