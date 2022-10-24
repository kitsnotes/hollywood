#ifndef TOOLBARDELEGATE_H
#define TOOLBARDELEGATE_H

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#include "luvedit.h"

@interface MacToolbarDelegate : NSObject <NSToolbarDelegate>
{
@public
    MacToolBar *myToolbar;
}

- (NSToolbarItem *) toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *) itemIdent willBeInsertedIntoToolbar:(BOOL) willBeInserted;
- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar;
- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar;
- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar;
- (IBAction)itemClicked:(id)sender;
@end

#endif // TOOLBARDELEGATE_H
