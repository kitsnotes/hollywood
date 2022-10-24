#import "mac/macdelegate.h"

#include "luvedit.h"

@implementation EditorWidgetServicesResponder
- (id)validRequestorForSendType:(NSString*)sendType returnType:(NSString*)returnType
{
    if([sendType isEqual:NSStringPboardType])
    {
        if(myApp->currentWindow()->tabWidget()->currentDocument()->editorWidget()
                    ->textCursor().hasSelection())
            return self;
        else
            return nil;
    }

    if([returnType isEqual:NSStringPboardType])
        return self;

    return nil;
}

- (BOOL)writeSelectionToPasteboard:(NSPasteboard*)pboard types:(NSArray*)types
{
    Q_UNUSED(types);

    if(myApp->allWindows().count() == 0)
        return NO;


    if(myApp->currentWindow()->tabWidget()->currentDocument()->editorWidget()
            ->textCursor().hasSelection())
    {
        [pboard declareTypes:@[NSStringPboardType] owner:nil];
        QString str = myApp->currentWindow()->tabWidget()->currentDocument()->editorWidget()
                ->textCursor().selectedText();
        return [pboard setString:str.toNSString() forType:NSStringPboardType];
    }

    return NO;
}

/*- (BOOL)readSelectionFromPasteboard:(NSPasteboard*)pboard type:(NSPasteboardType)type
{
    NSString* myString = [pboard  stringForType:NSPasteboardTypeString];

    return [super readSelectionFromPasteboard:pboard type:type];
}*/

- (BOOL)respondsToSelector:(SEL)aSelector
{
    return [_qt_delegate respondsToSelector:aSelector] ||
            [super respondsToSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation*)anInvocation
{
    [anInvocation invokeWithTarget:_qt_delegate];
}
@end

EditorWidgetServicesResponder* installResponder()
{
    EditorWidgetServicesResponder *resp = [[EditorWidgetServicesResponder alloc] init];
    resp.qt_delegate = NSApp.delegate;
    NSApp.delegate = resp;

    return resp;
}

void setupServicesMenu()
{
    [NSApp registerServicesMenuSendTypes:[NSArray arrayWithObjects:NSStringPboardType, nil] returnTypes:[NSArray arrayWithObjects:NSStringPboardType, nil]];
}
