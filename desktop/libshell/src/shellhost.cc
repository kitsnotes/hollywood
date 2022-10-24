#include "shellhost.h"
#include "viewoptionsdialog.h"
#include "actionmanager.h"
#include "locationbar.h"
#include "filesystemmodel.h"
#include "placemodel.h"
#include "placeview.h"
#include "columnpreview.h"
#include "fsitemdelegate.h"
#include "executor.h"

LSEmbeddedShellHost::LSEmbeddedShellHost(QWidget *parent)
    : QWidget(parent),
      m_actions(new LSActionManager(this)),
      m_filesList(new QListView(this)),
      m_model(new FilesystemModel(this)),
      m_placeModel(new LSPlaceModel(this)),
      m_viewOptions(new ViewOptionsDialog(this)),
      m_delegate(new LSFSItemDelegate(this))
{
    setContentsMargins(0,0,0,0);
    // TODO: arion shell (not app) settings
    // since this is a library
    QSettings settings("originull", "hollywood");
    m_model->setRootPath("/");

    setupLocationBar();

    /* Setup the main window splitter */
    m_mainSplitter = new QSplitter(this);
    m_mainSplitter->setOrientation(Qt::Horizontal);
    m_mainSplitter->setStretchFactor(1, 15);

    /* Setup the places and directory trees (left side of m_mainSplitter) */
    m_treeToolbox = new QTabWidget(m_mainSplitter);
    m_treeToolbox->setObjectName(QString::fromUtf8("TreeToolbox"));
    m_treeToolbox->setTabPosition(QTabWidget::North);
    m_treeToolbox->setUsesScrollButtons(false);
    m_treeToolbox->tabBar()->setDrawBase(false);

    m_treeFavorites = new LSPlaceView(this);
    m_treeFavorites->setObjectName(QString::fromUtf8("PlacesTree"));
    m_treeFavorites->setModel(m_placeModel);
    m_treeFavorites->expandAll();
    connect(m_treeFavorites, &QTreeView::clicked, this, &LSEmbeddedShellHost::placeClicked);

    //m_treeFavorites->setFirstColumnSpanned()

    m_treeDirs = new QTreeView(this);
    m_treeDirs->setObjectName(QString::fromUtf8("DirectoryTree"));

    m_treeToolbox->resize(m_treeToolbox->height(), 125);

    m_treeToolbox->addTab(m_treeFavorites, tr("Places"));
    m_treeToolbox->addTab(m_treeDirs, tr("Directories"));
    m_treeToolbox->setMaximumWidth(155);
    m_treeToolbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    /* setup tab bar host (right side of m_mainSplitter) */
    m_tabWndHost = new QWidget(m_mainSplitter);

    m_tabWndHostLayout = new QVBoxLayout(m_tabWndHost);
    m_tabWndHostLayout->setSpacing(0);
    m_tabWndHostLayout->setContentsMargins(0,0,0,0);
    m_tabs = new QTabBar(m_tabWndHost);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    m_tabs->setAutoHide(settings.value("Preferences/AutoHideTabs", true).toBool());
    connect(m_tabs, SIGNAL(currentChanged(int)),
            this, SLOT(currentTabChanged(int)));
    connect(m_tabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(tabCloseRequested(int)));

    m_tabWndHostLayout->addWidget(m_tabs);

    /* Setup our main right-side views */

    m_filesColumn = new QColumnView(this);
    m_filesColumn->setObjectName(QString::fromUtf8("FilesColumn"));
    m_filesColumn->setModel(m_model);
    m_filesColumn->setAutoScroll(true);

    m_columnPreview = new LSColumnPreview(this);
    m_filesColumn->setPreviewWidget(m_columnPreview);

    /* Setup Icon View */
    m_filesList->setObjectName(QString::fromUtf8("FilesIcon"));
    m_filesList->setModel(m_model);
    m_filesList->setIconSize(QSize(48,48));
    m_filesList->setSpacing(10);
    m_filesList->setViewMode(QListView::IconMode);
    m_filesList->setFlow(QListView::LeftToRight);
    m_filesList->setTextElideMode(Qt::ElideMiddle);
    m_filesList->setWrapping(true);
    m_filesList->setWordWrap(true);
    m_filesList->setUniformItemSizes(false);
    m_filesList->setItemDelegate(m_delegate);
    m_filesList->setSelectionMode(QAbstractItemView::ContiguousSelection);


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
    m_viewOptions->attachIconView(m_filesList);

    connect(m_filesColumn, &QColumnView::updatePreviewWidget, this, &LSEmbeddedShellHost::updateColumnWidget);

    connect(m_model, SIGNAL(rootPathChanged(const QString&)),
            this, SLOT(modelRootPathChanged(const QString&)));

    connect(m_actions->shellAction(ArionShell::ACT_VIEW_OPTIONS),
            &QAction::triggered, this, &LSEmbeddedShellHost::toggleViewOptions);
    connect(m_viewOptions, &ViewOptionsDialog::finished,
            this, &LSEmbeddedShellHost::viewOptionsFinished);

    connect(m_actions->shellAction(ArionShell::ACT_VIEW_ICONS),
            &QAction::triggered, this, &LSEmbeddedShellHost::setIconListView);
    connect(m_actions->shellAction(ArionShell::ACT_VIEW_LIST),
            &QAction::triggered, this, &LSEmbeddedShellHost::setTableListView);
    connect(m_actions->shellAction(ArionShell::ACT_VIEW_COLUMNS),
            &QAction::triggered, this, &LSEmbeddedShellHost::setColumnView);

    connect(m_actions->shellAction(ArionShell::ACT_FILE_NEW_TAB), &QAction::triggered,
             this, &LSEmbeddedShellHost::createNewTab);
    connect(m_actions->shellAction(ArionShell::ACT_GO_BACK), SIGNAL(triggered()),
            this, SLOT(goBack()));
    connect(m_actions->shellAction(ArionShell::ACT_GO_FORWARD), SIGNAL(triggered()),
            this, SLOT(goForward()));

    connect(m_actions->shellAction(ArionShell::ACT_GO_ENCLOSING_FOLDER), SIGNAL(triggered()),
            this, SLOT(goEnclosingFolder()));

    restoreViewModeFromSettings();
}

