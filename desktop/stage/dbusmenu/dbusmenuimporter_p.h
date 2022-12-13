#ifndef DBUSMENUIMPORTER_P_H
#define DBUSMENUIMPORTER_P_H

#include <QWidget>
#include <QDBusPendingCallWatcher>
#include <QMenu>
#include "dbusmenutypes_p.h"
#include "dbusmenu_interface.h"

class DBusMenuImporter;
class DBusMenuImporterPrivate
{
public:
    DBusMenuImporter *q;

    DBusMenuInterface *m_interface;
    QMenu *m_menu;
    using ActionForId = QMap<int, QAction *>;
    ActionForId m_actionForId;
    QTimer *m_pendingLayoutUpdateTimer;

    QSet<int> m_idsRefreshedByAboutToShow;
    QSet<int> m_pendingLayoutUpdates;

    QDBusPendingCallWatcher *refresh(int id);
    QMenu *createMenu(QWidget *parent);
    QAction *createAction(int id, const QVariantMap &_map, QWidget *parent);
    void updateAction(QAction *action, const QVariantMap &map, const QStringList &requestedProperties);
    void updateActionProperty(QAction *action, const QString &key, const QVariant &value);
    void updateActionLabel(QAction *action, const QVariant &value);

    void updateActionEnabled(QAction *action, const QVariant &value);
    void updateActionChecked(QAction *action, const QVariant &value);
    void updateActionIconByName(QAction *action, const QVariant &value);
    void updateActionIconByData(QAction *action, const QVariant &value);
    void updateActionVisible(QAction *action, const QVariant &value);
    void updateActionShortcut(QAction *action, const QVariant &value);
    QMenu *menuForId(int id) const;
    void slotItemsPropertiesUpdated(const DBusMenuItemList &updatedList, const DBusMenuItemKeysList &removedList);
    void sendEvent(int id, const QString &eventId);
};

#endif // DBUSMENUIMPORTER_P_H
