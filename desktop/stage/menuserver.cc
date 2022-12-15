#include "menuserver.h"
#include "app.h"
#include "stageclock.h"
#include "notifierhost.h"
#include "battery.h"
#include "client/privateprotocol.h"

MenuServer::MenuServer(StageClock *clock, BatteryMonitor *battery, QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_layout(new QHBoxLayout(this))
    , m_menuBar(new QMenuBar(0))
    , m_spacer(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding))
    , m_trayspacer(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding))
    , m_clock(clock)
    , m_battery(battery)
    , vl_opposite(new QHBoxLayout)
{
    setWindowFlag(Qt::FramelessWindowHint, true);
    setMinimumHeight(1);
    setMaximumHeight(25);
    setMaximumWidth(99999);
    setMinimumWidth(300);
    //m_opposite->setMaximumHeight(25);
    m_menuBar->addMenu(StageApplication::instance()->systemMenu());
    m_menuBar->setStyleSheet("QMenuBar { height: 20px; }  ");
    m_menuBar->setFixedHeight(this->size().height());
    m_menuBar->setMinimumHeight(size().height());
    // TODO: RTL Layout
    m_layout->addWidget(m_menuBar);
    m_layout->setStretch(0,1);
    m_layout->addSpacerItem(m_spacer);
    m_layout->addLayout(vl_opposite);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0,0,0,0);
    vl_opposite->setSpacing(0);
    vl_opposite->setContentsMargins(0,0,0,0);
    clock->setParent(this);
    vl_opposite->addSpacerItem(m_trayspacer);
    vl_opposite->addWidget(battery);
    vl_opposite->addWidget(clock);
}

void MenuServer::installMenu(QMenu *menu)
{
    cleanMenu();
    if(menu == nullptr)
        return;

    for(auto a : menu->actions())
    {
        auto m = QMenu::menuInAction(a);
        if(m != nullptr)
            m_menuBar->addMenu(m);
        else
            m_menuBar->addAction(a);
    }
}

void MenuServer::cleanMenu()
{
    for(auto a : m_menuBar->actions())
    {
        if(a->data() != 9195521)
            m_menuBar->removeAction(a);
    }
}

void MenuServer::show()
{
    QWidget::show();
    StageApplication::instance()->privateProtocolReady();

    m_lswnd = LayerShellQt::Window::get(windowHandle());
    m_lswnd->setAnchors(LayerShellQt::Window::AnchorTop);
    m_lswnd->setExclusiveZone(size().height());
    m_ready = true;
    m_lswnd->setSize(QSize(0, size().height()));
    m_menuBar->setFixedHeight(this->size().height());
    m_menuBar->setMinimumHeight(size().height());
}

void MenuServer::createStatusButton(StatusNotifierButton *btn)
{
    auto idx = vl_opposite->indexOf(m_trayspacer);
    vl_opposite->insertWidget(vl_opposite->indexOf(m_trayspacer), btn);
}

void MenuServer::statusButtonRemoved(StatusNotifierButton *btn)
{
    vl_opposite->removeWidget(btn);
}