void LSEmbeddedShellHost::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_mainSplitter->resize(event->size());
}

void LSEmbeddedShellHost::locationBarEditingFinished()
{

}

void LSEmbeddedShellHost::setupLocationBar()
{
    // we don't always use this so we provision it
    // only as needed to save on resources
    if(m_location)
        return;

    m_location = new LSLocationBar(this);
    connect(m_location, &LSLocationBar::chdir, this, &LSEmbeddedShellHost::navigateToUrl);
    connect(m_location, &LSLocationBar::middleClickChdir, this, &LSEmbeddedShellHost::newTabWithPath);
    connect(m_location, &LSLocationBar::editingFinished, this, &LSEmbeddedShellHost::locationBarEditingFinished);
}

LSLocationBar* LSEmbeddedShellHost::locationBar()
{
    return m_location;
}

void LSEmbeddedShellHost::restoreViewModeFromSettings()
{
    QSettings settings("originull", "hollywood");

    // restore our view mode from settings
    m_viewMode = (ArionShell::ViewMode)settings.value("Preferences/DefaultView", ArionShell::VIEW_ICONS).toInt();
    switch(m_viewMode)
    {
    case ArionShell::VIEW_COLUMN:
        setColumnView();
        break;
    case ArionShell::VIEW_LIST:
        setTableListView();
        break;
    case ArionShell::VIEW_ICONS:
    default:
        setIconListView();
    }
}

void LSEmbeddedShellHost::toggleViewOptions(bool checked)
{
    if(checked)
        m_viewOptions->show();
    else
        m_viewOptions->hide();
}

void LSEmbeddedShellHost::viewOptionsFinished(int result)
{
    Q_UNUSED(result);
    if(!m_viewOptions->isVisible())
        m_actions->shellAction(ArionShell::ACT_VIEW_OPTIONS)->setChecked(false);
}

void LSEmbeddedShellHost::tabCloseRequested(int index)
{
    QUuid uuid = m_tabs->tabData(index).toUuid();
    m_tabLocations.remove(uuid);
    m_tabViewMode.remove(uuid);
    m_tabs->removeTab(index);
    if(m_tabs->count() == 0)
        close();
}

