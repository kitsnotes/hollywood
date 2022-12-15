#include "notification.h"
#include "notifywindow.h"

#include <QApplication>
#include <QScreen>
#include <QPainterPath>

Notification::Notification(const uint id, const QString &app_name, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int expire_timeout, bool noSave, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_app_name(app_name)
    , m_app_icon(app_icon)
    , m_summary(summary)
    , m_body(body)
    , m_actions(actions)
    , m_timeout(expire_timeout)
    , m_nosave(noSave)
    , m_wnd(new NotificationWindow(this))
{

}

void Notification::show()
{
    auto screen = QApplication::primaryScreen();
    m_wnd->setScreen(screen);
    m_wnd->show();
    int margin = 10;
    int x = screen->size().width() - m_wnd->width() - margin;
    int y = 40;
    m_wnd->move(x,y);
    QPainterPath path;
    path.addRoundedRect(QRectF(m_wnd->rect()), 40.0, 40.0);
    QRegion mask(path.toFillPolygon().toPolygon());
    m_wnd->setMask(mask);
    m_wnd->setWindowOpacity(0.8);

}
