#include "menuserver.h"
#include "app.h"
#include "stageclock.h"
#include "notifierhost.h"
#include "battery.h"
#include "client/privateprotocol.h"

#include <dbusmenuimporter.h>

using namespace LayerShellQt;

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
    m_clock->setParent(this);
    m_battery->setParent(this);
    m_clock->updateDpiAwareSettings();
    m_battery->updateDpiAwareSettings();

    setWindowFlag(Qt::FramelessWindowHint, true);
    setMinimumHeight(1);
    setMaximumHeight(30);
    auto w = StageApplication::primaryScreen()->geometry().width();
    w *= window()->devicePixelRatio();
    setMinimumWidth(w);
    QFont font = m_menuBar->font();
    auto size = font.pointSize()+1;
    size *= window()->devicePixelRatio();
    font.setPointSize(size);
    QFontMetrics m(font);
    auto minheight = m.height()+8;
    setMinimumHeight(minheight);
    setMaximumHeight(minheight);

    qDebug() << "minimumheight" << minheight;
    //m_opposite->setMaximumHeight(25);
    
    m_menuBar->addMenu(StageApplication::instance()->systemMenu());
    
    m_menuBar->setStyleSheet(QString("QMenuBar { height: %1px; }  ").arg(QString::number(minheight-7)));
    m_menuBar->setFixedHeight(this->size().height());
    m_menuBar->setFont(font);
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
        {
            m_menuBar->addMenu(m);
            connect(m, &QMenu::aboutToShow, [this](){
                activateWindow();
                setFocus();
            });
            auto c = connect(m, &QMenu::triggered, [](QAction *act){
                qDebug() << act->text();
            });
            m_connections.insert(m, c);
        }
        else
            m_menuBar->addAction(a);
    }
}

void MenuServer::cleanMenu()
{
    for(auto a : m_menuBar->actions())
    {
        if(a->data() != 9195521)
        {
            auto m = QMenu::menuInAction(a);
            auto c= m_connections.value(m);
            disconnect(m, &QMenu::triggered, nullptr, nullptr);
            disconnect(m, &QMenu::aboutToShow, nullptr, nullptr);
            m_menuBar->removeAction(a);
        }
    }
}

void MenuServer::show()
{
    QWidget::show();
    StageApplication::instance()->privateProtocolReady();

    m_lswnd = Window::get(windowHandle());
    m_lswnd->setAnchors((Window::Anchors)Window::AnchorTop);
//    m_lswnd->setAnchors((Window::Anchors)Window::AnchorTop|LayerShellQt::Window::AnchorLeft|Window::AnchorRight);
    m_lswnd->setExclusiveZone(size().height());
    m_lswnd->setSize(QSize(size().width(), size().height()));
    m_ready = true;
    m_menuBar->setFixedHeight(this->size().height());
    m_menuBar->setMinimumHeight(size().height());
}

void MenuServer::createStatusButton(StatusNotifierButton *btn)
{
    vl_opposite->insertWidget(vl_opposite->indexOf(m_trayspacer), btn);
}

void MenuServer::statusButtonRemoved(StatusNotifierButton *btn)
{
    vl_opposite->removeWidget(btn);
}
