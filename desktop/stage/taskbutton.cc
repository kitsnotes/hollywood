#include "taskbutton.h"
#include "wndmgmt.h"

#include <QMenu>

TaskButton::TaskButton(PlasmaWindow *wnd, QWidget *parent)
    :QToolButton(parent)
    ,m_window(wnd)
    ,m_context(new QMenu(this))
{
    QFont myFont = font();
    myFont.setPointSize(myFont.pointSize()+1);
    setFont(myFont);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(wnd, &PlasmaWindow::themeIconChanged,
            this, &TaskButton::themeIconChanged);
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_restore = m_context->addAction(tr("&Restore"));
    m_move = m_context->addAction(tr("&Move"));
    m_size = m_context->addAction(tr("&Size"));
    m_minimize = m_context->addAction(tr("Mi&nimize"));
    m_maximize = m_context->addAction(tr("M&aximize"));
    m_context->addSeparator();
    m_close = m_context->addAction(tr("&Close Window"));

    connect(wnd, &PlasmaWindow::windowTitleChanged,
            this, &TaskButton::titleChanged);

    connect(this, &TaskButton::customContextMenuRequested,
            this, &TaskButton::displayContextMenu);
}

PlasmaWindow *TaskButton::window()
{
    return m_window;
}

void TaskButton::themeIconChanged(const QString &icon)
{
    setIcon(QIcon::fromTheme(icon));
}

void TaskButton::displayContextMenu(const QPoint &pos)
{
    m_context->popup(pos);
}

void TaskButton::titleChanged(const QString &title)
{
    // double ampersands to distinguish them from mnemonics
    auto newtitle = title;
    newtitle.replace(QLatin1Char('&'), QLatin1String("&&"));
    this->setText(newtitle);
}

void TaskButton::restore()
{

}

void TaskButton::move()
{

}

void TaskButton::size()
{

}

void TaskButton::minimize()
{

}

void TaskButton::maximize()
{

}

void TaskButton::close()
{

}
