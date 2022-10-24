#include "qdbusmenuadaptor_p.h"
#include "qdbusplatformmenu_p.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

DBusMenuAdaptor::DBusMenuAdaptor(DBusPlatformMenu *topLevelMenu)
    : QDBusAbstractAdaptor(topLevelMenu)
    , m_topLevelMenu(topLevelMenu)
{
    setAutoRelaySignals(true);
}

DBusMenuAdaptor::~DBusMenuAdaptor()
{
}

QString DBusMenuAdaptor::status() const
{
    qCDebug(qLcMenu);
    return QLatin1String("normal");
}

QString DBusMenuAdaptor::textDirection() const
{
    return QLocale().textDirection() == Qt::RightToLeft ? QLatin1String("rtl") : QLatin1String("ltr");
}

uint DBusMenuAdaptor::version() const
{
    return 4;
}

bool DBusMenuAdaptor::AboutToShow(int id)
{
    qCDebug(qLcMenu) << id;
    if (id == 0) {
        emit m_topLevelMenu->aboutToShow();
    } else {
        DBusPlatformMenuItem *item = DBusPlatformMenuItem::byId(id);
        if (item) {
            const DBusPlatformMenu *menu = static_cast<const DBusPlatformMenu *>(item->menu());
            if (menu)
                emit const_cast<DBusPlatformMenu *>(menu)->aboutToShow();
        }
    }
    return false;  // updateNeeded (we don't know that, so false)
}

QList<int> DBusMenuAdaptor::AboutToShowGroup(const QList<int> &ids, QList<int> &idErrors)
{
    qCDebug(qLcMenu) << ids;
    Q_UNUSED(idErrors)
    idErrors.clear();
    for (int id : ids)
        AboutToShow(id);
    return QList<int>(); // updatesNeeded
}

void DBusMenuAdaptor::Event(int id, const QString &eventId, const QDBusVariant &data, uint timestamp)
{
    Q_UNUSED(data)
    Q_UNUSED(timestamp)
    DBusPlatformMenuItem *item = DBusPlatformMenuItem::byId(id);
    qCDebug(qLcMenu) << id << (item ? item->text() : QLatin1String("")) << eventId;
    if (item && eventId == QLatin1String("clicked"))
        item->trigger();
    if (item && eventId == QLatin1String("hovered"))
        emit item->hovered();
    if (eventId == QLatin1String("closed")) {
        // There is no explicit AboutToHide method, so map closed event to aboutToHide method
        const DBusPlatformMenu *menu = nullptr;
        if (item)
            menu = static_cast<const DBusPlatformMenu *>(item->menu());
        else if (id == 0)
            menu = m_topLevelMenu;
        if (menu)
            emit const_cast<DBusPlatformMenu *>(menu)->aboutToHide();
    }
}

QList<int> DBusMenuAdaptor::EventGroup(const DBusMenuEventList &events)
{
    for (const DBusMenuEvent &ev : events)
        Event(ev.m_id, ev.m_eventId, ev.m_data, ev.m_timestamp);
    return QList<int>(); // idErrors
}

DBusMenuItemList DBusMenuAdaptor::GetGroupProperties(const QList<int> &ids, const QStringList &propertyNames)
{
    qCDebug(qLcMenu) << ids << propertyNames << "=>" << DBusMenuItem::items(ids, propertyNames);
    return DBusMenuItem::items(ids, propertyNames);
}

uint DBusMenuAdaptor::GetLayout(int parentId, int recursionDepth, const QStringList &propertyNames, DBusMenuLayoutItem &layout)
{
    uint ret = layout.populate(parentId, recursionDepth, propertyNames, m_topLevelMenu);
    qCDebug(qLcMenu) << parentId << "depth" << recursionDepth << propertyNames << layout.m_id << layout.m_properties << "revision" << ret << layout;
    return ret;
}

QDBusVariant DBusMenuAdaptor::GetProperty(int id, const QString &name)
{
    qCDebug(qLcMenu) << id << name;
    // handle method call com.canonical.dbusmenu.GetProperty
    QDBusVariant value;
    return value;
}

QT_END_NAMESPACE
