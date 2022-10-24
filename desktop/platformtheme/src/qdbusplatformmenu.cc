#include "qdbusplatformmenu_p.h"

#include <QDateTime>
#include <QDebug>
#include <QWindow>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcMenu, "qt.qpa.menu")

static int nextDBusID = 1;
QHash<int, DBusPlatformMenuItem *> menuItemsByID;

DBusPlatformMenuItem::DBusPlatformMenuItem()
    : m_subMenu(nullptr)
    , m_role(NoRole)
    , m_isEnabled(true)
    , m_isVisible(true)
    , m_isSeparator(false)
    , m_isCheckable(false)
    , m_isChecked(false)
    , m_hasExclusiveGroup(false)
    , m_dbusID(nextDBusID++)
{
    menuItemsByID.insert(m_dbusID, this);
}

DBusPlatformMenuItem::~DBusPlatformMenuItem()
{
    menuItemsByID.remove(m_dbusID);
    if (m_subMenu)
        static_cast<DBusPlatformMenu *>(m_subMenu)->setContainingMenuItem(nullptr);
}

void DBusPlatformMenuItem::setText(const QString &text)
{
    qCDebug(qLcMenu) << m_dbusID << text;
    m_text = text;
}

void DBusPlatformMenuItem::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

/*!
    Set a submenu under this menu item.
*/
void DBusPlatformMenuItem::setMenu(QPlatformMenu *menu)
{
    if (m_subMenu)
        static_cast<DBusPlatformMenu *>(m_subMenu)->setContainingMenuItem(nullptr);
    m_subMenu = menu;
    if (menu)
        static_cast<DBusPlatformMenu *>(menu)->setContainingMenuItem(this);
}

void DBusPlatformMenuItem::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

void DBusPlatformMenuItem::setVisible(bool isVisible)
{
    m_isVisible = isVisible;
}

void DBusPlatformMenuItem::setIsSeparator(bool isSeparator)
{
    m_isSeparator = isSeparator;
}

void DBusPlatformMenuItem::setRole(QPlatformMenuItem::MenuRole role)
{
    m_role = role;
}

void DBusPlatformMenuItem::setCheckable(bool checkable)
{
    m_isCheckable = checkable;
}

void DBusPlatformMenuItem::setChecked(bool isChecked)
{
    m_isChecked = isChecked;
}

void DBusPlatformMenuItem::setHasExclusiveGroup(bool hasExclusiveGroup)
{
    m_hasExclusiveGroup = hasExclusiveGroup;
}

#ifndef QT_NO_SHORTCUT
void DBusPlatformMenuItem::setShortcut(const QKeySequence &shortcut)
{
    m_shortcut = shortcut;
}
#endif

void DBusPlatformMenuItem::trigger()
{
    emit activated();
}

DBusPlatformMenuItem *DBusPlatformMenuItem::byId(int id)
{
    // We need to check contains because otherwise QHash would insert
    // a default-constructed nullptr value into menuItemsByID
    if (menuItemsByID.contains(id))
        return menuItemsByID[id];
    return nullptr;
}

QList<const DBusPlatformMenuItem *> DBusPlatformMenuItem::byIds(const QList<int> &ids)
{
    QList<const DBusPlatformMenuItem *> ret;
    for (int id : ids) {
        if (menuItemsByID.contains(id))
            ret << menuItemsByID[id];
    }
    return ret;
}


DBusPlatformMenu::DBusPlatformMenu()
    : m_isEnabled(true)
    , m_isVisible(true)
    , m_revision(1)
    , m_containingMenuItem(nullptr)
{
}

DBusPlatformMenu::~DBusPlatformMenu()
{
    if (m_containingMenuItem)
        m_containingMenuItem->setMenu(nullptr);
}

void DBusPlatformMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    DBusPlatformMenuItem *item = static_cast<DBusPlatformMenuItem *>(menuItem);
    DBusPlatformMenuItem *beforeItem = static_cast<DBusPlatformMenuItem *>(before);
    int idx = m_items.indexOf(beforeItem);
    qCDebug(qLcMenu) << item->dbusID() << item->text();
    if (idx < 0)
        m_items.append(item);
    else
        m_items.insert(idx, item);
    m_itemsByTag.insert(item->tag(), item);
    if (item->menu())
        syncSubMenu(static_cast<const DBusPlatformMenu *>(item->menu()));
    emitUpdated();
}

void DBusPlatformMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    DBusPlatformMenuItem *item = static_cast<DBusPlatformMenuItem *>(menuItem);
    m_items.removeAll(item);
    m_itemsByTag.remove(menuItem->tag());
    if (item->menu()) {
        // disconnect from the signals we connected to in syncSubMenu()
        const DBusPlatformMenu *menu = static_cast<const DBusPlatformMenu *>(item->menu());
        disconnect(menu, &DBusPlatformMenu::propertiesUpdated,
                   this, &DBusPlatformMenu::propertiesUpdated);
        disconnect(menu, &DBusPlatformMenu::updated,
                   this, &DBusPlatformMenu::updated);
        disconnect(menu, &DBusPlatformMenu::popupRequested,
                   this, &DBusPlatformMenu::popupRequested);
    }
    emitUpdated();
}

void DBusPlatformMenu::syncSubMenu(const DBusPlatformMenu *menu)
{
    // The adaptor is only connected to the propertiesUpdated signal of the top-level
    // menu, so the submenus should transfer their signals to their parents.
    connect(menu, &DBusPlatformMenu::propertiesUpdated,
            this, &DBusPlatformMenu::propertiesUpdated, Qt::UniqueConnection);
    connect(menu, &DBusPlatformMenu::updated,
            this, &DBusPlatformMenu::updated, Qt::UniqueConnection);
    connect(menu, &DBusPlatformMenu::popupRequested,
            this, &DBusPlatformMenu::popupRequested, Qt::UniqueConnection);
}

void DBusPlatformMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    DBusPlatformMenuItem *item = static_cast<DBusPlatformMenuItem *>(menuItem);
    // if a submenu was added to this item, we need to connect to its signals
    if (item->menu())
        syncSubMenu(static_cast<const DBusPlatformMenu *>(item->menu()));
    // TODO keep around copies of the QDBusMenuLayoutItems so they can be updated?
    // or eliminate them by putting dbus streaming operators in this class instead?
    // or somehow tell the dbusmenu client that something has changed, so it will ask for properties again
    DBusMenuItemList updated;
    DBusMenuItemKeysList removed;
    updated << DBusMenuItem(item);
    qCDebug(qLcMenu) << updated;
    emit propertiesUpdated(updated, removed);
}

void DBusPlatformMenu::emitUpdated()
{
    if (m_containingMenuItem)
        emit updated(++m_revision, m_containingMenuItem->dbusID());
    else
        emit updated(++m_revision, 0);
}

void DBusPlatformMenu::setText(const QString &text)
{
    m_text = text;
}

void DBusPlatformMenu::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

void DBusPlatformMenu::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

void DBusPlatformMenu::setVisible(bool isVisible)
{
    m_isVisible = isVisible;
}

void DBusPlatformMenu::setContainingMenuItem(DBusPlatformMenuItem *item)
{
    m_containingMenuItem = item;
}

void DBusPlatformMenu::showPopup(const QWindow *parentWindow, const QRect &targetRect, const QPlatformMenuItem *item)
{
    Q_UNUSED(parentWindow);
    Q_UNUSED(targetRect);
    Q_UNUSED(item);
    setVisible(true);
    emit popupRequested(m_containingMenuItem->dbusID(), QDateTime::currentMSecsSinceEpoch());
}

QPlatformMenuItem *DBusPlatformMenu::menuItemAt(int position) const
{
    return m_items.value(position);
}

QPlatformMenuItem *DBusPlatformMenu::menuItemForTag(quintptr tag) const
{
    return m_itemsByTag[tag];
}

const QList<DBusPlatformMenuItem *> DBusPlatformMenu::items() const
{
    return m_items;
}

QPlatformMenuItem *DBusPlatformMenu::createMenuItem() const
{
    DBusPlatformMenuItem *ret = new DBusPlatformMenuItem();
    return ret;
}

QPlatformMenu *DBusPlatformMenu::createSubMenu() const
{
    return new DBusPlatformMenu;
}

QT_END_NAMESPACE
