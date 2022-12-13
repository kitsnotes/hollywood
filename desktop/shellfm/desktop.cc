#include "desktop.h"
#include "fmapplication.h"
#include <actionmanager.h>
#include <filesystemmodel.h>
#include <fsitemdelegate.h>
#include <executor.h>
#include <desktopmodel.h>
#include <desktopentry.h>

DesktopWindow::DesktopWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QListView(this))
    , m_model(new LSDesktopModel(this))
    , m_delegate(new LSFSItemDelegate(this))
    , m_actions(new LSActionManager(this))
    , m_rightclick(new QMenu(this))
{
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
    setObjectName("DesktopWindow");
    setCentralWidget(m_view);
    m_view->setModel(m_model);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    //connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &FileWindow::desktopGeometryChanged);

    setWindowFlag(Qt::Widget, true);
    setWindowFlag(Qt::FramelessWindowHint, true);
    setWindowFlag(Qt::WindowStaysOnBottomHint, true);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setViewMode(QListView::IconMode);
    //m_view->setModel(m_model);
    m_view->setIconSize(QSize(32,32));
    m_view->setSpacing(8);
    m_view->setFlow(QListView::TopToBottom);
    m_view->setUniformItemSizes(false);
    m_view->setWordWrap(true);
    auto font = m_view->font();
    font.setBold(true);
    m_view->setFont(font);
    m_view->setItemAlignment(Qt::AlignHCenter);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setItemDelegate(m_delegate);
    m_delegate->setShadowHidden(false);

    QPalette p = m_view->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, QColor(Qt::white));
    m_view->setPalette(p);
    m_delegate->setShadowColor(QColor(Qt::black));

    setupActions();
    setupMenuBar();

    connect(m_view, &QAbstractItemView::activated,
            this, &DesktopWindow::activated);

    connect(m_view, &QAbstractItemView::customContextMenuRequested,
            this, &DesktopWindow::contextMenu);

    resize(qApp->primaryScreen()->size());
}

QAction* DesktopWindow::shellAction(HWShell::ShellActions shellAction)
{
    return m_actions->shellAction(shellAction);
}

void DesktopWindow::activated(const QModelIndex &idx)
{
    if(m_model->isTrash(idx))
    {
        FMApplication::instance()->newFileWindow(QUrl("trash://"));

        return;
    }

    QFileInfo fileInfo(m_model->fileInfo(idx));
    if(fileInfo.isDir())
    {
        if(fileInfo.isReadable() && fileInfo.isExecutable())
            FMApplication::instance()->newFileWindow(QUrl::fromLocalFile(fileInfo.canonicalFilePath()));
        else
            FMApplication::instance()->showPermissionError(QUrl(fileInfo.canonicalFilePath()));

        return;
    }

    // handle a .desktop file
    if(m_model->isDesktop(idx))
    {
        FMApplication::instance()->executeDesktopOverDBus(m_model->desktopFileForIndex(idx)->fileName());
        return;
    }

    // TODO: handle an appimage
    //openFileOverDBusWithDefault(fileInfo.canonicalFilePath());

    // TODO: handle executing an executable file???
}

void DesktopWindow::contextMenu(const QPoint &pos)
{
    // provision the standard nosel context menu
    if(m_view->selectionModel()->selectedIndexes().isEmpty())
        m_rightclick->popup(pos);
}

