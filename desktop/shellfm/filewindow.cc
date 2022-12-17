#include "filewindow.h"
#include "fmapplication.h"

#include <QToolButton>

FileWindow::FileWindow(HWShell::WindowMode windowMode, QWidget *parent)
    : QMainWindow(parent),
      m_shellHost(new LSEmbeddedShellHost(this)),
      m_windowMode(windowMode),
      m_menuBar(new QMenuBar(this)),
      m_search(new QLineEdit(this))
{
    setObjectName("FMWindow");
    setupActions();
    setupMenuBar();
    setupMainView();
}

FileWindow::~FileWindow()
{

}

void FileWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("originull", "persistence");
    // preserve our view states
    settings.beginGroup("ShellFM");
    settings.setValue("Cache/LastWindowPos", saveGeometry());

    event->accept();
}

void FileWindow::shWindowTitleChanged(const QString &title)
{
    setWindowTitle(title);
}

void FileWindow::setupMenuBar()
{
    /* We do the menu bar separate because it can be used
     * by both a browser window and the root desktop window
     * over dbusmenu */

    m_menuBar->setObjectName(QString::fromUtf8("m_menuBar"));
    menu_File = new QMenu(m_menuBar);
    menu_Edit = new QMenu(m_menuBar);
    menu_View = new QMenu(m_menuBar);
    menu_Go = new QMenu(m_menuBar);
    menu_Bookmark = new QMenu(m_menuBar);
    menu_Help = new QMenu(m_menuBar);
    menuRecent_Locations = new QMenu(menu_Go);
    menu_Sort = new QMenu(menu_View);

    menu_File->setObjectName(QString::fromUtf8("FileMenu"));
    menu_Edit->setObjectName(QString::fromUtf8("EditMenu"));
    menu_View->setObjectName(QString::fromUtf8("ViewMeu"));
    menu_Sort->setObjectName(QString::fromUtf8("SortOrderMenu"));
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
    menu_Sort->setTitle(tr("&Organize By"));

    m_menuBar->addAction(menu_File->menuAction());
    m_menuBar->addAction(menu_Edit->menuAction());
    m_menuBar->addAction(menu_View->menuAction());
    m_menuBar->addAction(menu_Go->menuAction());
    m_menuBar->addAction(menu_Bookmark->menuAction());
    m_menuBar->addAction(menu_Help->menuAction());
    menu_File->addAction(a_NewWindow);
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_NEW_TAB));
    menu_File->addSeparator();
    menu_File->addMenu(m_shellHost->newMenu());
    m_shellHost->newMenu()->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_NEW_FOLDER));
    m_shellHost->newMenu()->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_NEW_FILE));
    m_shellHost->newMenu()->addSeparator();
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_OPEN));
    menu_File->addMenu(m_shellHost->openWithMenu());
    m_shellHost->openWithMenu()->addSeparator();
    m_shellHost->openWithMenu()->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_OPEN_WITH));
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_GET_INFO));
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_RENAME));
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_ARCHIVE));
    menu_File->addSeparator();
    menu_File->addAction(m_shellHost->shellAction(HWShell::ACT_FILE_TRASH));
    //menu_File->addAction(m_shellHost->shellAction(ArionShell::ACT_FILE_EJECT));
    menu_File->addSeparator();
    menu_File->addAction(a_Close_Window);
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_UNDO));
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_REDO));
    menu_Edit->addSeparator();
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_CUT));
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_COPY));
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_PASTE));
    menu_Edit->addSeparator();
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_SEL_ALL));
    menu_Edit->addAction(m_shellHost->shellAction(HWShell::ACT_EDIT_INV_SEL));

    menu_View->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_ICONS));
    menu_View->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_LIST));
    menu_View->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_COLUMNS));
    menu_View->addSeparator();
    menu_View->addAction(menu_Sort->menuAction());
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_NONE));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_NAME));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_SIZE));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_KIND));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_MODIFIED));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_OWNER));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_GROUP));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_COMMENT));
    menu_Sort->addSeparator();
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_ASC));
    menu_Sort->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_DESC));

    menu_View->addSeparator();
    menu_View->addAction(a_Toolbar);
    menu_View->addAction(a_StatusBar);
    menu_View->addSeparator();
    menu_View->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_OPTIONS));
    menu_Go->addAction(m_shellHost->shellAction(HWShell::ACT_GO_BACK));
    menu_Go->addAction(m_shellHost->shellAction(HWShell::ACT_GO_FORWARD));
    menu_Go->addAction(m_shellHost->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER));
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
}

void FileWindow::setupActions()
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
    a_Applications->setIcon(QIcon::fromTheme(QString::fromUtf8("folder-activities")));
    a_Applications->setShortcut(QKeySequence("Ctrl+Shift+A"));
    connect(a_Applications, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
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

    connect(a_Close_Window, SIGNAL(triggered()), this, SLOT(close()));
}

