#include "notifierhost.h"
#include "statusnotifier/statusnotifierproxy.h"
#include "statusnotifier/statusnotifierbutton.h"

NotifierHost::NotifierHost(QWidget *parent)
    : QObject(0)
    , m_parent(parent)
{
    StatusNotifierProxy & proxy = StatusNotifierProxy::registerLifetimeUsage(this);
    connect(&proxy, &StatusNotifierProxy::StatusNotifierItemRegistered,
                    this, &NotifierHost::itemAdded);
    connect(&proxy, &StatusNotifierProxy::StatusNotifierItemUnregistered,
                    this, &NotifierHost::itemRemoved);
    for (const auto & service: proxy.RegisteredStatusNotifierItems())
        itemAdded(service);
}

void NotifierHost::itemAdded(QString serviceAndPath)
{
    int slash = serviceAndPath.indexOf(QLatin1Char('/'));
    QString serv = serviceAndPath.left(slash);
    QString path = serviceAndPath.mid(slash);
    StatusNotifierButton *button = new StatusNotifierButton(serv, path, m_parent);

    m_services.insert(serviceAndPath, button);
    emit buttonAdded(button);
    button->show();
}

void NotifierHost::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = m_services.value(serviceAndPath, nullptr);
    if (button)
    {
        emit buttonRemoved(button);
        button->deleteLater();
        m_services.remove(serviceAndPath);
    }
}
