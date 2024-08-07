#include "qdbusmenutypes_p.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QImage>
#include <QIcon>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QtEndian>
#include <QBuffer>
#include <QKeySequence>
#include <private/qkeysequence_p.h>
#include <qpa/qplatformmenu.h>
#include "qdbusplatformmenu_p.h"

QT_BEGIN_NAMESPACE

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItem &item)
{
    arg.beginStructure();
    arg << item.m_id << item.m_properties;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItem &item)
{
    arg.beginStructure();
    arg >> item.m_id >> item.m_properties;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuItemKeys &keys)
{
    arg.beginStructure();
    arg << keys.id << keys.properties;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuItemKeys &keys)
{
    arg.beginStructure();
    arg >> keys.id >> keys.properties;
    arg.endStructure();
    return arg;
}

uint DBusMenuLayoutItem::populate(int id, int depth, const QStringList &propertyNames, const DBusPlatformMenu *topLevelMenu)
{
    qCDebug(qLcMenu) << id << "depth" << depth << propertyNames;
    m_id = id;
    if (id == 0) {
        m_properties.insert(QLatin1String("children-display"), QLatin1String("submenu"));
        if (topLevelMenu)
            populate(topLevelMenu, depth, propertyNames);
        return 1; // revision
    }

    DBusPlatformMenuItem *item = DBusPlatformMenuItem::byId(id);
    if (item) {
        const DBusPlatformMenu *menu = static_cast<const DBusPlatformMenu *>(item->menu());

        if (menu) {
            if (depth != 0)
                populate(menu, depth, propertyNames);
            return menu->revision();
        }
    }

    return 1; // revision
}

void DBusMenuLayoutItem::populate(const DBusPlatformMenu *menu, int depth, const QStringList &propertyNames)
{
    const auto items = menu->items();
    for (DBusPlatformMenuItem *item : items) {
        DBusMenuLayoutItem child;
        child.populate(item, depth - 1, propertyNames);
        m_children << child;
    }
}

void DBusMenuLayoutItem::populate(const DBusPlatformMenuItem *item, int depth, const QStringList &propertyNames)
{
    m_id = item->dbusID();
    DBusMenuItem proxy(item);
    m_properties = proxy.m_properties;

    const DBusPlatformMenu *menu = static_cast<const DBusPlatformMenu *>(item->menu());
    if (depth != 0 && menu)
        populate(menu, depth, propertyNames);
}

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuLayoutItem &item)
{
    arg.beginStructure();
    arg << item.m_id << item.m_properties;
    arg.beginArray(qMetaTypeId<QDBusVariant>());
    for (const DBusMenuLayoutItem &child : item.m_children)
        arg << QDBusVariant(QVariant::fromValue<DBusMenuLayoutItem>(child));
    arg.endArray();
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuLayoutItem &item)
{
    arg.beginStructure();
    arg >> item.m_id >> item.m_properties;
    arg.beginArray();
    while (!arg.atEnd()) {
        QDBusVariant dbusVariant;
        arg >> dbusVariant;
        QDBusArgument childArgument = dbusVariant.variant().value<QDBusArgument>();

        DBusMenuLayoutItem child;
        childArgument >> child;
        item.m_children.append(child);
    }
    arg.endArray();
    arg.endStructure();
    return arg;
}

void DBusMenuItem::registerDBusTypes()
{
    qDBusRegisterMetaType<DBusMenuItem>();
    qDBusRegisterMetaType<DBusMenuItemList>();
    qDBusRegisterMetaType<DBusMenuItemKeys>();
    qDBusRegisterMetaType<DBusMenuItemKeysList>();
    qDBusRegisterMetaType<DBusMenuLayoutItem>();
    qDBusRegisterMetaType<DBusMenuLayoutItemList>();
    qDBusRegisterMetaType<DBusMenuEvent>();
    qDBusRegisterMetaType<DBusMenuEventList>();
    qDBusRegisterMetaType<DBusMenuShortcut>();
}

