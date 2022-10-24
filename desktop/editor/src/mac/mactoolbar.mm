#include "mac/mactoolbar.h"
#include "mac/toolbardelegate.h"

#include <QtCore/qglobal.h>

MacToolBar::MacToolBar(const QString &identifier, QObject *parent)
{
    Q_UNUSED(parent);
    m_toolbar = [[NSToolbar alloc] initWithIdentifier:identifier.toNSString()];
    [m_toolbar setAutosavesConfiguration:YES];
    [m_toolbar setAllowsUserCustomization:YES];

    MacToolbarDelegate *delegate = [[MacToolbarDelegate alloc] init];
    delegate->myToolbar = this;
    [m_toolbar setDelegate:delegate];

    m_targetWindow = 0;
}

MacToolBar::~MacToolBar()
{
    [[m_toolbar delegate]release];
    [m_toolbar release];
}

void MacToolBar::loadFromPlist()
{
    NSDictionary *tb = [[NSDictionary alloc]
    initWithDictionary:[m_toolbar configurationDictionary]];
    if(tb != nil)
        [m_toolbar setConfigurationFromDictionary:tb];
}

void MacToolBar::setAutosaveConfiguration(bool autosave)
{
    if(autosave)
        [m_toolbar setAutosavesConfiguration:YES];
    else
        [m_toolbar setAutosavesConfiguration:NO];
}

void MacToolBar::insertSearchWidget(SearchWidget *s)
{
    s->setParent(NULL);
    NSToolbarItem *ti = addItem(QString("Search"), QIcon(":/Logos/PNG"),
                                QString("Search Document"))->nativeToolBarItem();

    [ti setView:s->cocoaView()];
    NSSize size;
    size.height = 20;
    size.width = 150;

    [ti setPaletteLabel:@"Search Document"];
    [ti setMaxSize:size];
    [ti autorelease];
}

MacToolBarItem* MacToolBar::addItemByAction(QAction *action)
{
    MacToolBarItem* item = addItem(action->objectName(),
                                   action->icon(),
                    action->iconText());

    [item->nativeToolBarItem() setPaletteLabel:action->iconText().toNSString()];
    return item;
}

MacToolBarItem* MacToolBar::addAllowedItemByAction(QAction *action)
{
    MacToolBarItem* item = addAllowedItem(action->objectName(),
                                   action->icon(),
                    action->iconText());

    [item->nativeToolBarItem() setPaletteLabel:action->iconText().toNSString()];
    return item;
}

MacToolBarItem* MacToolBar::addItem(const QString &id, const QIcon &icon, const QString &text)
{
    MacToolBarItem *i = new MacToolBarItem(id, this);
    i->setText(text);
    i->setIcon(icon);

    m_items.append(i);
    m_allowedItems.append(i);
    return i;
}

MacToolBarItem* MacToolBar::addAllowedItem(const QString &id, const QIcon &icon, const QString &text)
{
    MacToolBarItem *i = new MacToolBarItem(id, this);
    i->setText(text);
    i->setIcon(icon);

    m_allowedItems.append(i);
    return i;
}

MacToolBarItem* MacToolBar::addStandardItem(MacToolBarItem::StandardItem item)
{
    MacToolBarItem *i = MacToolBarItem::createFromStandardItem(item);
    m_items.append(i);
    m_allowedItems.append(i);
    return i;
}

MacToolBarItem* MacToolBar::addAllowedStandardItem(MacToolBarItem::StandardItem item)
{
    MacToolBarItem *i = MacToolBarItem::createFromStandardItem(item);
    m_allowedItems.append(i);
    return i;
}

NSMutableArray *MacToolBar::getItemIdentifiers(const QList<MacToolBarItem *> &items, bool omitSelectable)
{
    NSMutableArray *array = [[NSMutableArray alloc] init];
    for(MacToolBarItem* item : items)
    {
        if(omitSelectable && !item->selectable())
            continue;

        [array addObject : item->identifier().toNSString()];
    }
    return array;
}

void MacToolBar::itemClicked(NSToolbarItem *item)
{
    QString id = QString::fromNSString([item itemIdentifier]);
    MacToolBarItem *tb = findItemByIdentifier(id);
    emit tb->activated();
}

void MacToolBar::attachToWindow(QWindow *window)
{
    if(!window)
        return detachFromWindow();;

    m_targetWindow = window;
    QTimer::singleShot(90, this, SLOT(showWindowTimerSlot()));

    return;
}

void MacToolBar::showWindowTimerSlot()
{
    if(!m_targetWindow)
    {
        QTimer::singleShot(90, this, SLOT(showWindowTimerSlot()));
        return;
    }

    NSWindow *wnd = [(NSView*)m_targetWindow->winId() window];

    if(!wnd)
    {
        QTimer::singleShot(90, this, SLOT(showWindowTimerSlot()));
        return;
    }

    [wnd setToolbar: m_toolbar];
    [wnd setShowsToolbarButton:YES];
}

void MacToolBar::detachFromWindow()
{
    if(!m_targetWindow)
        return;

    NSWindow *wnd = [(NSView*)m_targetWindow->winId() window];

    [wnd setToolbar:nil];
}

void MacToolBar::setVisible(bool visible)
{
    if(visible)
        m_toolbar.visible = YES;
    else
        m_toolbar.visible = NO;
}

bool MacToolBar::isVisible()
{
    if(m_toolbar.visible == YES)
        return true;
    else
        return false;
}

MacToolBarItem* MacToolBar::findItemByIdentifier(const QString &identifier)
{
    for(MacToolBarItem* item : m_allowedItems)
    {
        if(item->identifier() == identifier)
            return item;
    }

    return Q_NULLPTR;
}

MacToolBarItem::MacToolBarItem(const QString &id, QObject *parent)
{
    Q_UNUSED(parent);
    m_item = [[NSToolbarItem alloc] initWithItemIdentifier:id.toNSString()];

    m_selectable = false;
    m_identifier = id;
}

MacToolBarItem* MacToolBarItem::createFromStandardItem(StandardItem standardItem, QObject *parent)
{
    if(standardItem == Space)
        return new MacToolBarItem(QString::fromNSString(NSToolbarSpaceItemIdentifier), parent);

    if(standardItem == FlexibleSpace)
        return new MacToolBarItem(QString::fromNSString(NSToolbarFlexibleSpaceItemIdentifier), parent);

    return Q_NULLPTR;
}

NSToolbarItem* MacToolBarItem::nativeToolBarItem() const
{
    if(m_item != nil)
        return m_item;

    return NULL;
}

void MacToolBarItem::setIcon(const QIcon &icon)
{
    m_icon = icon;
    QPixmap pixmap = icon.pixmap(64, 64);
    NSSize mySize;
    mySize.height = 64;
    mySize.width = 64;
    if(pixmap.isNull() == false)
        [m_item setImage: [[NSImage alloc] initWithCGImage:pixmap.toImage().toCGImage()
                                            size:mySize]];
}

void MacToolBarItem::setText(const QString &text)
{
    m_text = text;
    [m_item setLabel:text.toNSString()];
}

QString MacToolBarItem::text() const
{
    const QString c(m_text);
    return c;
}

QIcon MacToolBarItem::icon() const
{
    const QIcon i(m_icon);
    return i;
}

QString MacToolBarItem::identifier() const
{
    return m_identifier;
}

bool MacToolBarItem::selectable()
{
    return m_selectable;
}

void MacToolBarItem::setSelectable(bool selectable)
{
    m_selectable = selectable;
}