void DesktopWindow::setupActions()
{
    FMApplication *myApp = FMApplication::instance();
    a_NewWindow = new QAction(this);
    a_NewWindow->setObjectName(QString::fromUtf8("FileNewWindow"));
    a_NewWindow->setShortcut(QKeySequence(QKeySequence::New));
    connect(a_NewWindow, SIGNAL(triggered()),
            myApp, SLOT(newFileWindow()));

    a_Close_Window = new QAction(this);
    a_Close_Window->setObjectName(QString::fromUtf8("FileCloseWindow"));
    a_Close_Window->setShortcut(QKeySequence(QKeySequence::Close));

    // filler action - we really rely on the one
    // provided by QToolbar in real (nondesktop) windows
    a_Toolbar = new QAction(this);
    a_Toolbar->setCheckable(true);
    a_Toolbar->setDisabled(true);

    a_StatusBar = new QAction(this);
    a_StatusBar->setObjectName(QString::fromUtf8("ViewStatusBar"));
    a_StatusBar->setCheckable(true);
    connect(a_StatusBar, SIGNAL(toggled(bool)),
            this, SLOT(statusBarActionToggled(bool)));

    a_Home = new QAction(this);
    a_Home->setObjectName(QString::fromUtf8("GoHome"));
    a_Home->setIcon(QIcon::fromTheme("user-home"));
    a_Home->setShortcut(QKeySequence("Ctrl+Shift+H"));
    connect(a_Home, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Documents = new QAction(this);
    a_Documents->setObjectName(QString::fromUtf8("GoDocuments"));
    a_Documents->setIcon(QIcon::fromTheme("folder-documents"));
    a_Documents->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(a_Documents, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Photos = new QAction(this);
    a_Photos->setObjectName(QString::fromUtf8("GoPhotos"));
    a_Photos->setIcon(QIcon::fromTheme("folder-pictures"));
    connect(a_Photos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));

    a_Videos = new QAction(this);
    a_Videos->setObjectName(QString::fromUtf8("GoVideos"));
    a_Videos->setIcon(QIcon::fromTheme("folder-videos"));
    connect(a_Videos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Music = new QAction(this);
    a_Music->setObjectName(QString::fromUtf8("GoMusic"));
    a_Music->setIcon(QIcon::fromTheme("folder-music"));
    connect(a_Music, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Desktop = new QAction(this);
    a_Desktop->setObjectName(QString::fromUtf8("GoDesktop"));
    a_Desktop->setIcon(QIcon::fromTheme("user-desktop"));
    connect(a_Desktop, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Applications = new QAction(this);
    a_Applications->setObjectName(QString::fromUtf8("GoApplications"));
    a_Applications->setEnabled(false);
    a_Applications->setShortcut(QKeySequence("Ctrl+Shift+A"));
    a_Network = new QAction(this);
    a_Network->setObjectName(QString::fromUtf8("GoNetwork"));
    a_Network->setIcon(QIcon::fromTheme(QString::fromUtf8("network-workgroup")));
    a_Network->setShortcut(QKeySequence("Ctrl+Shift+N"));
    a_Network->setEnabled(false);

    a_Connect_to_Server = new QAction(this);
    a_Connect_to_Server->setObjectName(QString::fromUtf8("ConnectServer"));
    a_Connect_to_Server->setShortcut(QKeySequence("Ctrl+K"));

    // Help Menu
    actionGet_System_Help = new QAction(this);
    actionGet_System_Help->setObjectName(QString::fromUtf8("HelpSystem"));

    a_About = new QAction(this);
    a_About->setObjectName("ActionAboutShell");
    a_About->setText(tr("About &Shell"));
    connect(a_About, SIGNAL(triggered()), myApp, SLOT(aboutApplication()));

    // Bookmark Menu
    a_Add_Bookmark = new QAction(this);
    a_Add_Bookmark->setObjectName(QString::fromUtf8("BookmarkAdd"));

    a_NewWindow->setText(tr("New &Window"));
    a_Close_Window->setText(tr("&Close Window"));
    actionGet_System_Help->setText(tr("Get System &Help"));
    a_Add_Bookmark->setText(tr("&Add Bookmark..."));
    a_Toolbar->setText(tr("Toolbar"));
    a_StatusBar->setText(tr("Status Bar"));

    //a_Clear_List->setText(tr("&Clear List"));
    a_Home->setText(tr("&Home Folder"));
    a_Documents->setText(tr("&Documents"));
    a_Photos->setText(tr("&Photos"));
    a_Videos->setText(tr("&Videos"));
    a_Music->setText(tr("&Music"));
    a_Desktop->setText(tr("Des&ktop"));
    a_Applications->setText(tr("&Applications"));
    a_Network->setText(tr("&Network"));
    a_Connect_to_Server->setText(tr("&Connect to Server..."));

    a_Home->setIconVisibleInMenu(true);
    a_Documents->setIconVisibleInMenu(true);
    a_Desktop->setIconVisibleInMenu(true);
    a_Music->setIconVisibleInMenu(true);
    a_Videos->setIconVisibleInMenu(true);
    a_Home->setIconVisibleInMenu(true);
    a_Applications->setIconVisibleInMenu(true);
    a_Network->setIconVisibleInMenu(true);

    // FM window ONLY connections:
    /*  */

}

void DesktopWindow::setupMenuBar()
{
    /* We do the menu bar separate because it can be used
     * by both a browser window and the root desktop window
     * over dbusmenu */
    m_menuBar = new QMenuBar(this);
    setMenuBar(m_menuBar);
    m_menuBar->setObjectName(QString::fromUtf8("m_menuBar"));
    menu_File = new QMenu(m_menuBar);
    menu_Edit = new QMenu(m_menuBar);
    menu_View = new QMenu(m_menuBar);
    menu_Go = new QMenu(m_menuBar);
    menu_Bookmark = new QMenu(m_menuBar);
    menu_Help = new QMenu(m_menuBar);
    menuRecent_Locations = new QMenu(menu_Go);

    menu_File->setObjectName(QString::fromUtf8("FileMenu"));
    menu_Edit->setObjectName(QString::fromUtf8("EditMenu"));
    menu_View->setObjectName(QString::fromUtf8("ViewMeu"));
    menu_Go->setObjectName(QString::fromUtf8("GoMenu"));
    menuRecent_Locations->setObjectName(QString::fromUtf8("RecentLocationsMenu"));
    menu_Bookmark->setObjectName(QString::fromUtf8("BookmarskMeu"));
    menu_Help->setObjectName(QString::fromUtf8("HelpMenu"));

    menu_File->setTitle(tr("&File"));
    menu_Edit->setTitle(tr("&Edit"));
    menu_View->setTitle(tr("&View"));
    menu_Go->setTitle(tr("&Go"));
    menuRecent_Locations->setTitle(tr("Recent &Locations"));
    menu_Bookmark->setTitle(tr("&Bookmarks"));
    menu_Help->setTitle(tr("&Help"));

    m_menuBar->addAction(menu_File->menuAction());
    m_menuBar->addAction(menu_Edit->menuAction());
    m_menuBar->addAction(menu_View->menuAction());
    m_menuBar->addAction(menu_Go->menuAction());
    m_menuBar->addAction(menu_Bookmark->menuAction());
    m_menuBar->addAction(menu_Help->menuAction());
    menu_File->addAction(a_NewWindow);
    menu_File->addAction(shellAction(HWShell::ACT_FILE_NEW_TAB));
    menu_File->addSeparator();
    menu_File->addAction(shellAction(HWShell::ACT_FILE_NEW_FOLDER));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_NEW_FILE));
    menu_File->addSeparator();
    //menu_File->addAction(shellAction(ArionShell::ACT_FILE_OPEN));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_OPEN_WITH));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_GET_INFO));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_RENAME));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_ARCHIVE));
    menu_File->addSeparator();
    menu_File->addAction(shellAction(HWShell::ACT_FILE_TRASH));
    //menu_File->addAction(shellAction(ArionShell::ACT_FILE_EJECT));
    menu_File->addSeparator();
    menu_File->addAction(a_Close_Window);
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_UNDO));
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_REDO));
    menu_Edit->addSeparator();
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_CUT));
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_COPY));
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_PASTE));
    menu_Edit->addSeparator();
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_SEL_ALL));
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_INV_SEL));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_ICONS));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_LIST));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_COLUMNS));
    menu_View->addSeparator();
    menu_View->addAction(a_Toolbar);
    menu_View->addAction(a_StatusBar);
    menu_View->addSeparator();
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_OPTIONS));
    menu_Go->addAction(shellAction(HWShell::ACT_GO_BACK));
    menu_Go->addAction(shellAction(HWShell::ACT_GO_FORWARD));
    menu_Go->addAction(shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER));
    menu_Go->addSeparator();
    menu_Go->addAction(a_Home);
    menu_Go->addAction(a_Documents);
    menu_Go->addAction(a_Photos);
    menu_Go->addAction(a_Videos);
    menu_Go->addAction(a_Music);
    menu_Go->addAction(a_Desktop);
    menu_Go->addAction(a_Network);
    menu_Go->addAction(a_Applications);
    menu_Go->addSeparator();
    menu_Go->addAction(menuRecent_Locations->menuAction());
    menu_Go->addSeparator();
    menu_Go->addAction(a_Connect_to_Server);
    //menuRecent_Locations->addAction(a_Clear_List);
    menu_Bookmark->addAction(a_Add_Bookmark);
    menu_Help->addAction(actionGet_System_Help);
    menu_Help->addAction(a_About);
    //setMenuBar(m_menuBar);

    m_rightclick->addAction(tr("Paste"));
    m_rightclick->addAction(tr("Paste Symbolic Links"));
    auto wp = m_rightclick->addAction(tr("Change Wallpaper..."));
    connect(wp, &QAction::triggered, FMApplication::instance(), &FMApplication::showWallpaperSettings);
}

void DesktopWindow::desktopGeometryChanged(const QRect &geom)
{
    resize(geom.size());
}
