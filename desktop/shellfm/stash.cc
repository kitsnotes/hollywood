/* Setup the main window splitter */
m_mainSplitter = new QSplitter(this);
m_mainSplitter->setOrientation(Qt::Horizontal);
/* Setup the places and directory trees */

m_treeToolbox = new QTabWidget(m_mainSplitter);
m_treeToolbox->setObjectName(QString::fromUtf8("TreeToolbox"));
m_treeToolbox->setTabPosition(QTabWidget::South);
m_treeToolbox->setUsesScrollButtons(false);
m_treeToolbox->tabBar()->setDrawBase(false);

m_treeFavorites = new QTreeView(this);
m_treeFavorites->setObjectName(QString::fromUtf8("PlacesTree"));

m_treeDirs = new QTreeView(this);
m_treeDirs->setObjectName(QString::fromUtf8("DirectoryTree"));

m_treeToolbox->resize(m_treeToolbox->height(), 125);

m_treeToolbox->addTab(m_treeFavorites, tr("Places"));
m_treeToolbox->addTab(m_treeDirs, tr("Directories"));
m_treeToolbox->setMaximumWidth(155);
m_treeToolbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


m_mainSplitter->setStretchFactor(1, 15);

/* setup tab bar */

m_tabWndHost = new QWidget(this);
m_tabWndHostLayout = new QVBoxLayout(m_tabWndHost);
m_tabs = new QTabBar(this);
m_tabs->setTabsClosable(true);
m_tabs->setMovable(true);
m_tabs->setAutoHide(settings.value("Preferences/AutoHideTabs", true).toBool());
connect(m_tabs, SIGNAL(currentChanged(int)),
        this, SLOT(currentTabChanged(int)));
connect(m_tabs, SIGNAL(tabCloseRequested(int)),
        this, SLOT(tabCloseRequested(int)));

m_tabWndHostLayout->addWidget(m_tabs);
#if QT_VERSION < 0x060000
m_tabWndHostLayout->setMargin(0);
#endif
/* Setup our main right-side views */

m_filesColumn = new QColumnView(this);
m_filesColumn->setObjectName(QString::fromUtf8("FilesColumn"));
m_filesColumn->setModel(m_model);

/* Setup Icon View */
m_filesList = new QListView(this);
m_filesList->setObjectName(QString::fromUtf8("FilesIcon"));
m_filesList->setModel(m_model);
m_filesList->setIconSize(QSize(32,32));
m_filesList->setViewMode(QListView::IconMode);
m_filesList->setFlow(QListView::LeftToRight);
m_filesList->setTextElideMode(Qt::ElideMiddle);
m_filesList->setWrapping(true);
m_filesList->setWordWrap(true);
m_filesList->setUniformItemSizes(true);


/* Setup List View */
m_filesTable = new QTreeView(this);
m_filesTable->setObjectName(QString::fromUtf8("FilesList"));
m_filesTable->setModel(m_model);
m_filesTable->setAlternatingRowColors(true);
m_filesTable->setAnimated(true);
m_filesTable->setExpandsOnDoubleClick(false);

/* Only the icons is visible here - window setup
 * procedure will set the default where required */

m_filesColumn->setVisible(false);
m_filesTable->setVisible(false);
m_tabWndHostLayout->addWidget(m_filesList);

m_mainSplitter->addWidget(m_treeToolbox);
m_mainSplitter->addWidget(m_tabWndHost);
