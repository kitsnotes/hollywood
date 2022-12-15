// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef NOTIFICATIONDAEMON_H
#define NOTIFICATIONDAEMON_H

#include <QApplication>
#include <QObject>
#include <QFileSystemWatcher>
#include <QSoundEffect>
#include "notification.h"

class NotificationDaemon : public QApplication
{
    Q_OBJECT
public:
    NotificationDaemon(int &argc, char **argv);
    static NotificationDaemon* instance() { return static_cast<NotificationDaemon*>(QApplication::instance()); }
    bool southernMode() const { return m_southern; }
    QList<Notification*> openNotifications();
public slots:
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString& vendor, QString& version, QString& spec_version);
    uint Notify(const QString& app_name,
                uint replaces_id,
                const QString& app_icon,
                const QString& summary,
                const QString& body,
                const QStringList& actions,
                const QVariantMap& hints,
                int expire_timeout,
                bool noSave = false);
private slots:
    void loadSettings();
    void dismiss();
    void closeRequested();
    void expired();
signals:
    void ActionInvoked(uint in0, const QString& in1);
    void NotificationClosed(uint in0, uint in1);
private:
    void changeSouthernMode();
    void animateCloseNotification(Notification *n);
private:
    uint m_id = 0;
    QString m_configfile;
    QSoundEffect m_bell;
    bool m_southern = false;
    QHash<uint, Notification*> m_notifications;
    QFileSystemWatcher *m_configwatcher;
    uint m_currentShown = 0;
};

#endif // NOTIFICATIONDAEMON_H
