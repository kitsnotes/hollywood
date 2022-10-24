#ifndef QDBUSMENUTYPES_P_H
#define QDBUSMENUTYPES_P_H
#include <QObject>
#include <QString>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QPixmap>

QT_BEGIN_NAMESPACE

class DBusPlatformMenu;
class DBusPlatformMenuItem;
class DBusMenuItem;
typedef QVector<DBusMenuItem> DBusMenuItemList;
typedef QVector<QStringList> DBusMenuShortcut;

class DBusMenuItem
{
public:
    DBusMenuItem() { }
    DBusMenuItem(const DBusPlatformMenuItem *item);

    static DBusMenuItemList items(const QList<int> &ids, const QStringList &propertyNames);
    static QString convertMnemonic(const QString &label);
#ifndef QT_NO_SHORTCUT
    static DBusMenuShortcut convertKeySequence(const QKeySequence &sequence);
#endif
    static void registerDBusTypes();

    int m_id;
    QVariantMap m_properties;
};
Q_DECLARE_TYPEINFO(DBusMenuItem, Q_MOVABLE_TYPE);

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItem &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItem &item);

class DBusMenuItemKeys
{
public:

    int id;
    QStringList properties;
};
Q_DECLARE_TYPEINFO(DBusMenuItemKeys, Q_MOVABLE_TYPE);

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemKeys &keys);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemKeys &keys);

typedef QVector<DBusMenuItemKeys> DBusMenuItemKeysList;

class DBusMenuLayoutItem
{
public:
    uint populate(int id, int depth, const QStringList &propertyNames, const DBusPlatformMenu *topLevelMenu);
    void populate(const DBusPlatformMenu *menu, int depth, const QStringList &propertyNames);
    void populate(const DBusPlatformMenuItem *item, int depth, const QStringList &propertyNames);

    int m_id;
    QVariantMap m_properties;
    QVector<DBusMenuLayoutItem> m_children;
};
Q_DECLARE_TYPEINFO(DBusMenuLayoutItem, Q_MOVABLE_TYPE);

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuLayoutItem &);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuLayoutItem &item);

typedef QVector<DBusMenuLayoutItem> DBusMenuLayoutItemList;

class DBusMenuEvent
{
public:
    int m_id;
    QString m_eventId;
    QDBusVariant m_data;
    uint m_timestamp;
};
Q_DECLARE_TYPEINFO(DBusMenuEvent, Q_MOVABLE_TYPE); // QDBusVariant is movable, even though it cannot
                                                    // be marked as such until Qt 6.

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuEvent &ev);
const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuEvent &ev);

typedef QVector<DBusMenuEvent> DBusMenuEventList;

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const DBusMenuItem &item);
QDebug operator<<(QDebug d, const DBusMenuLayoutItem &item);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(DBusMenuItem)
Q_DECLARE_METATYPE(DBusMenuItemList)
Q_DECLARE_METATYPE(DBusMenuItemKeys)
Q_DECLARE_METATYPE(DBusMenuItemKeysList)
Q_DECLARE_METATYPE(DBusMenuLayoutItem)
Q_DECLARE_METATYPE(DBusMenuLayoutItemList)
Q_DECLARE_METATYPE(DBusMenuEvent)
Q_DECLARE_METATYPE(DBusMenuEventList)
Q_DECLARE_METATYPE(DBusMenuShortcut)


#endif // QDBUSMENUTYPES_P_H
