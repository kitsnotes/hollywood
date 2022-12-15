// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "daemon.h"
#include "notifywindow.h"
#include "notification.h"
#include "adaptor.h"
#include <QDBusConnection>

#include <hollywood/hollywood.h>

NotificationDaemon::NotificationDaemon(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_configwatcher(new QFileSystemWatcher(this))
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setApplicationName("Notification Daemon");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);

    setQuitOnLastWindowClosed(false);
    loadSettings();
    m_configwatcher->addPath(m_configfile);
    connect(m_configwatcher, &QFileSystemWatcher::fileChanged,
            this, &NotificationDaemon::loadSettings);

    new NotificationsAdaptor(this);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.registerService(QStringLiteral("org.freedesktop.Notifications")))
    {
        qDebug() << "registerService failed: another service with 'org.freedesktop.Notifications' is already running";
        exit(-1);
    }
    else
    {
        if (!connection.registerObject(QStringLiteral("/org/freedesktop/Notifications"), this))
        {
            qDebug() << "registerObject failed: another object with '/org/freedesktop/Notifications' is already running";
            connection.unregisterService("org.freedesktop.Notifications");
        }
    }
}

QList<Notification *> NotificationDaemon::openNotifications()
{
    return m_notifications.values();
}

void NotificationDaemon::CloseNotification(uint id)
{
    auto n = m_notifications[id];
    if(n)
        animateCloseNotification(n);
}

QStringList NotificationDaemon::GetCapabilities()
{
    return QStringList{"actions",
            "body",
            "body-hyperlinks",
            "body-images",
            "body-markup",
            "icon-static",
            "persistence",
            "sound"};
}

QString NotificationDaemon::GetServerInformation(QString &vendor, QString &version, QString &spec_version)
{
    spec_version = "1.2";
    vendor = "Originull";
    version = "1.0";
    return "Hollywood";
}

uint NotificationDaemon::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout, bool noSave)
{
    m_id++;
    uint id = m_id;
    if(replaces_id != 0)
        id = replaces_id;

    auto n = new Notification(id, app_name, app_icon, summary, body,
                              actions, hints, expire_timeout, noSave);
    m_notifications.insert(id, n);
    connect(n, &Notification::closeRequested, this, &NotificationDaemon::closeRequested);
    connect(n, &Notification::dismiss, this, &NotificationDaemon::dismiss);
    connect(n, &Notification::expired, this, &NotificationDaemon::expired);
    m_currentShown++;
    n->setCurrentPosition(m_currentShown);
    n->show();
    return id;
}

void NotificationDaemon::loadSettings()
{
    bool old_southern = m_southern;
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup("Bell");
    auto bell = settings.value("AudioFile", "/usr/share/sounds/Hollywood/Bell.wav").toString();
    m_bell.setSource(QUrl::fromLocalFile(bell));
    settings.endGroup();
    settings.beginGroup("Stage");
    m_southern = settings.value("UseSouthernMode", false).toBool();

    if(m_southern != old_southern && !m_configfile.isEmpty())
        changeSouthernMode();

    m_configfile = settings.fileName();
}

void NotificationDaemon::dismiss()
{
    auto notification = qobject_cast<Notification*>(sender());
    Q_ASSERT(notification);
    emit NotificationClosed(notification->id(), 2);
    animateCloseNotification(notification);
}

void NotificationDaemon::closeRequested()
{
    auto notification = qobject_cast<Notification*>(sender());
    Q_ASSERT(notification);
    emit NotificationClosed(notification->id(), 3);
    animateCloseNotification(notification);
}

void NotificationDaemon::expired()
{
    auto notification = qobject_cast<Notification*>(sender());
    Q_ASSERT(notification);
    emit NotificationClosed(notification->id(), 1);
    animateCloseNotification(notification);
}

void NotificationDaemon::changeSouthernMode()
{
    // TODO: tacky, but lets just hide all notifications
    // on our layout switch.  the user can revisit them
    // as they wish
}

void NotificationDaemon::animateCloseNotification(Notification *n)
{
    if(m_southern)
    {
        // we go down the position numbers and move all up
        int moveSize = n->window()->size().height();
        auto group = new QParallelAnimationGroup;
        auto myself = new QPropertyAnimation(n->window(), "size");
        myself->setStartValue(n->window()->size());
        auto endval = n->window()->size();
        endval.setHeight(0);
        myself->setEndValue(endval);
        myself->setDuration(190);
        myself->setEasingCurve(QEasingCurve::InOutQuad);
        n->window()->setMinimumHeight(0);
        group->addAnimation(myself);
        for(auto notification : m_notifications.values())
        {
            if(notification->currentPosition() > n->currentPosition())
            {
                auto ani = new QPropertyAnimation(notification->window(), "pos");
                ani->setStartValue(notification->window()->pos());
                auto end = notification->window()->pos();
                end.setY(end.y() - moveSize - 10);
                ani->setEndValue(end);
                ani->setDuration(190);
                ani->setEasingCurve(QEasingCurve::InOutQuad);
                group->addAnimation(ani);
            }
            notification->setCurrentPosition(notification->currentPosition()-1);
        }
        connect(group, &QParallelAnimationGroup::finished,[this, group, n](){
            m_notifications.remove(n->id());
            n->window()->close();
            n->deleteLater();
            group->deleteLater();
        });
        group->start(QAnimationGroup::DeleteWhenStopped);
    }
    else
    {
        int moveSize = n->window()->size().height();
        auto group = new QParallelAnimationGroup;
        auto myself = new QPropertyAnimation(n->window(), "size");
        myself->setStartValue(n->window()->size());
        auto endval = n->window()->size();
        endval.setHeight(0);
        myself->setEndValue(endval);
        myself->setDuration(190);
        myself->setEasingCurve(QEasingCurve::InOutQuad);
        auto myselfPos = new QPropertyAnimation(n->window(), "pos");
        myselfPos->setStartValue(n->window()->pos());
        auto endpos = n->window()->pos();
        endpos.setY(endpos.y()+n->window()->size().height());
        myselfPos->setEndValue(endpos);
        myselfPos->setDuration(190);
        myselfPos->setEasingCurve(QEasingCurve::InOutQuad);
        n->window()->setMinimumHeight(0);
        group->addAnimation(myself);
        group->addAnimation(myselfPos);
        for(auto notification : m_notifications.values())
        {
            if(notification->currentPosition() > n->currentPosition())
            {
                auto ani = new QPropertyAnimation(notification->window(), "pos");
                ani->setStartValue(notification->window()->pos());
                auto end = notification->window()->pos();
                end.setY(end.y() + moveSize + 10);
                ani->setEndValue(end);
                ani->setDuration(190);
                ani->setEasingCurve(QEasingCurve::InOutQuad);
                group->addAnimation(ani);
            }
            notification->setCurrentPosition(notification->currentPosition()-1);
        }
        connect(group, &QParallelAnimationGroup::finished,[this, group, n](){
            m_notifications.remove(n->id());
            n->window()->close();
            n->deleteLater();
            group->deleteLater();
        });
        group->start(QAnimationGroup::DeleteWhenStopped);
    }
}


int main(int argc, char *argv[])
{
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
    NotificationDaemon a(argc, argv);
    return a.exec();
}
