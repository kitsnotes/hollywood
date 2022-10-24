#include "luvedit.h"
#import <AppKit/AppKit.h>

/* These are functions only used by LuvEdit on the macOS Platform
 * This has reliance on AppKit and MacOS 10.10+
 * */

void setWindowMenuMac(QMenu *windowMenu)
{
    [NSApp setWindowsMenu:windowMenu->toNSMenu()];
}

void displayNSMenuContextStyle(QWidget* parent, QMenu* m, QPoint p)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSMenu *menu = m->toNSMenu();

    NSPoint point = NSMakePoint(p.x() - 1, p.y());
    NSView *nsview = (NSView*)parent->winId();

    if(parent != NULL)
    {
        point.y = nsview.frame.size.height - point.y;
        point = [nsview convertPoint:point toView:nsview.window.contentView];
    }

    NSEvent *menuEvent = [NSEvent mouseEventWithType:NSRightMouseDown
                                              location:point
                                              modifierFlags:0
                                              timestamp:0
                                              windowNumber:nsview ? nsview.window.windowNumber : 0
                                              context:nil
                                              eventNumber:0
                                              clickCount:1
                                              pressure:1.0];


    myApp->processEvents();
    if(menu)
        [NSMenu popUpContextMenu:menu withEvent:menuEvent forView:nsview];

    [pool release];
    m->deleteLater();
}

void setQMainWindowTitlebarOff(QMainWindow *m)
{
    NSWindow *nsWin = [(NSView*)m->winId() window];
    nsWin.titleVisibility = NSWindowTitleHidden;
}

void setTabBarAccessory(TabWidget *t)
{
    NSTitlebarAccessoryViewController* vc = [[NSTitlebarAccessoryViewController alloc] init];

    vc.view = (NSView*)t->tabBar()->winId();
    vc.layoutAttribute = NSLayoutAttributeBottom;

    //t->tabBar()->setParent(0);
    EditorWindow *w = qobject_cast<EditorWindow*>(t->parent());
    if(w)
        [[(NSView*)w->winId() window] addTitlebarAccessoryViewController:vc];
}

void removeTabBarAccessory(TabWidget *t)
{
    EditorWindow *w = qobject_cast<EditorWindow*>(t->parent());
    if(w)
        [[(NSView*)w->winId() window] removeTitlebarAccessoryViewControllerAtIndex:0];
}

void disableWindowTabbing()
{
    if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)])
        NSWindow.allowsAutomaticWindowTabbing = NO;
}