bool LSEmbeddedShellHost::navigateToPath(const QString &path)
{
    QUrl url(QUrl::fromLocalFile(path));
    if(url.isValid())
        return navigateToUrl(url);

    return false;
}

bool LSEmbeddedShellHost::navigateToUrl(const QUrl &path)
{
    qDebug() << "navigateToUrl: " << path.toString();
    if(!path.isValid())
        return false;

    if(path.isLocalFile())
    {
        QString file = path.toLocalFile();
        QModelIndex idx = m_model->index(file);
        if(idx.isValid())
        {
            updateRootIndex(idx);
            return true;
        }
    }

    return false;
}


void LSEmbeddedShellHost::newTabWithPath(const QUrl &path)
{
    qDebug() << "newTabWithPath: " << path.toString();
    if(path.isLocalFile())
    {
     const QString localPath = path.toLocalFile();
     QDir dir(localPath);
     if(dir.isReadable())
     {
         int idx = m_tabs->addTab(localPath);
         QUuid tabId = QUuid::createUuid();
         m_tabs->setTabData(idx, tabId);
         // just temporary to add the map entry
         m_tabLocations.insert(tabId, QUrl::fromLocalFile("/"));
         m_tabViewMode.insert(tabId, m_viewMode);

         m_tabs->setCurrentIndex(idx);
         navigateToPath(localPath);
     }
     // TODO: error out here
    }
}

void LSEmbeddedShellHost::currentTabChanged(int index)
 {
    QUuid uuid = m_tabs->tabData(index).toUuid();
    QUrl path = m_tabLocations[uuid];
    ArionShell::ViewMode mode = m_tabViewMode[uuid];
    switch(mode)
    {
    case ArionShell::VIEW_COLUMN:
        setColumnView(false);
        break;
    case ArionShell::VIEW_LIST:
        setTableListView(false);
        break;
    case ArionShell::VIEW_ICONS:
    default:
        setIconListView(false);
    }
    navigateToUrl(path);
    emit updateStatusBar(generateStatusBarMsg());
 }


