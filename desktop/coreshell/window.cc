#include "window.h"
#include "app.h"
#include "wndmgmt.h"

#include <QScreen>

CSWindow::CSWindow(QScreen *screen)
    : QWidget(nullptr),
      m_screen(screen),
      m_view(new QListView(this))
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint
                   | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setViewMode(QListView::IconMode);
    m_view->setIconSize(QSize(32,32));
    m_view->setSpacing(20);
    m_view->setFlow(QListView::TopToBottom);
    m_view->setUniformItemSizes(false);
    m_view->setWordWrap(true);
    m_view->setItemAlignment(Qt::AlignHCenter);
    m_view->setFrameShape(QFrame::NoFrame);

    QPalette p = m_view->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    m_view->setPalette(p);

    m_view->resize(m_screen->geometry().width(),
                   m_screen->geometry().height());

    connect(m_view, &QAbstractItemView::activated,
            this, &CSWindow::activated);

    connect(m_view, &QAbstractItemView::customContextMenuRequested,
            this, &CSWindow::contextMenu);

    connect(screen, &QScreen::geometryChanged,
            this, &CSWindow::geometryChanged);

    // TODO: filter model
    m_view->setModel(CSApplication::instance()->model());
}

CSWindow::~CSWindow()
{
}

QScreen *CSWindow::screen()
{
    return m_screen;
}

void CSWindow::geometryChanged(const QRect &geom)
{
    resize(geom.width(), geom.height());
    m_view->resize(geom.width(), geom.height());
}

void CSWindow::activated(const QModelIndex &idx)
{
    Q_UNUSED(idx);
    auto model = CSApplication::instance()->model();
    auto item = model->itemFromIndex(idx);
    auto datastr = item->data().toString();
    if(datastr == "Window")
    {
        auto uuid = QUuid::fromString(item->data(Qt::UserRole+2).toString());
        auto w = CSApplication::instance()->
                windowManager()->windowByUUID(uuid);

        w->toggleMinimize();
        model->removeRow(idx.row());
        delete item;
        return;
    }
}

void CSWindow::contextMenu(const QPoint &pos)
{
    CSApplication::instance()->contextMenu()->popup(pos);
}