DBusMenuItem::DBusMenuItem(const DBusPlatformMenuItem *item)
    : m_id(item->dbusID())
{
    if (item->isSeparator()) {
        m_properties.insert(QLatin1String("type"), QLatin1String("separator"));
    } else {
        m_properties.insert(QLatin1String("label"), convertMnemonic(item->text()));
        if (item->menu())
            m_properties.insert(QLatin1String("children-display"), QLatin1String("submenu"));
        m_properties.insert(QLatin1String("enabled"), item->isEnabled());
        if (item->isCheckable()) {
            QString toggleType = item->hasExclusiveGroup() ? QLatin1String("radio") : QLatin1String("checkmark");
            m_properties.insert(QLatin1String("toggle-type"), toggleType);
            m_properties.insert(QLatin1String("toggle-state"), item->isChecked() ? 1 : 0);
        }
#ifndef QT_NO_SHORTCUT
        const QKeySequence &scut = item->shortcut();
        if (!scut.isEmpty()) {
            DBusMenuShortcut shortcut = convertKeySequence(scut);
            m_properties.insert(QLatin1String("shortcut"), QVariant::fromValue(shortcut));
        }
#endif
        const QIcon &icon = item->icon();
        if (!icon.name().isEmpty()) {
            m_properties.insert(QLatin1String("icon-name"), icon.name());
        } else if (!icon.isNull()) {
            QBuffer buf;
            icon.pixmap(16).save(&buf, "PNG");
            m_properties.insert(QLatin1String("icon-data"), buf.data());
        }
    }
    m_properties.insert(QLatin1String("visible"), item->isVisible());
}

DBusMenuItemList DBusMenuItem::items(const QList<int> &ids, const QStringList &propertyNames)
{
    Q_UNUSED(propertyNames)
    DBusMenuItemList ret;
    const QList<const DBusPlatformMenuItem *> items = DBusPlatformMenuItem::byIds(ids);
    ret.reserve(items.size());
    for (const DBusPlatformMenuItem *item : items)
        ret << DBusMenuItem(item);
    return ret;
}

QString DBusMenuItem::convertMnemonic(const QString &label)
{
    // convert only the first occurrence of ampersand which is not at the end
    // dbusmenu uses underscore instead of ampersand
    int idx = label.indexOf(QLatin1Char('&'));
    if (idx < 0 || idx == label.length() - 1)
        return label;
    QString ret(label);
    ret[idx] = QLatin1Char('_');
    return ret;
}

DBusMenuShortcut DBusMenuItem::convertKeySequence(const QKeySequence &sequence)
{
    DBusMenuShortcut shortcut;
    for (int i = 0; i < sequence.count(); ++i) {
        QStringList tokens;
#if QT_VERSION >= 0x060000
        int key = sequence[i].toCombined();
#else
        int key = sequence[i];
#endif
        if (key & Qt::MetaModifier)
            tokens << QStringLiteral("Super");
        if (key & Qt::ControlModifier)
            tokens << QStringLiteral("Control");
        if (key & Qt::AltModifier)
            tokens << QStringLiteral("Alt");
        if (key & Qt::ShiftModifier)
            tokens << QStringLiteral("Shift");
        if (key & Qt::KeypadModifier)
            tokens << QStringLiteral("Num");

        QString keyName = QKeySequencePrivate::keyName((Qt::Key)key, QKeySequence::PortableText);
        if (keyName == QLatin1String("+"))
            tokens << QStringLiteral("plus");
        else if (keyName == QLatin1String("-"))
            tokens << QStringLiteral("minus");
        else
            tokens << keyName;
        shortcut << tokens;
    }
    return shortcut;
}

const QDBusArgument &operator<<(QDBusArgument &arg, const DBusMenuEvent &ev)
{
    arg.beginStructure();
    arg << ev.m_id << ev.m_eventId << ev.m_data << ev.m_timestamp;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DBusMenuEvent &ev)
{
    arg.beginStructure();
    arg >> ev.m_id >> ev.m_eventId >> ev.m_data >> ev.m_timestamp;
    arg.endStructure();
    return arg;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const DBusMenuItem &item)
{
    QDebugStateSaver saver(d);
    d.nospace();
    d << "QDBusMenuItem(id=" << item.m_id << ", properties=" << item.m_properties << ')';
    return d;
}

QDebug operator<<(QDebug d, const DBusMenuLayoutItem &item)
{
    QDebugStateSaver saver(d);
    d.nospace();
    d << "QDBusMenuLayoutItem(id=" << item.m_id << ", properties=" << item.m_properties << ", " << item.m_children.count() << " children)";
    return d;
}
#endif

QT_END_NAMESPACE
