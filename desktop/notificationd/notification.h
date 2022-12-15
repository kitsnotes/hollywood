#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QVariant>

class NotificationWindow;
class Notification : public QObject
{
    Q_OBJECT
public:
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
private:
    void serialize();
    void remove();
private:
    friend class NotificationWindow;
    uint m_id;
    QString m_app_name;
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
