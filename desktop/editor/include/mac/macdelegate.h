#ifndef MACDELEGATE_H
#define MACDELEGATE_H

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

class Editor;

@interface EditorWidgetServicesResponder : NSResponder <NSApplicationDelegate, NSWindowDelegate>
/* @property Editor *editor_widget; */
@property NSObject *qt_delegate;
- (id)validRequestorForSendType:(NSString*)sendType returnType:(NSString*)returnType;
- (BOOL)writeSelectionToPasteboard:(NSPasteboard*)pboard types:(NSArray*)types;
//- (BOOL)readSelectionFromPasteboard:(NSPasteboard*)pboard type:(NSPasteboardType)type;
- (BOOL)respondsToSelector:(SEL)aSelector;
@end

#endif // MACDELEGATE_H
