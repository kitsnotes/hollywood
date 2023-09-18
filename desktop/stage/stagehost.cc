#include "stagehost.h"
#include "taskbutton.h"
#include "app.h"
#include "stageclock.h"
#include "surfacemanager.h"
#include "notifierhost.h"
#include "battery.h"
#include <QtWaylandClient/private/qwaylandwindow_p.h>

StageHost::StageHost(QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_menu(new QToolButton(this))
    , m_sm(new SurfaceManager(this))
{
    m_spacer = new QSpacerItem(1,1,QSizePolicy::Expanding);
    m_trayspacer = new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(m_sm, &SurfaceManager::windowCreated, this, &StageHost::createWindowButton);
    connect(m_sm, &SurfaceManager::windowDestroyed, this, &StageHost::windowClosed);

    auto app = StageApplication::instance();
    setWindowFlag(Qt::FramelessWindowHint, true);
    m_menu->setIcon(QIcon::fromTheme("hollywood-logo"));
    m_menu->setAutoRaise(true);
    m_menu->setPopupMode(QToolButton::InstantPopup);
    m_menu->setToolTip(tr("Launch applications and control your system."));
    m_menu->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    m_menu->setMenu(app->systemMenu());
    m_menu->setArrowType(Qt::NoArrow);
    setAlignment(Horizontal);
    setMinimumWidth(650);

    QFont font = m_menu->font();
    font.setPointSize(font.pointSize()+1);
    if(m_clock)
        m_clock->setFont(font);
    m_menu->setFont(font);
}

void StageHost::setAlignment(Alignment align)
{
    switch(align)
    {
    case Vertical:
        if(m_hbox)
            m_hbox->deleteLater();

        m_align = align;
        m_vbox = new QVBoxLayout(this);
        setLayout(m_vbox);
        if(!StageApplication::instance()->isSouthernMode())
            m_vbox->addWidget(m_menu);
        setMaximumWidth(160);
        setMaximumHeight(99999);
        resize(650, m_screen->availableSize().height());

        break;
    case Horizontal:
    default:
        if(m_vbox)
            m_vbox->deleteLater();

        m_align = align;
        m_hbox = new QHBoxLayout(this);
        m_hbox->setSpacing(0);
        m_hbox->setContentsMargins(0,0,0,0);
        if(!StageApplication::instance()->isSouthernMode())
        {
            m_hbox->addWidget(m_menu);
            m_hbox->addItem(m_spacer);
            m_hbox->addSpacerItem(m_trayspacer);
            m_hbox->addWidget(m_battery);
            m_hbox->addWidget(m_clock);
            m_menu->setVisible(true);
        }
        else
        {
            //m_hbox->addWidget(m_menu);
            m_hbox->addItem(m_spacer);
            m_hbox->addSpacerItem(m_trayspacer);
            m_menu->setVisible(false);
            m_hbox->addWidget(new QWidget());
        }
        //m_hbox->addWidget(m_shutdown);
        setLayout(m_hbox);
        setMinimumHeight(32);
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
    if(m_align == Horizontal)
        m_hbox->addWidget(m_clock);
    else
        m_vbox->addWidget(m_clock);
}

void StageHost::setBattery(BatteryMonitor *battery)
{
    m_battery = battery;
}

void StageHost::takeBattery()
{
    m_battery->setParent(this);
    if(m_align == Horizontal)
        m_hbox->addWidget(m_battery);
    else
        m_vbox->addWidget(m_battery);
}

void StageHost::setMainEnabled(bool enabled)
{
    if(m_menu)
        m_menu->setVisible(enabled);
}

void StageHost::createWindowButton(TaskButton *btn)
{
    if(m_align == Horizontal)
        m_hbox->insertWidget(m_hbox->indexOf(m_spacer), btn);
    else
        m_vbox->addWidget(btn);
}

void StageHost::show()
{
    setMinimumWidth(StageApplication::primaryScreen()->geometry().width());
    QWidget::show();
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    //m_lswnd->setAnchors(LayerShellQt::Window::AnchorBottom);
    m_lswnd->setAnchors((LayerShellQt::Window::Anchors)LayerShellQt::Window::AnchorBottom|LayerShellQt::Window::AnchorLeft|LayerShellQt::Window::AnchorRight);
    m_lswnd->setExclusiveZone(size().height());

    m_ready = true;
    if(m_align == Horizontal)
        m_lswnd->setSize(QSize(size().width(), size().height()));
    else
        m_lswnd->setSize(QSize(size().width(), 0));
}

void StageHost::windowClosed(TaskButton *btn)
{
    if(m_align == Horizontal)
        m_hbox->removeWidget(btn);
    else
        m_vbox->removeWidget(btn);
}

void StageHost::createStatusButton(StatusNotifierButton *btn)
{
    if(m_align == Horizontal)
        m_hbox->insertWidget(m_hbox->indexOf(m_spacer)+1, btn);
    else
        m_vbox->addWidget(btn);
}

void StageHost::statusButtonRemoved(StatusNotifierButton *btn)
{
    if(m_align == Horizontal)
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