void LSEmbeddedShellHost::setIconListView(bool updateSettings)
{
    disconnect(this, nullptr, m_filesList, nullptr);
    disconnect(this, nullptr, m_filesColumn, nullptr);
    disconnect(this, nullptr, m_filesTable, nullptr);

    m_tabWndHostLayout->replaceWidget(m_tabWndHostLayout->itemAt(1)->widget(), m_filesList);
    m_filesColumn->setVisible(false);
    m_filesList->setVisible(true);
    m_filesTable->setVisible(false);
    m_actions->shellAction(ArionShell::ACT_VIEW_ICONS)->setChecked(true);
    connect(m_filesList, &QListView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    connect(m_filesList, &QListView::activated, this, &LSEmbeddedShellHost::viewDoubleClicked);
    connect(m_filesList->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    m_viewMode = ArionShell::VIEW_ICONS;
    QUuid uuid = m_tabs->tabData(m_tabs->currentIndex()).toUuid();
    m_tabViewMode[uuid] = m_viewMode;
    m_viewOptions->setCurrentView(ArionShell::VIEW_ICONS);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", m_viewMode);
    }
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::setColumnView(bool updateSettings)
{
    disconnect(this, nullptr, m_filesList, nullptr);
    disconnect(this, nullptr, m_filesColumn, nullptr);
    disconnect(this, nullptr, m_filesTable, nullptr);
    m_tabWndHostLayout->replaceWidget(m_tabWndHostLayout->itemAt(1)->widget(), m_filesColumn);
    m_filesColumn->setVisible(true);
    m_filesList->setVisible(false);
    m_filesTable->setVisible(false);
    m_actions->shellAction(ArionShell::ACT_VIEW_COLUMNS)->setChecked(true);
    connect(m_filesColumn, &QColumnView::doubleClicked, this, &LSEmbeddedShellHost::viewDoubleClicked);
    connect(m_filesColumn->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    m_viewMode = ArionShell::VIEW_COLUMN;
    QUuid uuid = m_tabs->tabData(m_tabs->currentIndex()).toUuid();
    m_tabViewMode[uuid] = m_viewMode;
    m_viewOptions->setCurrentView(ArionShell::VIEW_COLUMN);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", m_viewMode);
    }
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::setTableListView(bool updateSettings)
{
    disconnect(this, nullptr, m_filesList, nullptr);
    disconnect(this, nullptr, m_filesColumn, nullptr);
    disconnect(this, nullptr, m_filesTable, nullptr);

    m_tabWndHostLayout->replaceWidget(m_tabWndHostLayout->itemAt(1)->widget(), m_filesTable);
    m_filesColumn->setVisible(false);
    m_filesList->setVisible(false);
    m_filesTable->setVisible(true);
    m_actions->shellAction(ArionShell::ACT_VIEW_LIST)->setChecked(true);
    m_viewMode = ArionShell::VIEW_LIST;
    connect(m_filesTable, &QTreeView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    connect(m_filesTable, &QTreeView::doubleClicked, this, &LSEmbeddedShellHost::viewDoubleClicked);
    QUuid uuid = m_tabs->tabData(m_tabs->currentIndex()).toUuid();
    m_tabViewMode[uuid] = m_viewMode;
    m_viewOptions->setCurrentView(ArionShell::VIEW_LIST);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", m_viewMode);
    }
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::viewClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::viewDoubleClicked(const QModelIndex &idx)
{
    QFileInfo fileInfo(m_model->fileInfo(idx));
    if(fileInfo.isDir())
    {
        // TODO: Handle a spacitial mode
        if(fileInfo.isReadable() && fileInfo.isExecutable())
            updateRootIndex(idx);
        else
            showFolderPermissionError(fileInfo.fileName());
    }

    // handle a .desktop file
    if(m_model->isDesktop(idx))
    {
        qDebug() << "executing desktop";
        LSExecutor *exec = new LSExecutor(this);
        exec->setDesktopFile(m_model->desktopFileForIndex(idx));
        exec->launch();
        return;
    }

    // TODO: handle launching a non-executable file

    // TODO: handle executing an executable file
}

void LSEmbeddedShellHost::viewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::placeClicked(const QModelIndex &idx)
{
    navigateToUrl(m_placeModel->place(idx).location);
}

void LSEmbeddedShellHost::updateColumnWidget(const QModelIndex &idx)
{
    m_columnPreview->updateFileInfo(m_model->fileInfo(idx));
    m_columnPreview->show();
}

void LSEmbeddedShellHost::modelRootPathChanged(const QString &path)
{
    Q_UNUSED(path);
}

void LSEmbeddedShellHost::createNewTab()
{
    // TODO: user preference
    QString path =
            QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    newTabWithPath(QUrl::fromLocalFile(path));
}

void LSEmbeddedShellHost::updatePlaceModelSelection(const QUrl &place)
{
    m_treeFavorites->selectionModel()->clear();
    if(m_placeModel->hasPlaceForUrl(place))
    {
        const QModelIndex idx = m_placeModel->indexForPlace(m_placeModel->place(place));
        m_treeFavorites->selectionModel()->select(idx, QItemSelectionModel::Select);
    }
}

QString LSEmbeddedShellHost::generateStatusBarMsg() const
{
    QString fileName, fileSize, fileType;
    QModelIndex current;
    bool selected = false;
    int count = 0;
    switch(m_viewMode)
    {
    case ArionShell::VIEW_ICONS:
        current = m_filesList->currentIndex();
        selected = m_filesList->selectionModel()->hasSelection();
        count = selected ? m_filesList->selectionModel()->selectedIndexes().count() : 0;
        if(count == 1)
        {
            QFileInfo info =
                    m_model->fileInfo(m_filesList->selectionModel()->selectedIndexes().first());
            fileName = info.fileName();
            fileSize = tr("0KB");
            fileType = info.fileName();
        }
        break;
    case ArionShell::VIEW_LIST:
        current = m_filesTable->currentIndex();
        selected = m_filesTable->selectionModel()->hasSelection();
        count = selected ? m_filesTable->selectionModel()->selectedIndexes().count() : 0;
        if(count == 1)
        {
            QFileInfo info =
                    m_model->fileInfo(m_filesList->selectionModel()->selectedIndexes().first());
            fileName = info.fileName();
            fileSize = tr("0KB");
            fileType = info.fileName();
        }
        break;
    default:
        break;
    }

    if(selected)
    {
        if(count > 1)
            return tr("%1 items (%2)").arg(count).arg(count);
        else
            return tr("\"%1\" (%2) - %3").arg(fileName).arg(fileSize).arg(fileType);
    }
    else
    {
        if(current.isValid())
        {
            if(m_model->rowCount(current) == 1)
                return tr("%1 item").arg(m_model->rowCount(current));
            else
                return tr("%1 items").arg(m_model->rowCount(current));
        }
    }
    return QString();
}

void LSEmbeddedShellHost::showFolderPermissionError(const QString &folder)
{
    qDebug() << "showFolderPermissionError: " << folder;

    auto *msg = new QMessageBox(this);
    msg->setModal(true);
    msg->setIcon(QMessageBox::Information);
    msg->setWindowTitle(" ");
    msg->setText(tr("The folder \"%1\" can not be opened because you do not have permissions to see its contents.").arg(folder));
    msg->setInformativeText(tr("A system administrator can set the relevant execute flag to allow this."));
    msg->setStandardButtons(QMessageBox::Ok);
    msg->exec();

    delete msg;
}

void LSEmbeddedShellHost::updateRootIndex(const QModelIndex &idx)
{
    QSettings settings("originull", "hollywood");
    bool showFullPath = settings.value("Preferences/ShowFullPathName", false).toBool();
    QFileIconProvider icons;
    QFileInfo info(m_model->fileInfo(idx));
    qDebug() << "updateRootIndex: " << info.canonicalFilePath();

    m_filesList->setRootIndex(idx);

    // we shouldn't do the following two in desktop mode
    m_filesColumn->setRootIndex(idx);
    m_filesTable->setRootIndex(idx);

    // update our tab and window title
    QString directory = info.canonicalFilePath();
    QIcon ico = icons.icon(info);

    if(showFullPath)
    {
        emit updateWindowTitle(directory);
        m_tabs->setTabText(m_tabs->currentIndex(), directory);
    }
    else
    {
        QString shortDir = info.fileName();
        if(shortDir.isEmpty())
            shortDir = "/";
        emit updateWindowTitle(shortDir);
        m_tabs->setTabText(m_tabs->currentIndex(), shortDir);
    }
    m_tabs->setTabIcon(m_tabs->currentIndex(), ico);
    QUuid uuid = m_tabs->tabData(m_tabs->currentIndex()).toUuid();
    m_tabLocations[uuid] = QUrl::fromLocalFile(info.canonicalFilePath());
    emit updateWindowIcon(ico);
    QUrl url = QUrl::fromLocalFile(directory);
    m_location->setPath(url);

    updatePlaceModelSelection(url);
    // update our up (enclosing folder) action
    if(idx.parent() != QModelIndex())
        m_actions->shellAction(ArionShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(true);
    else
        m_actions->shellAction(ArionShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(false);

    emit updateStatusBar(generateStatusBarMsg());
}

QAction* LSEmbeddedShellHost::shellAction(ArionShell::ShellActions shellAction)
{
    return m_actions->shellAction(shellAction);
}

void LSEmbeddedShellHost::goBack()
{
    qDebug("Unimplemented Action: GoBack");
}

void LSEmbeddedShellHost::goForward()
{
    qDebug("Unimplemented Action: GoForward");
}

void LSEmbeddedShellHost::goEnclosingFolder()
{
    QModelIndex currentIndex;
    switch(m_viewMode)
    {
    case ArionShell::VIEW_COLUMN:
        currentIndex = m_filesColumn->rootIndex();
        break;
    case ArionShell::VIEW_LIST:
        currentIndex = m_filesTable->rootIndex();
        break;
    case ArionShell::VIEW_ICONS:
    default:
        currentIndex = m_filesList->rootIndex();
    }

    updateRootIndex(currentIndex.parent());
}


