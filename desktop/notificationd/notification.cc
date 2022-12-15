// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "notification.h"
#include "notifywindow.h"
#include "daemon.h"
#include <QApplication>
#include <QScreen>
#include <QPainterPath>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <hollywood/hollywood.h>

Notification::Notification(const uint id, const QString &app_name, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout, bool noSave, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_app_name(app_name)
    , m_app_icon(app_icon)
    , m_summary(summary)
    , m_body(body)
    , m_actions(actions)
    , m_hints(hints)
    , m_timeout(expire_timeout)
    , m_nosave(noSave)
    , m_wnd(new NotificationWindow(this))
{
    connect(m_wnd, &NotificationWindow::closeRequested, this, &Notification::closeRequested);
    connect(m_wnd, &NotificationWindow::dismissed, this, &Notification::dismiss);

    if(m_hints.contains("desktop-entry"))
        m_desktop_file = m_hints.value("desktop-entry").toString();

    if(m_hints.contains("urgency"))
    {
        auto urgency = m_hints.value("urgency").toUInt();
        if(urgency > 2)
            urgency = 1;
        m_urgency = (Urgency)urgency;
    }

    if(m_hints.contains("supress-sound"))
        m_nosound = m_hints.value("supress-sound").toBool();

    if(m_timeout == -1)
    {
        if(m_urgency == Critical)
            m_timeout = 0;
        if(m_urgency == Normal)
            m_timeout = HOLLYWOOD_NN_TIMEOUT;
        if(m_urgency == Low)
            m_timeout = HOLLYWOOD_LN_TIMEOUT;
    }

    if(m_timeout > 0)
    {
        //setup our timer
        auto timer = new QTimer(this);
        timer->setInterval(m_timeout);
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, &Notification::expired);
        connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
        timer->start();
    }
}

void Notification::show()
{
    auto screen = QApplication::primaryScreen();
    m_wnd->setScreen(screen);
    m_wnd->show();
    int margin = 10;
    if(NotificationDaemon::instance()->southernMode())
    {
        int x = screen->size().width() - m_wnd->width() - margin;
        int y = 40;
        for(auto n : NotificationDaemon::instance()->openNotifications())
        {
            if(n == this)
                continue;

            y = y + n->notificationHeight() + 10;
        }
        m_wnd->move(x,y);
    }
    else
    {
        int x = screen->size().width() - m_wnd->width() - margin;
        int y = screen->size().height() - 30 - m_wnd->height();
        for(auto n : NotificationDaemon::instance()->openNotifications())
        {
            if(n == this)
                continue;

            y = y - n->notificationHeight() - 10;
        }
        m_wnd->move(x,y);
    }
    m_size = m_wnd->size();
    m_position = m_wnd->pos();
    m_wnd->setVisible(false);
    animateOpen();
}

uint Notification::notificationHeight() const
{
    return m_size.height();
}

void Notification::actionSelected(QVariant action)
{
    // TODO: support actions
    Q_UNUSED(action);
}

void Notification::animateOpen()
{
    auto minwidth = m_wnd->minimumWidth();
    m_wnd->setMinimumWidth(1);
    auto group = new QParallelAnimationGroup;
    auto size_animation = new QPropertyAnimation(m_wnd, "size");
    auto pos_animation = new QPropertyAnimation(m_wnd, "pos");
    QSize startSize = m_size;
    startSize.setWidth(1);
    m_wnd->resize(startSize);
    m_wnd->setVisible(true);
    QSize endSize = m_size;
    QPoint startPoint = m_position;
    startPoint.setX(startPoint.x()+m_size.width());
    m_wnd->move(startPoint);
    QPoint endPoint = m_position;
    size_animation->setStartValue(startSize);
    size_animation->setEndValue(endSize);
    size_animation->setDuration(190);
    size_animation->setEasingCurve(QEasingCurve::InOutQuad);

    pos_animation->setStartValue(startPoint);
    pos_animation->setEndValue(endPoint);
    pos_animation->setDuration(190);
    pos_animation->setEasingCurve(QEasingCurve::InOutQuad);

    group->addAnimation(pos_animation);
    group->addAnimation(size_animation);

    group->start(QAbstractAnimation::DeleteWhenStopped);
    connect(group, &QParallelAnimationGroup::finished,[this, group, pos_animation, size_animation, minwidth](){
        m_wnd->setMinimumWidth(minwidth);
        size_animation->deleteLater();
        pos_animation->deleteLater();
        group->deleteLater();
    });
}