void FileWindow::setupMainView()
{
    QSettings settings("originull", "hollywood");
    settings.beginGroup("ShellFM");
    connect(m_shellHost, &LSEmbeddedShellHost::updateWindowTitle, this, &FileWindow::shWindowTitleChanged);

    /* Setup Toolbar & Status Bar */
    m_statusBar = new QStatusBar(this);
    m_statusBar->setObjectName(QString::fromUtf8("StatusBar"));
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QString::fromUtf8("StatusLabel"));
    m_statusBar->addPermanentWidget(m_statusLabel, 1);
    m_toolBar = new QToolBar(this);
    m_toolBar->setObjectName(QString::fromUtf8("ToolBar"));
    m_toolBar->addAction(m_shellHost->shellAction(HWShell::ACT_GO_BACK));
    m_toolBar->addAction(m_shellHost->shellAction(HWShell::ACT_GO_FORWARD));
    m_toolBar->addAction(m_shellHost->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER));
    m_toolBar->addWidget(m_shellHost->locationBar());
    m_sort = new QToolButton(m_toolBar);
    m_sort->setPopupMode(QToolButton::MenuButtonPopup);
    m_sort->setText(tr("Sort"));
    m_sort->setIcon(QIcon::fromTheme("view-sort"));
    auto sortmenu = new QMenu(m_sort);
    auto header = sortmenu->addAction(tr("Organize by..."));
    header->setEnabled(false);
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_NONE));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_NAME));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_SIZE));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_KIND));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_MODIFIED));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_OWNER));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_GROUP));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_COMMENT));
    sortmenu->addSeparator();
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_ASC));
    sortmenu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_SORT_DESC));

    m_view = new QToolButton(m_toolBar);
    m_view->setPopupMode(QToolButton::MenuButtonPopup);
    m_view->setText(tr("Sort"));
    m_view->setIcon(QIcon::fromTheme("view-sort"));
    auto menu = new QMenu(m_view);
    menu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_ICONS));
    menu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_LIST));
    menu->addAction(m_shellHost->shellAction(HWShell::ACT_VIEW_COLUMNS));

    m_view->setMenu(menu);
    m_sort->setMenu(sortmenu);
    m_view->setAutoRaise(true);
    m_sort->setAutoRaise(true);
    m_toolBar->addWidget(m_sort);
    m_toolBar->addWidget(m_view);
    m_toolBar->addWidget(m_search);
    m_search->setMaximumWidth(150);
    m_search->setDisabled(true);
    auto *icon = new QIcon(QIcon::fromTheme("search"));
    m_search->addAction(*icon, QLineEdit::LeadingPosition);
    m_search->setPlaceholderText(tr("Search"));
    m_toolBar->setWindowTitle(tr("Toolbar"));
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);
    m_shellHost->locationBar()->setVisible(true);
    setStatusBar(m_statusBar);
    setMenuBar(m_menuBar);
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    bool showStatus = settings.value("Preferences/ShowStatusBar", true).toBool();
    if(showStatus)
    {
        m_statusBar->setVisible(true);
        a_StatusBar->setChecked(true);
    }
    else
    {
        m_statusBar->setVisible(false);
        a_StatusBar->setChecked(false);
    }
    menu_View->insertAction(a_Toolbar, m_toolBar->toggleViewAction());
    menu_View->removeAction(a_Toolbar);
    m_toolBar->toggleViewAction()->setShortcut(QKeySequence::fromString("Ctrl+Shift+T"));

    connect(m_shellHost, &LSEmbeddedShellHost::viewModeChanged, this, &FileWindow::viewModeChanged);
    connect(m_shellHost, &LSEmbeddedShellHost::sortOrderChanged, this, &FileWindow::sortOrderChanged);
    connect(m_shellHost, &LSEmbeddedShellHost::updateStatusBar, m_statusLabel, &QLabel::setText);
    setCentralWidget(m_shellHost);

    /* now that the window is configured lets
     * restore our previous settings */

    QSettings settings2("originull", "persistence");
    settings.beginGroup("ShellFM");
    if(settings.contains("Cache/LastWindowPos"))
        restoreGeometry(settings.value("Cache/LastWindowPos").toByteArray());
    else
    {
        // fallback defaults
        resize(800,500);
        move(screen()->geometry().center() - frameGeometry().center());
    }
}

void FileWindow::statusBarActionToggled(bool toggled)
{
    QSettings settings("originull", "hollywood");
    settings.beginGroup("ShellFM");
    m_statusBar->setVisible(toggled);
    settings.setValue("Preferences/ShowStatusBar", toggled);
}


void FileWindow::goQuickAction()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString actionName = action->objectName();
    QString path("");

    if(actionName == "GoHome")
        path = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();

    if(actionName == "GoDocuments")
        path = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();

    if(actionName == "GoDownloads")
        path = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();

    if(actionName == "GoDesktop")
        path = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();

    if(actionName == "GoPhotos")
        path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();

    if(actionName == "GoVideos")
        path = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first();

    if(actionName == "GoMusic")
        path = QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first();

    if(!path.isEmpty())
    {
        m_shellHost->navigateToPath(path);
        return;
    }

    if(actionName == "GoApplications")
    {
        m_shellHost->navigateToUrl(QUrl("applications://"));
        return;
    }
    if(!path.isEmpty())
    {
    //    if(m_windowMode == ArionShell::WINDOW_BROWSER)
     //       m_shellHost->navigateToPath(path);

        // TODO: desktop mode
    }
}

void FileWindow::viewModeChanged()
{
    if(m_view)
    {
        auto action = m_shellHost->groupViewMode()->checkedAction();
        if(action && !action->icon().isNull())
            m_view->setIcon(action->icon());
    }
}

void FileWindow::sortOrderChanged()
{
    if(m_sort)
    {
        auto action = m_shellHost->groupViewOrder()->checkedAction();
        if(action && !action->icon().isNull())
            m_sort->setIcon(action->icon());
    }
}
