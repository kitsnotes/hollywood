#ifndef MACTOOLTITLEBAR_H
#define MACTOOLTITLEBAR_H

#include "luvedit.h"

// this exists entirely because the QMacToolBar in Qt MacExtras
// is a heaping pile of uncustomizable poop.
// If the QtMacExtras implementation ever improves, perhaps we
// can move back to it.

Q_FORWARD_DECLARE_OBJC_CLASS(NSView);
Q_FORWARD_DECLARE_OBJC_CLASS(NSMutableArray);
Q_FORWARD_DECLARE_OBJC_CLASS(NSToolbar);
Q_FORWARD_DECLARE_OBJC_CLASS(NSToolbarItem);

class EditorWindow;
class SearchWidget;
class QMacToolbarPrivate;

class MacToolBarItem : public QObject
{
    Q_OBJECT
public:
    enum StandardItem
    {
        NoStandardItem,
        Space,
        FlexibleSpace
    };
public:
    MacToolBarItem(const QString &id, QObject *parent = Q_NULLPTR);
    static MacToolBarItem* createFromStandardItem(MacToolBarItem::StandardItem standardItem, QObject *parent = Q_NULLPTR);
    NSToolbarItem* nativeToolBarItem() const;
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    MacToolBarItem::StandardItem standardItem() const;

    QString identifier() const;
    QIcon icon() const;
    QString text() const;

    bool selectable();
    void setSelectable(bool selectable);

signals:
    void activated();
private:
    QString m_text;
    QIcon m_icon;
    QString m_identifier;
    bool m_selectable;
    StandardItem m_stdItem;
    NSToolbarItem *m_item;
};

class MacToolBar : public QObject
{
    Q_OBJECT
public:
    MacToolBar(const QString &identifier, QObject *parent = Q_NULLPTR);
    ~MacToolBar();
    void loadFromPlist();
    void setAutosaveConfiguration(bool autosave = false);
    void insertSearchWidget(SearchWidget *w);
    MacToolBarItem* addItemByAction(QAction *action);
    MacToolBarItem* addAllowedItemByAction(QAction *action);
    MacToolBarItem* addItem(const QString &id, const QIcon &icon, const QString &text);
    MacToolBarItem* addAllowedItem(const QString &id, const QIcon &icon, const QString &text);
    static NSString *getItemIdentifier(const MacToolBarItem *item);
    static NSMutableArray *getItemIdentifiers(const QList<MacToolBarItem *> &items, bool omitSelectable = false);
    void itemClicked(NSToolbarItem *itemClicked);
    MacToolBarItem *addStandardItem(MacToolBarItem::StandardItem standardItem);
    MacToolBarItem *addAllowedStandardItem(MacToolBarItem::StandardItem standardItem);
    void attachToWindow(QWindow *window);
    void detachFromWindow();
    MacToolBarItem* findItemByIdentifier(const QString& identifier);


    bool isVisible();
    void setVisible(bool visible);
private slots:
    void showWindowTimerSlot();
private:
    NSToolbar *m_toolbar;
    QWindow *m_targetWindow;
public:
    QList<MacToolBarItem *> m_items;
    QList<MacToolBarItem *> m_allowedItems;
};

#endif // MACTOOLTITLEBAR_H
