
#include "mac/qsearchfield.h"

#import "Foundation/NSAutoreleasePool.h"
#import "Foundation/NSNotification.h"
#import "AppKit/NSSearchField.h"

#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>
#include <QLayout>
#include <QMacCocoaViewContainer>
#include <QString>
#include <QCoreApplication>

SearchWidget::SearchWidget(QWidget *parent)
    : QMacCocoaViewContainer(0, parent)
{
    // Many Cocoa objects create temporary autorelease objects,
    // so create a pool to catch them.
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    m_search = [[NSSearchField alloc] init];


    setCocoaView(m_search);

    //QMenu *qtMenu = createSearchMenu(this);
    //NSMenu *nsMenu = qtMenu->toNSMenu();
    //[[m_search cell] setSearchMenuTemplate:nsMenu];

    [m_search release];
    [pool release];
}

SearchWidget::~SearchWidget()
{
}

QSize SearchWidget::sizeHint() const
{
    return QSize(150, 40);
}

QMenu *createSearchMenu(QWidget *parent)
{
    QMenu *searchMenu = new QMenu(parent);

    QAction * indexAction = searchMenu->addAction("Index Search");
    indexAction->setCheckable(true);
    indexAction->setChecked(true);

    QAction * fulltextAction = searchMenu->addAction("Full Text Search");
    fulltextAction->setCheckable(true);

    QActionGroup *searchActionGroup = new QActionGroup(parent);
    searchActionGroup->addAction(indexAction);
    searchActionGroup->addAction(fulltextAction);
    searchActionGroup->setExclusive(true);

    return searchMenu;
}
