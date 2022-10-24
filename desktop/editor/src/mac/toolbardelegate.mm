#import "mac/toolbardelegate.h"

@implementation MacToolbarDelegate

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
    Q_UNUSED(toolbar);
    return myToolbar->getItemIdentifiers(myToolbar->m_items);
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
    Q_UNUSED(toolbar);
    return myToolbar->getItemIdentifiers(myToolbar->m_allowedItems);
}

- (NSArray *)toolbarSelectableItemIdentifiers: (NSToolbar *)toolbar
{
    Q_UNUSED(toolbar);
    NSMutableArray *array = myToolbar->getItemIdentifiers(myToolbar->m_items, true);
    [array addObjectsFromArray:myToolbar->getItemIdentifiers(myToolbar->m_allowedItems, true)];
    return array;
}

- (IBAction)itemClicked:(id)sender
{
    NSToolbarItem *item = reinterpret_cast<NSToolbarItem *>(sender);
    myToolbar->itemClicked(item);
}

- (NSToolbarItem *) toolbar: (NSToolbar *)toolbar itemForItemIdentifier: (NSString *) itemIdentifier willBeInsertedIntoToolbar:(BOOL) willBeInserted
{
    Q_UNUSED(toolbar);
    Q_UNUSED(willBeInserted);
    const QString identifier = QString::fromNSString(itemIdentifier);
    MacToolBarItem *toolButton =
            myToolbar->findItemByIdentifier(identifier);
    NSToolbarItem *toolbarItem = toolButton->nativeToolBarItem();

    [toolbarItem setTarget:self];
    [toolbarItem setAction:@selector(itemClicked:)];

    return toolbarItem;
}

@end
