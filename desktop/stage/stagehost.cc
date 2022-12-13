#include "stagehost.h"
#include "taskbutton.h"
#include "app.h"
#include "stageclock.h"
#include "surfacemanager.h"
#include "notifierhost.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>

StageHost::StageHost(QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_menu(new QToolButton(this))
    , m_sm(new SurfaceManager(this))
{
    connect(m_sm, &SurfaceManager::windowCreated, this, &StageHost::createWindowButton);
    connect(m_sm, &SurfaceManager::windowDestroyed, this, &StageHost::windowClosed);

    auto app = StageApplication::instance();
    connect(app, &StageApplication::clockSettingsChanged, m_clock, &StageClock::clockSettingsChanged);
    setWindowFlag(Qt::FramelessWindowHint, true);
    m_menu->setIcon(QIcon::fromTheme("food-cake"));
    m_menu->setAutoRaise(true);
    m_menu->setPopupMode(QToolButton::InstantPopup);
    m_menu->setToolTip(tr("Launch applications and control your system."));
    m_menu->setStyleSheet("QToolButton::menu-indicator { image: none; }");
    m_menu->setMenu(app->systemMenu());
    m_menu->setArrowType(Qt::NoArrow);
    setAlignment(Horizontal);

    QFont font = m_menu->font();
    font.setPointSize(font.pointSize()+1);
    if(m_clock)
        m_clock->setFont(font);
    m_menu->setFont(font);
    //connect(m_notifier, &NotifierHost::buttonAdded, this, &StageHost::createStatusButton);
    //connect(m_notifier, &NotifierHost::buttonRemoved, this, &StageHost::statusButtonRemoved);
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
        resize(150, m_screen->availableSize().height());

        break;
    case Horizontal:
    default:
        if(m_vbox)
            m_vbox->deleteLater();

        m_align = align;
        m_hbox = new QHBoxLayout(this);
        m_hbox->setContentsMargins(0,0,0,0);
        m_spacer = new QSpacerItem(1,20,QSizePolicy::Expanding);
        if(!StageApplication::instance()->isSouthernMode())
        {
            m_hbox->addWidget(m_menu);
            m_hbox->addItem(m_spacer);
            if(m_clock)
                m_hbox->addWidget(m_clock);
        }
        else
        {
            m_hbox->addItem(m_spacer);
            m_menu->setVisible(false);
        }
        //m_hbox->addWidget(m_shutdown);
        setLayout(m_hbox);
        setMinimumHeight(1);
        setMaximumHeight(30);
        setMaximumWidth(99999);
        setMinimumWidth(300);
        break;
    }
}

void StageHost::setClock(StageClock *clock)
{
    m_clock = clock;
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
    QWidget::show();
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    m_lswnd->setAnchors(LayerShellQt::Window::AnchorBottom);
    m_lswnd->setExclusiveZone(size().height());
    m_ready = true;
    if(m_align == Horizontal)
        m_lswnd->setSize(QSize(0, size().height()));
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
