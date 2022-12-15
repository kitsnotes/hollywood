#ifndef NOTIFICATIONWINDOW_H
#define NOTIFICATIONWINDOW_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>
class Notification;
class NotificationWindow : public QWidget
{
    Q_OBJECT

public:
    NotificationWindow(Notification *parent);
    ~NotificationWindow();
signals:
    void acknowledged();
    void expired();
private:
    Notification *m_parent;
    QHBoxLayout *m_layout;
    QLabel *m_icon;
    QLabel *m_title;
    QLabel *m_details;
    QToolButton *m_close;
};
#endif // NOTIFICATIONWINDOW_H
