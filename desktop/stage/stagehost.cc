#include "stagehost.h"
#include "taskbutton.h"
#include "app.h"
#include "stageclock.h"

#include <QButtonGroup>
#include <QtWaylandClient/private/qwaylandwindow_p.h>


StageHost::StageHost(QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_menu(new QToolButton(this))
    , m_group(new QButtonGroup(this))
    , m_clock(new StageClock(this))
{
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
    m_clock->setFont(font);
    m_menu->setFont(font);

    m_group->setExclusive(true);
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
        m_hbox->addWidget(m_menu);
        m_spacer = new QSpacerItem(1,1,QSizePolicy::Expanding);
        m_hbox->addItem(m_spacer);
        m_hbox->addWidget(m_clock);
        //m_hbox->addWidget(m_shutdown);
        setLayout(m_hbox);
        setMinimumHeight(1);
        setMaximumHeight(30);
        setMaximumWidth(99999);
        setMinimumWidth(300);
        break;
    }
}

void StageHost::createWindowButton(PlasmaWindow *wnd)
{
    auto btn = new TaskButton(wnd, this);
    btn->setText(wnd->windowTitle());
    btn->setToolTip(wnd->windowTitle());
#ifdef QT_DEBUG
    btn->setToolTip(wnd->uuid().toString());
#endif
    connect(wnd, &PlasmaWindow::windowClosed, this, &StageHost::windowClosed);
    connect(wnd, &PlasmaWindow::minimizedChanged, this, &StageHost::minimizedChanged);
    connect(wnd, &PlasmaWindow::destroyed, btn, &TaskButton::deleteLater);


    if(m_align == Horizontal)
        m_hbox->insertWidget(m_hbox->indexOf(m_spacer), btn);
    else
        m_vbox->addWidget(btn);

    m_group->addButton(btn);
    m_windows.append(btn);
    connect(btn, &QAbstractButton::pressed, this, &StageHost::buttonClicked);
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

void StageHost::stackingOrderChanged()
{
    //qDebug() << "stacking order chanaged" << StageApplication::instance()->windowManager()->stackingOrder();

    for (auto &uuid : StageApplication::instance()->windowManager()->stackingOrder())
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

void StageHost::buttonClicked()
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

void StageHost::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(!m_ready)
        return;
    m_lswnd->setSize(size());
}

void StageHost::windowClosed()
{
    qDebug() << "windowClosed";
    auto pm = qobject_cast<PlasmaWindow*>(sender());

    if(!pm)
        return;

    for(auto btn : m_windows)
    {
        if(btn->window() == pm)
        {
            m_group->removeButton(btn);
            connect(pm, &PlasmaWindow::windowTitleChanged, btn, &QToolButton::setText);
            if(m_align == Horizontal)
                m_hbox->removeWidget(btn);
            else
                m_vbox->removeWidget(btn);

            m_windows.removeOne(btn);
            btn->deleteLater();
        }
    }
}

void StageHost::minimizedChanged()
{
    auto pm = qobject_cast<PlasmaWindow*>(sender());

    qDebug() << "minimizedChanged" << pm->uuid() << pm->minimized();
}
