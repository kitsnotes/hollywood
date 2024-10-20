// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "notifywindow.h"
#include "notification.h"
#include <QPainterPath>
#include <hollywood/hollywood.h>
#include <QPainter>
#include <QPaintEvent>
#include <QApplication>
#include "hollywood/layershellwindow.h"

NotificationWindow::NotificationWindow(Notification *parent)
    : QWidget(0)
    , m_parent(parent)
    , m_layout(new QHBoxLayout(this))
    , m_icon(new QLabel(this))
    , m_title(new QLabel(this))
    , m_details(new QLabel(this))
    , m_close(new QToolButton(this))
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::WindowDoesNotAcceptFocus);
    setMinimumWidth(350);
    setMaximumWidth(350);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(10,10,10,10);
    QFont font = m_title->font();
    font.setBold(true);
    font.setPointSize(font.pointSize()+1);
    m_title->setFont(font);

    m_close->setIcon(QIcon::fromTheme("window-close"));
    m_close->setAutoRaise(true);
    m_close->setIconSize(QSize(24,24));
    connect(m_close, &QToolButton::pressed, this, &NotificationWindow::closeRequested);

    m_icon->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_title->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_details->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_details->setOpenExternalLinks(true);
    auto iconLayout = new QVBoxLayout;
    iconLayout->addWidget(m_icon);
    iconLayout->setAlignment(m_icon, Qt::AlignTop|Qt::AlignHCenter);
    iconLayout->setContentsMargins(0,0,0,0);

    auto textlayout = new QVBoxLayout;
    textlayout->addWidget(m_title);
    textlayout->addWidget(m_details);
    textlayout->setAlignment(m_title, Qt::AlignTop|Qt::AlignLeft);
    textlayout->setAlignment(m_details, Qt::AlignTop|Qt::AlignLeft);
    textlayout->setSpacing(0);
    textlayout->setContentsMargins(0,0,0,0);

    auto actionLayout = new QVBoxLayout;
    actionLayout->addWidget(m_close);
    actionLayout->setAlignment(m_close, Qt::AlignTop|Qt::AlignRight);
    actionLayout->setSpacing(0);
    actionLayout->setContentsMargins(0,0,0,0);

    m_layout->addLayout(iconLayout);
    m_layout->addLayout(textlayout,1);
    m_layout->addLayout(actionLayout);

    m_title->setText(m_parent->m_summary);
    m_details->setText(m_parent->m_body);
    if(!m_parent->m_app_icon.isEmpty())
    {
        QIcon ico = QIcon::fromTheme(m_parent->m_app_icon);
        if(!ico.isNull())
            m_icon->setPixmap(ico.pixmap(48,48));
    }
    m_details->setTextFormat(Qt::RichText);
}

NotificationWindow::~NotificationWindow()
{
}

void NotificationWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QPalette pal = QApplication::palette();

    auto bg = pal.base().color();
    bg.setAlpha(230);

    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), QColor(255,255,255,0));

    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QRect wg = QRect(QPoint(0,0),
             QPoint(size().width(),size().height()));

    QRect clips[] =
    {
        // left top width height
        QRect(wg.left(), wg.top(), wg.width(), wg.height()), // title bar
        QRect(wg.left(), wg.bottom(), wg.width(), wg.height()), // bottom line
        QRect(wg.left(), 0, 0, wg.height()), // left line
        QRect((wg.right()), wg.top(), 0, wg.height()) // right line
    };

    p.setRenderHint(QPainter::Antialiasing);

    // Title bar
    QPainterPath roundedRect;
    roundedRect.addRoundedRect(wg, 11, 11);
    for (int i = 0; i < 4; ++i) {
        p.save();
        p.setClipRect(clips[i]);
        p.fillPath(roundedRect, bg);
        p.restore();
    }
    p.save();
    wg.adjust(1,1,2,-1);
    p.restore();
    event->accept();
}

