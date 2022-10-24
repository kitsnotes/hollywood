#include "stagehost.h"
#include "taskbutton.h"
#include "app.h"

#include <QButtonGroup>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

StageHost::StageHost(QScreen *screen, QWidget *parent)
    : QWidget(parent)
    , m_screen(screen)
    , m_menu(new QToolButton(this))
    , m_context(new QMenu(this))
    , m_group(new QButtonGroup(this))
{
    auto app = StageApplication::instance();
    setWindowFlag(Qt::FramelessWindowHint, true);
    m_menu->setIcon(QIcon::fromTheme("food-cake"));
    m_menu->setAutoRaise(true);
    m_menu->setPopupMode(QToolButton::InstantPopup);

    m_pm = new QToolButton(this);
    m_pm->setIcon(QIcon::fromTheme("applications-all"));
    m_pm->setAutoRaise(true);
    connect(m_pm, &QAbstractButton::pressed, app, &StageApplication::showProgramManager);
    setAlignment(Horizontal);

    auto aboutSys = m_context->addAction(tr("&About This Computer..."));
    m_context->addSeparator();
    auto settings = m_context->addAction(tr("System &Settings"));
    auto sysmon = m_context->addAction(tr("System &Monitor"));
    auto term = m_context->addAction(tr("&Terminull"));

    connect(settings, &QAction::triggered, app, &StageApplication::launchSettings);
    connect(sysmon, &QAction::triggered, app, &StageApplication::launchSysmon);
    connect(term, &QAction::triggered, app, &StageApplication::launchTerminull);
    m_context->addSeparator();
    auto logoff = m_context->addAction(tr("&Exit Session..."));
    connect(logoff, &QAction::triggered, app, &StageApplication::logoffSession);

    m_menu->setMenu(m_context);
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
        m_vbox->addWidget(m_pm);
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
        m_hbox->addWidget(m_pm);
        m_spacer = new QSpacerItem(1,1,QSizePolicy::Expanding);
        m_hbox->addItem(m_spacer);
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
    m_group->addButton(btn);
    connect(wnd, &PlasmaWindow::windowTitleChanged, btn, &QToolButton::setText);
    connect(wnd, &PlasmaWindow::windowClosed, this, &StageHost::windowClosed);
    if(m_align == Horizontal)
        m_hbox->insertWidget(m_hbox->indexOf(m_spacer), btn);
    else
        m_vbox->addWidget(btn);

    m_windows.append(btn);
}

void StageHost::show()
{
    QWidget::show();
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    m_lswnd->setAnchors(LayerShellQt::Window::AnchorBottom);
    m_ready = true;
    if(m_align == Horizontal)
        m_lswnd->setSize(QSize(0, size().height()));
    else
        m_lswnd->setSize(QSize(size().width(), 0));
}

void StageHost::stackingOrderChanged()
{
    qDebug() << "stacking order chanaged" << StageApplication::instance()->windowManager()->stackingOrder();
    for(auto btn : m_windows)
        btn->setDown(false);

    for (auto &uuid : StageApplication::instance()->windowManager()->stackingOrder())
    {
        for(auto btn : m_windows)
            if(btn->window()->uuid().toString(QUuid::WithoutBraces) == uuid)
                btn->setDown(true);
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
