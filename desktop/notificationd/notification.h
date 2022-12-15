// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QVariant>
#include <QSize>
#include <QPoint>

class NotificationWindow;
class Notification : public QObject
{
    Q_OBJECT
public:
    enum Urgency {
        Low,
        Normal,
        Critical
    };
    explicit Notification(uint id,
                          const QString& app_name,
                          const QString& app_icon,
                          const QString& summary,
                          const QString& body,
                          const QStringList& actions,
                          const QVariantMap& hints,
                          int expire_timeout,
                          bool noSave = false,
            QObject *parent = nullptr);
    void show();
    uint notificationHeight() const;
    void setCurrentPosition(uint pos) { m_currentPos = pos; }
    NotificationWindow *window() { return m_wnd; }
    uint id() { return m_id; }
    uint currentPosition() const { return m_currentPos; }
signals:
    void dismiss();
    void closeRequested();
    void expired();
private slots:
    void actionSelected(QVariant action);
private:
    void animateOpen();
    void serialize();
    void remove();
private:
    friend class NotificationWindow;
    Urgency m_urgency = Normal;
    uint m_id;
    uint m_currentPos;
    QSize m_size;
    QPoint m_position;
    QString m_app_name;
    QString m_desktop_file;
    bool m_nosound = false;
    QString m_app_icon;
    QString m_summary;
    QString m_body;
    QStringList m_actions;
    QVariantMap m_hints;
    int m_timeout;
    bool m_nosave;
    NotificationWindow *m_wnd = nullptr;
};

#endif // NOTIFICATION_H
