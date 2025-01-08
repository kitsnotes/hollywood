// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "shellhost.h"
#include "shellhost_p.h"
#include "viewoptionsdialog.h"
#include "actionmanager.h"
#include "locationbar.h"
#include "filesystemmodel.h"
#include "placemodel.h"
#include "placeview.h"
#include "columnpreview.h"
#include "fsitemdelegate.h"
#include "executor.h"
#include "appmodel.h"
#include "getinfodialog.h"
#include "trashmodel.h"
#include "mimeapps.h"
#include "fileinfo.h"
#include "desktopentry.h"
#include "commonfunctions.h"
#include "opmanager.h"
#include "trashmodel.h"
#include "sectionwidget.h"
#include "lsdiskmodel.h"
#include "disks.h"

#include <QUndoStack>
#include <QDBusInterface>
#include <hollywood/hollywood.h>
#include <QAbstractItemModel>
#include <QMimeDatabase>
#include <QClipboard>
#include <QMimeData>
#include <QGuiApplication>

LSEmbeddedShellHostPrivate::LSEmbeddedShellHostPrivate(LSEmbeddedShellHost *parent, OperationManager *ops)
    : d(parent)
    , m_actions(new LSActionManager(parent))
    , m_favoriteSection(new LSSectionWidget(QApplication::tr("Favorites"), 100, parent, false))
    , m_deviceSection(new LSSectionWidget(QApplication::tr("Devices")))
    , m_driveModel(new LSDiskModel(parent))
    , m_directorySection(new LSSectionWidget(QApplication::tr("Filesystem"), 100, parent))
    , m_filesList(new QListView(parent))
    , m_model(new LSFSModel(parent))
    , m_apps(new ApplicationModel(parent))
    , m_trash(new LSTrashModel(parent))
    , m_placeModel(new LSPlaceModel(parent))
    , m_sidebarModel(new LSFSModel(parent))
    , m_viewOptions(new LSViewOptionsDialog(parent))
    , m_delegate(new LSFSItemDelegate(parent))
    , m_mimeapps(new LSMimeApplications(parent))
{
    m_model->setReadOnly(false);
}

LSEmbeddedShellHost::LSEmbeddedShellHost(QWidget *parent)
    : QWidget(parent)
    , p(new LSEmbeddedShellHostPrivate(this))
{

    p->m_mimeapps->processGlobalMimeCache();
    p->m_mimeapps->cacheAllDesktops();
    setContentsMargins(0,0,0,0);
    QSettings settings("originull", "hollywood");
    p->m_model->setRootPath("/");
    QModelIndex idx = p->m_model->index("/");
    if(idx.isValid())
        p->m_model->fetchMore(idx);

    setupLocationBar();

    /* Setup the main window splitter */
    p->m_mainSplitter = new QSplitter(this);
    p->m_mainSplitter->setOrientation(Qt::Horizontal);
    p->m_mainSplitter->setStretchFactor(1, 15);

    /* Setup the places and directory trees (left side of p->m_mainSplitter) */
    auto sidebarwidget = new QWidget(this);
    auto treelayout = new QVBoxLayout(sidebarwidget);
    sidebarwidget->setLayout(treelayout);
    treelayout->setSpacing(3);
    treelayout->setContentsMargins(0,10,0,0);
    p->m_treeFavorites = new LSPlaceView(p->m_favoriteSection);
    p->m_treeFavorites->setObjectName(QString::fromUtf8("PlacesTree"));
    p->m_treeFavorites->setModel(p->m_placeModel);
    p->m_treeFavorites->setDropIndicatorShown(true);
    p->m_treeFavorites->setDragDropMode(QAbstractItemView::DropOnly);
    connect(p->m_treeFavorites, &QTreeView::clicked, this, &LSEmbeddedShellHost::placeClicked);

    auto *favlayout = new QVBoxLayout;
    favlayout->setContentsMargins(2,1,0,0);
    favlayout->setSpacing(0);
    favlayout->setSizeConstraint(QVBoxLayout::SetFixedSize);
    favlayout->addWidget(p->m_treeFavorites);
    p->m_favoriteSection->setContentLayout(*favlayout);
    treelayout->addWidget(p->m_favoriteSection);
    p->m_favoriteSection->toggleWithoutAnimation(true);

    p->m_treeDevices = new LSPlaceView(this);
    p->m_treeDevices->setModel(p->m_driveModel);
    connect(p->m_treeDevices, &QListView::clicked, this, [this](const QModelIndex &idx) {
        if(QApplication::style()->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick) == 0)
            deviceActivated(idx);
    });
    connect(p->m_treeDevices, &QListView::activated,
            this, &LSEmbeddedShellHost::deviceActivated);

    auto *devlayout = new QVBoxLayout;
    devlayout->setContentsMargins(2,1,0,0);
    devlayout->setSpacing(0);
    devlayout->addWidget(p->m_treeDevices);
    p->m_deviceSection->setContentLayout(*devlayout);
    treelayout->addWidget(p->m_deviceSection);
    p->m_deviceSection->toggleWithoutAnimation(true);

    p->m_treeDirs = new LSPlaceView(this);
    p->m_treeDirs->setObjectName(QString::fromUtf8("DirectoryTree"));
    p->m_sidebarModel->setFilter(QDir::Dirs);
    p->m_treeDirs->setModel(p->m_sidebarModel);

    auto *dirlayout = new QVBoxLayout;
    dirlayout->setContentsMargins(2,1,0,0);
    dirlayout->setSpacing(0);
    dirlayout->addWidget(p->m_treeDirs);
    p->m_directorySection->setContentLayout(*dirlayout);
    treelayout->addWidget(p->m_directorySection);
    sidebarwidget->setMaximumWidth(155);
    sidebarwidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    treelayout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));

    p->m_treeDirs->setVisible(false);
    p->m_directorySection->setVisible(false);
    /* setup tab bar host (right side of p->m_mainSplitter) */
    p->m_tabWndHost = new QWidget(p->m_mainSplitter);
    p->m_tabWndHostLayout = new QVBoxLayout(p->m_tabWndHost);
    p->m_tabWndHostLayout->setSpacing(0);
    p->m_tabWndHostLayout->setContentsMargins(0,0,0,0);
    p->m_tabs = new QTabBar(p->m_tabWndHost);
    p->m_tabs->setTabsClosable(true);
    p->m_tabs->setMovable(true);
    p->m_tabs->setAutoHide(settings.value("Preferences/AutoHideTabs", true).toBool());
    connect(p->m_tabs, SIGNAL(currentChanged(int)),
            this, SLOT(currentTabChanged(int)));
    connect(p->m_tabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(tabCloseRequested(int)));

    p->m_tabWndHostLayout->addWidget(p->m_tabs);

    /* Setup our main right-side views */

    p->m_filesColumn = new QColumnView(this);
    p->m_filesColumn->setObjectName(QString::fromUtf8("FilesColumn"));
    p->m_filesColumn->setAutoScroll(true);

    p->m_columnPreview = new LSColumnPreview(this);
    p->m_filesColumn->setPreviewWidget(p->m_columnPreview);
    p->m_filesColumn->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Setup Icon View */
    p->m_filesList->setObjectName(QString::fromUtf8("FilesIcon"));
    p->m_filesList->setIconSize(QSize(32,32));
    p->m_filesList->setSpacing(6);
    p->m_filesList->setViewMode(QListView::IconMode);
    p->m_filesList->setFlow(QListView::LeftToRight);
    p->m_filesList->setTextElideMode(Qt::ElideMiddle);
    p->m_filesList->setWrapping(true);
    p->m_filesList->setWordWrap(true);
    p->m_filesList->setUniformItemSizes(false);
    p->m_filesList->setItemDelegate(p->m_delegate);
    p->m_filesList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    p->m_filesList->setContextMenuPolicy(Qt::CustomContextMenu);

    /* Setup List View */
    p->m_filesTable = new QTreeView(this);
    p->m_filesTable->setObjectName(QString::fromUtf8("FilesList"));
    p->m_filesTable->setAlternatingRowColors(true);
    p->m_filesTable->setAnimated(true);
    p->m_filesTable->setExpandsOnDoubleClick(false);
    p->m_filesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    p->m_filesTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    p->m_filesTable->setSortingEnabled(true);
    p->m_filesTable->header()->setFirstSectionMovable(false);
    p->m_filesTable->header()->resizeSection(0,290);
    /* Only the icons is visible here - window setup
     * procedure will set the default where required */
    p->m_filesColumn->setVisible(false);
    p->m_filesTable->setVisible(false);
    p->m_tabWndHostLayout->addWidget(p->m_filesList);
    p->m_mainSplitter->addWidget(sidebarwidget);
    p->m_mainSplitter->addWidget(p->m_tabWndHost);
    p->m_viewOptions->attachIconView(p->m_filesList);

    // Enable Drag & Drop (TODO: common dialog)
    p->m_filesColumn->setDragEnabled(true);
    p->m_filesTable->setDragEnabled(true);
    p->m_filesList->setDragEnabled(true);

    p->m_filesList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    p->m_filesTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    p->m_filesColumn->setSelectionMode(QAbstractItemView::ExtendedSelection);

    swapToModel(Filesystem);
    connect(p->m_model, &LSFSModel::sortingChanged,
            this, &LSEmbeddedShellHost::filesystemSortingChanged);
    filesystemSortingChanged();

    connect(p->m_filesColumn, &QColumnView::updatePreviewWidget,
            this, &LSEmbeddedShellHost::updateColumnWidget);

    connect(p->m_model, &LSFSModel::rootPathChanged,
            this, &LSEmbeddedShellHost::modelRootPathChanged);

    connect(p->m_model, &LSFSModel::sortingChanged,
            this, &LSEmbeddedShellHost::sortOrderChanged);

    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_OPTIONS),
            &QAction::triggered, this, &LSEmbeddedShellHost::toggleViewOptions);
    connect(p->m_viewOptions, &LSViewOptionsDialog::finished,
            this, &LSEmbeddedShellHost::viewOptionsFinished);

    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_ICONS),
            &QAction::triggered, this, &LSEmbeddedShellHost::setIconListView);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_LIST),
            &QAction::triggered, this, &LSEmbeddedShellHost::setTableListView);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_COLUMNS),
            &QAction::triggered, this, &LSEmbeddedShellHost::setColumnView);

    connect(p->m_actions->shellAction(HWShell::ACT_FILE_GET_INFO),
            &QAction::triggered, this, &LSEmbeddedShellHost::getInformationRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FOLDER), &QAction::triggered,
            this, &LSEmbeddedShellHost::newFolder);
    connect(p->m_actions->shellAction(HWShell::ACT_FILE_OPEN), &QAction::triggered,
            this, &LSEmbeddedShellHost::openSelection);
    connect(p->m_actions->shellAction(HWShell::ACT_FILE_ARCHIVE), &QAction::triggered,
            this, &LSEmbeddedShellHost::archiveSelection);
    connect(p->m_actions->shellAction(HWShell::ACT_FILE_TRASH), &QAction::triggered,
            this, &LSEmbeddedShellHost::trash);
    connect(p->m_actions->shellAction(HWShell::ACT_FILE_RENAME), &QAction::triggered,
            this, &LSEmbeddedShellHost::rename);

    connect(p->m_actions->shellAction(HWShell::ACT_FILE_NEW_TAB), &QAction::triggered,
             this, &LSEmbeddedShellHost::createNewTab);
    connect(p->m_actions->shellAction(HWShell::ACT_GO_BACK), &QAction::triggered,
            this, &LSEmbeddedShellHost::goBack);
    connect(p->m_actions->shellAction(HWShell::ACT_GO_FORWARD), &QAction::triggered,
            this, &LSEmbeddedShellHost::goForward);

    connect(p->m_actions->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER), &QAction::triggered,
            this, &LSEmbeddedShellHost::goEnclosingFolder);

    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NONE),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NAME),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_SIZE),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_KIND),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_MODIFIED),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_OWNER),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_GROUP),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_COMMENT),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortRequested);

    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_ASC),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortOrderRequested);
    connect(p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_DESC),
            &QAction::triggered, this, &LSEmbeddedShellHost::actionSortOrderRequested);

    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_UNDO), &QAction::triggered,
            LSCommonFunctions::instance()->undoStack(), &QUndoStack::undo);
    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_REDO), &QAction::triggered,
            LSCommonFunctions::instance()->undoStack(), &QUndoStack::redo);

    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_COPY), &QAction::triggered,
            this, &LSEmbeddedShellHost::copyItems);
    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE), &QAction::triggered,
            this, &LSEmbeddedShellHost::paste);

    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_SEL_ALL), &QAction::triggered,
            this, &LSEmbeddedShellHost::selectAll);
    connect(p->m_actions->shellAction(HWShell::ACT_EDIT_INV_SEL), &QAction::triggered,
            this, &LSEmbeddedShellHost::invertSelection);

    connect(LSCommonFunctions::instance()->undoStack(), &QUndoStack::canUndoChanged,
            this, &LSEmbeddedShellHost::canUndoChanged);
    connect(LSCommonFunctions::instance()->undoStack(), &QUndoStack::undoTextChanged,
            this, &LSEmbeddedShellHost::canUndoChanged);
    connect(LSCommonFunctions::instance()->undoStack(), &QUndoStack::canRedoChanged,
            this, &LSEmbeddedShellHost::canRedoChanged);
    connect(LSCommonFunctions::instance()->undoStack(), &QUndoStack::redoTextChanged,
            this, &LSEmbeddedShellHost::canRedoChanged);

    connect(LSCommonFunctions::instance(), &LSCommonFunctions::pasteAvailable,
            this, &LSEmbeddedShellHost::enablePaste);

    restoreViewModeFromSettings();
}

void LSEmbeddedShellHost::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    p->m_mainSplitter->resize(event->size());
}

void LSEmbeddedShellHost::getInformationRequested()
{
    if(p->m_currentModel == Filesystem)
    {
        QItemSelectionModel *model = nullptr;
        switch(p->m_viewMode)
        {
        case HWShell::VIEW_ICONS:
            model = p->m_filesList->selectionModel();
            break;
        case HWShell::VIEW_LIST:
            model = p->m_filesTable->selectionModel();
            break;
        case HWShell::VIEW_COLUMN:
            model = p->m_filesColumn->selectionModel();
        }

        if(model)
        {
            if(model->selectedIndexes().count() == 1)
            {
                // show a dialog for our one file
                auto url = QUrl(p->m_model->filePath(model->selectedIndexes().first()));
                LSCommonFunctions::instance()->showGetInfoDialog(url, this);
            }
        }
    }
}

void LSEmbeddedShellHost::locationBarEditingFinished()
{

}

void LSEmbeddedShellHost::setupLocationBar()
{
    // we don't always use this so we provision it
    // only as needed to save on resources
    if(p->m_location)
        return;

    p->m_location = new LSLocationBar(this);
    connect(p->m_location, &LSLocationBar::chdir, this, &LSEmbeddedShellHost::navigateToUrl);
    connect(p->m_location, &LSLocationBar::middleClickChdir, this, &LSEmbeddedShellHost::newTabWithPath);
    connect(p->m_location, &LSLocationBar::editingFinished, this, &LSEmbeddedShellHost::locationBarEditingFinished);
}

LSLocationBar* LSEmbeddedShellHost::locationBar()
{
    return p->m_location;
}

void LSEmbeddedShellHost::restoreViewModeFromSettings()
{
    QSettings settings("originull", "hollywood");

    // restore our view mode from settings
    p->m_viewMode = (HWShell::ViewMode)settings.value("Preferences/DefaultView", HWShell::VIEW_ICONS).toInt();
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_COLUMN:
        setColumnView();
        break;
    case HWShell::VIEW_LIST:
        setTableListView();
        break;
    case HWShell::VIEW_ICONS:
    default:
        setIconListView();
    }

    p->m_sortColumn = settings.value("Preferences/DefaultSortColumn", 0).toUInt();
    if(p->m_sortColumn > LSFSModel::Comment)
        p->m_sortColumn = 0;

    auto sortOrder = settings.value("Preferences/DefaultSortOrder", 0).toUInt();
    if(sortOrder > 1)
        sortOrder = 0;
    p->m_sortOrder = (Qt::SortOrder)sortOrder;
    doSort(p->m_sortColumn, p->m_sortOrder);
}

void LSEmbeddedShellHost::toggleViewOptions(bool checked)
{
    if(checked)
        p->m_viewOptions->show();
    else
        p->m_viewOptions->hide();
}

void LSEmbeddedShellHost::doSort(int column, Qt::SortOrder order)
{
    if(p->m_currentModel == Filesystem)
    {
        auto uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
        p->m_model->sort(column, order);
        p->m_sortOrder = order;
        p->m_sortColumn = column;
        p->m_tabSortOrder[uuid] = p->m_sortOrder;
        p->m_tabSortCol[uuid] = p->m_sortColumn;

        if(p->m_viewMode == HWShell::VIEW_LIST)
            p->m_filesTable->header()->setSortIndicator(column, order);
        emit sortOrderChanged();
    }
}

void LSEmbeddedShellHost::
    viewOptionsFinished(int result)
{
    Q_UNUSED(result);
    if(!p->m_viewOptions->isVisible())
        p->m_actions->shellAction(HWShell::ACT_VIEW_OPTIONS)->setChecked(false);
}

void LSEmbeddedShellHost::tabCloseRequested(int index)
{
    QUuid uuid = p->m_tabs->tabData(index).toUuid();
    p->m_tabLocations.remove(uuid);
    p->m_tabViewMode.remove(uuid);
    p->m_tabSortOrder.remove(uuid);
    p->m_tabSortCol.remove(uuid);
    p->m_backLists.remove(uuid);
    p->m_forwardLists.remove(uuid);
    p->m_tabs->removeTab(index);
    if(p->m_tabs->count() == 0)
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
    if(path.isEmpty())
        return false;

    if(!path.isValid())
        return false;

    swapModelForUrl(path);

    if(path.scheme() == "trash")
    {
        QIcon ico = QIcon::fromTheme("folder-activities");
        emit updateWindowTitle(tr("Trash"));
        p->m_tabs->setTabText(p->m_tabs->currentIndex(), tr("Trash"));
        p->m_tabs->setTabIcon(p->m_tabs->currentIndex(), ico);
        QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();

        p->m_tabLocations[uuid] = path;
        emit updateWindowIcon(ico);
        p->m_location->setPath(path);

        updatePlaceModelSelection(path);
        disableActionsForNoSelection();
        p->m_actions->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(false);
        return true;
    }

    if(path.scheme() == "applications")
    {
        QIcon ico = QIcon::fromTheme("folder-activities");
        emit updateWindowTitle(tr("Applications"));
        p->m_tabs->setTabText(p->m_tabs->currentIndex(), tr("Applications"));
        p->m_tabs->setTabIcon(p->m_tabs->currentIndex(), ico);
        QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();

        p->m_tabLocations[uuid] = path;
        emit updateWindowIcon(ico);
        p->m_location->setPath(path);

        updatePlaceModelSelection(path);
        disableActionsForNoSelection();
        p->m_actions->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(false);
        return true;
    }

    if(path.isLocalFile())
    {
        QString file = path.toLocalFile();
        QModelIndex idx = p->m_model->index(file);
        if(idx.isValid())
        {
            // TODO: back list append
            updateRootIndex(idx);
            disableActionsForNoSelection();
            return true;
        }
    }

    return false;
}


void LSEmbeddedShellHost::newTabWithPath(const QUrl &path)
{
    if(path.scheme() == "file")
    {
     const QString localPath = path.toLocalFile();
     QDir dir(localPath);
     if(dir.isReadable())
     {
         int idx = p->m_tabs->addTab(localPath);
         QUuid tabId = QUuid::createUuid();
         p->m_tabs->setTabData(idx, tabId);
         // just temporary to add the map entry
         p->m_tabLocations.insert(tabId, QUrl::fromLocalFile("/"));
         p->m_tabViewMode.insert(tabId, p->m_viewMode);
         p->m_tabSortOrder.insert(tabId, p->m_sortOrder);
         p->m_tabSortCol.insert(tabId, p->m_sortColumn);

         p->m_backLists.insert(tabId, UrlList());
         p->m_forwardLists.insert(tabId, UrlList());

         p->m_tabs->setCurrentIndex(idx);
         navigateToPath(localPath);
     }
     // TODO: error out here
    }
    if(path.scheme() == "trash")
    {
        QUrl url = QUrl("trash://");
        int idx = p->m_tabs->addTab(QApplication::tr("Trash"));
        QUuid tabId = QUuid::createUuid();
        p->m_tabs->setTabData(idx, tabId);
        // just temporary to add the map entry
        p->m_tabLocations.insert(tabId, url);
        p->m_tabViewMode.insert(tabId, p->m_viewMode);
        p->m_tabSortOrder.insert(tabId, p->m_sortOrder);
        p->m_tabSortCol.insert(tabId, p->m_sortColumn);

        p->m_backLists.insert(tabId, UrlList());
        p->m_forwardLists.insert(tabId, UrlList());
        p->m_tabs->setCurrentIndex(idx);
        navigateToUrl(url);
    }
    if(path.scheme() == "applications")
    {
        QUrl url = QUrl("applications://");
        int idx = p->m_tabs->addTab(QApplication::tr("Applications"));
        QUuid tabId = QUuid::createUuid();
        p->m_tabs->setTabData(idx, tabId);
        // just temporary to add the map entry
        p->m_tabLocations.insert(tabId, url);
        p->m_tabViewMode.insert(tabId, p->m_viewMode);
        p->m_tabSortOrder.insert(tabId, p->m_sortOrder);
        p->m_tabSortCol.insert(tabId, p->m_sortColumn);

        p->m_backLists.insert(tabId, UrlList());
        p->m_forwardLists.insert(tabId, UrlList());
        p->m_tabs->setCurrentIndex(idx);
        navigateToUrl(url);
    }
}

void LSEmbeddedShellHost::currentTabChanged(int index)
 {
    QUuid uuid = p->m_tabs->tabData(index).toUuid();
    QUrl path = p->m_tabLocations[uuid];
    swapModelForUrl(path);
    HWShell::ViewMode mode = p->m_tabViewMode[uuid];
    p->m_sortColumn = p->m_tabSortCol[uuid];
    p->m_sortOrder = p->m_tabSortOrder[uuid];
    switch(mode)
    {
    case HWShell::VIEW_COLUMN:
        setColumnView(false);
        break;
    case HWShell::VIEW_LIST:
        setTableListView(false);
        break;
    case HWShell::VIEW_ICONS:
    default:
        setIconListView(false);
    }
    navigateToUrl(path);
    updateNavigationButtonStatus();
    doSort(p->m_sortColumn, p->m_sortOrder);
    emit updateStatusBar(generateStatusBarMsg());
 }


void LSEmbeddedShellHost::setIconListView(bool updateSettings)
{
    if(p->m_curSelModel != nullptr)
        p->m_curSelModel->clear();

    p->m_curSelModel = nullptr;
    disconnectViewSlots();

    p->m_tabWndHostLayout->replaceWidget(p->m_tabWndHostLayout->itemAt(1)->widget(), p->m_filesList);
    p->m_filesColumn->setVisible(false);
    p->m_filesList->setVisible(true);
    p->m_filesTable->setVisible(false);
    p->m_actions->shellAction(HWShell::ACT_VIEW_ICONS)->setChecked(true);
    p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NONE)->setEnabled(true);
    connect(p->m_filesList, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);
    connect(p->m_filesList, &QListView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    connect(p->m_filesList, &QListView::activated, this, &LSEmbeddedShellHost::viewActivated);
    connect(p->m_filesList->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    p->m_viewMode = HWShell::VIEW_ICONS;
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
    p->m_tabViewMode[uuid] = p->m_viewMode;
    p->m_viewOptions->setCurrentView(HWShell::VIEW_ICONS);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", p->m_viewMode);
    }
    disableActionsForNoSelection();
    p->m_curSelModel = p->m_filesList->selectionModel();
    emit viewModeChanged();
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::setColumnView(bool updateSettings)
{
    if(p->m_curSelModel != nullptr)
        p->m_curSelModel->clear();

    p->m_curSelModel = nullptr;

    disconnectViewSlots();
    p->m_tabWndHostLayout->replaceWidget(p->m_tabWndHostLayout->itemAt(1)->widget(), p->m_filesColumn);
    p->m_filesColumn->setVisible(true);
    p->m_filesList->setVisible(false);
    p->m_filesTable->setVisible(false);
    p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NONE)->setEnabled(false);
    p->m_actions->shellAction(HWShell::ACT_VIEW_COLUMNS)->setChecked(true);
    connect(p->m_filesColumn, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);
    connect(p->m_filesColumn, &QColumnView::activated, this, &LSEmbeddedShellHost::viewActivated);
    connect(p->m_filesColumn->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    p->m_viewMode = HWShell::VIEW_COLUMN;
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
    p->m_tabViewMode[uuid] = p->m_viewMode;
    p->m_viewOptions->setCurrentView(HWShell::VIEW_COLUMN);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", p->m_viewMode);
    }
    p->m_curSelModel = p->m_filesColumn->selectionModel();
    disableActionsForNoSelection();
    emit viewModeChanged();
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::setTableListView(bool updateSettings)
{
    if(p->m_curSelModel != nullptr)
        p->m_curSelModel->clear();

    disconnectViewSlots();
    p->m_curSelModel = nullptr;
    p->m_tabWndHostLayout->replaceWidget(p->m_tabWndHostLayout->itemAt(1)->widget(), p->m_filesTable);
    p->m_filesColumn->setVisible(false);
    p->m_filesList->setVisible(false);
    adjustColumnHeaders();
    p->m_filesTable->setVisible(true);
    p->m_actions->shellAction(HWShell::ACT_VIEW_LIST)->setChecked(true);
    p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NONE)->setEnabled(false);
    p->m_viewMode = HWShell::VIEW_LIST;
    connect(p->m_filesTable, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);
    connect(p->m_filesTable, &QTreeView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    connect(p->m_filesTable, &QTreeView::activated, this, &LSEmbeddedShellHost::viewActivated);
    connect(p->m_filesTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
    p->m_tabViewMode[uuid] = p->m_viewMode;
    p->m_viewOptions->setCurrentView(HWShell::VIEW_LIST);
    if(updateSettings)
    {
        QSettings settings("originull", "hollywood");
        settings.setValue("Preferences/DefaultView", p->m_viewMode);
    }
    p->m_curSelModel = p->m_filesTable->selectionModel();
    disableActionsForNoSelection();
    emit viewModeChanged();
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::viewClicked(const QModelIndex &idx)
{
    Q_UNUSED(idx);
    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::viewActivated(const QModelIndex &idx)
{
    openItem(idx);
}

void LSEmbeddedShellHost::viewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    auto idxes = p->m_curSelModel->selectedIndexes();
    if(idxes.count() > 2)
        enableActionsForFileSelection(true);
    else if(idxes.count() == 1)
        enableActionsForFileSelection(false);
    else
        disableActionsForNoSelection();

    emit updateStatusBar(generateStatusBarMsg());
}

void LSEmbeddedShellHost::placeClicked(const QModelIndex &idx)
{
    p->m_treeDirs->clearSelection();
    navigateToUrl(p->m_placeModel->place(idx).location);
}

void LSEmbeddedShellHost::deviceActivated(const QModelIndex &idx)
{
    auto device = p->m_driveModel->device(idx);
    if(device)
    {
        if(!device->mountpoint().isEmpty())
        {
            // this should be somewhere we can go...
            navigateToPath(device->mountpoint());
            return;
        }
        else
        {
            // this is a device that is unmounted...
            // mount it up:
            device->mount();
            // TODO: check for mount, and open it:
        }
    }
}

void LSEmbeddedShellHost::updateColumnWidget(const QModelIndex &idx)
{
    p->m_columnPreview->updateFileInfo(p->m_model->fileInfo(idx));
    p->m_columnPreview->show();
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

void LSEmbeddedShellHost::viewContextMenuRequested(const QPoint &pos)
{
    qDebug() << "viewContextMenuRequested" << pos;
    auto menu = new QMenu(this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    QModelIndex pointIndex;
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_ICONS:
        pointIndex = p->m_filesList->indexAt(pos);
        break;
    case HWShell::VIEW_LIST:
        pointIndex = p->m_filesTable->indexAt(pos);
        break;
    case HWShell::VIEW_COLUMN:
        pointIndex = p->m_filesColumn->indexAt(pos);
        break;
    }

    bool onSel = false;
    if(pointIndex.isValid())
    {
        // we rightclick on a valid index

        if(p->m_curSelModel != nullptr)
        {
            if(p->m_curSelModel->selectedIndexes().contains(pointIndex))
                onSel = true;
            else
            {
                // vald index, not part of selection though
                // lets select it
                p->m_curSelModel->clearSelection();
                p->m_curSelModel->select(pointIndex, QItemSelectionModel::ClearAndSelect);
                onSel = true;
            }
        }
    }

    if(onSel)
    {
        // create our menu for a selection
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_OPEN));
        menu->addMenu(p->m_actions->openWithMenu());
        menu->addSeparator();
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_ARCHIVE));
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_RENAME));
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_EDIT_COPY));
        menu->addSeparator();
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_TRASH));
        menu->addSeparator();
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_GET_INFO));
    }
    else
    {
        // no selection - create menu for this folder
        menu->addMenu(p->m_actions->newMenu());
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE));
        menu->addSeparator();
        menu->addAction(p->m_actions->shellAction(HWShell::ACT_FILE_GET_INFO));
    }

    qDebug() << pos << "mapToParent" << mapToParent(pos);
    menu->popup(mapToParent(pos));
}

void LSEmbeddedShellHost::filesystemSortingChanged()
{
    if(p->m_currentModel == Filesystem)
    {
        switch(p->m_model->sortColumn())
        {
        case LSFSModel::Comment:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_COMMENT)->setChecked(true);
            break;
        case LSFSModel::Group:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_GROUP)->setChecked(true);
            break;
        case LSFSModel::Owner:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_OWNER)->setChecked(true);
            break;
        case LSFSModel::ModificationDate:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_MODIFIED)->setChecked(true);
            break;
        case LSFSModel::Kind:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_KIND)->setChecked(true);
            break;
        case LSFSModel::Size:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_SIZE)->setChecked(true);
            break;
        case LSFSModel::Name:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NAME)->setChecked(true);
            break;
        case -1:
        default:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NONE)->setChecked(true);
            break;
        }

        switch(p->m_model->sortOrder())
        {
        case Qt::AscendingOrder:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_DESC)->setChecked(false);
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_ASC)->setChecked(true);
            break;
        case Qt::DescendingOrder:
        default:
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_ASC)->setChecked(false);
            p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_DESC)->setChecked(true);
            break;
        }
        emit sortOrderChanged();
    }

}

void LSEmbeddedShellHost::copyItems()
{
    if(p->m_currentModel == Filesystem)
    {
        QItemSelectionModel *model = nullptr;
        switch(p->m_viewMode)
        {
        case HWShell::VIEW_ICONS:
            model = p->m_filesList->selectionModel();
            break;
        case HWShell::VIEW_LIST:
            model = p->m_filesTable->selectionModel();
            break;
        case HWShell::VIEW_COLUMN:
            model = p->m_filesColumn->selectionModel();
        }

        if(model)
        {
            QList<QUrl> urls;
            for(auto idx : model->selectedIndexes())
                urls.append(QUrl::fromLocalFile(p->m_model->fileInfo(idx).absoluteFilePath()));

            auto mimedata = new QMimeData;
            mimedata->setUrls(urls);
            QGuiApplication::clipboard()->setMimeData(mimedata);
        }
    }
}

void LSEmbeddedShellHost::enablePaste(bool enable, uint count)
{
    p->m_clipboardHasPaste = enable;
    if(!enable)
    {
        shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste"));
        shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(enable);
    }
    else
    {
        if(count == 1)
            shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste %1 Item").arg(QString::number(count)));
        else
            shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste %1 Items").arg(QString::number(count)));
        shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(enable);
    }
    if(p->m_currentModel == Trash ||
        p->m_currentModel == Applications)
        shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(false);
}

void LSEmbeddedShellHost::paste()
{
    if(p->m_currentModel == Filesystem)
    {
        auto curdir = QString();
        switch(p->m_viewMode)
        {
        case HWShell::VIEW_ICONS:
            curdir = p->m_model->absolutePath(p->m_filesList->rootIndex());
            break;
        case HWShell::VIEW_LIST:
            curdir = p->m_model->absolutePath(p->m_filesTable->rootIndex());
            break;
        case HWShell::VIEW_COLUMN:
            curdir = p->m_model->absolutePath(p->m_filesColumn->rootIndex());
            break;
        default:
            break;
        }
        auto data = QGuiApplication::clipboard()->mimeData()->urls();
        LSCommonFunctions::instance()->operationManager()->copyFiles(data, QUrl::fromLocalFile(curdir));
    }
}

void LSEmbeddedShellHost::canUndoChanged()
{
    bool canundo = LSCommonFunctions::instance()->undoStack()->canUndo();
    if(canundo)
    {
        shellAction(HWShell::ACT_EDIT_UNDO)->setEnabled(true);
        shellAction(HWShell::ACT_EDIT_UNDO)->setText(tr("&Undo %1")
            .arg(LSCommonFunctions::instance()->undoStack()->undoText()));
    }
    else
    {
        shellAction(HWShell::ACT_EDIT_UNDO)->setEnabled(false);
        shellAction(HWShell::ACT_EDIT_UNDO)->setText(tr("&Undo"));
    }
}

void LSEmbeddedShellHost::canRedoChanged()
{
    bool canredo = LSCommonFunctions::instance()->undoStack()->canRedo();
    if(canredo)
    {
        shellAction(HWShell::ACT_EDIT_REDO)->setEnabled(true);
        shellAction(HWShell::ACT_EDIT_REDO)->setText(tr("&Redo %1")
            .arg(LSCommonFunctions::instance()->undoStack()->redoText()));
    }
    else
    {
        shellAction(HWShell::ACT_EDIT_REDO)->setEnabled(false);
        shellAction(HWShell::ACT_EDIT_REDO)->setText(tr("&Redo"));
    }
}

void LSEmbeddedShellHost::selectAll()
{
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_ICONS:
        p->m_filesList->selectAll();
        break;
    case HWShell::VIEW_LIST:
        p->m_filesTable->selectAll();
        break;
    case HWShell::VIEW_COLUMN:
        p->m_filesColumn->selectAll();
        break;
    default:
        break;
    }
}

void LSEmbeddedShellHost::invertSelection()
{

}

void LSEmbeddedShellHost::newFolder()
{
    if(p->m_currentModel == Filesystem)
    {
        auto folder = p->m_model->absolutePath(p->m_currentFsRoot);
        auto newpath = LSCommonFunctions::instance()->newFolder(QUrl::fromLocalFile(folder), this);
    }
}

void LSEmbeddedShellHost::openSelection()
{
    if(p->m_curSelModel->selectedIndexes().count() == 1)
        openItem(p->m_curSelModel->selectedIndexes().first());
    else
    {
        // TODO: multiple open
    }
}

void LSEmbeddedShellHost::rename()
{
    if(p->m_curSelModel->selectedIndexes().count() == 1)
    {
        switch(p->m_viewMode)
        {
        case HWShell::VIEW_ICONS:
            p->m_filesList->edit(p->m_curSelModel->selectedIndexes().first());
            break;
        case HWShell::VIEW_LIST:
            p->m_filesTable->edit(p->m_curSelModel->selectedIndexes().first());
            break;
        case HWShell::VIEW_COLUMN:
            p->m_filesColumn->edit(p->m_curSelModel->selectedIndexes().first());
            break;
        default:
            break;
        }
    }
    else
    {
        qDebug() << "multi-rename coming soon";
    }
}

void LSEmbeddedShellHost::trash()
{

}

void LSEmbeddedShellHost::archiveSelection()
{

}

void LSEmbeddedShellHost::adjustColumnHeaders()
{
    if(p->m_currentModel == Filesystem)
    {
        p->m_filesTable->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        p->m_filesTable->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        p->m_filesTable->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        p->m_filesTable->header()->setSectionResizeMode(3, QHeaderView::Interactive);
        p->m_filesTable->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        p->m_filesTable->header()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
        p->m_filesTable->header()->setSectionResizeMode(6, QHeaderView::Interactive);
    }
}

void LSEmbeddedShellHost::enableNoSelectionWritableActions()
{
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
    auto url = p->m_tabLocations[uuid];

    if(url.isEmpty())
        return;
\
    if(p->m_currentModel == Filesystem)
    {
        if(url.scheme() == "file")
        {
            auto file = url.toLocalFile();
            QFileInfo f(file);
            if(f.isWritable())
            {
                p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FOLDER)->setEnabled(true);
                p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FILE)->setEnabled(true);
            }
            else
            {
                p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FOLDER)->setDisabled(true);
                p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FILE)->setDisabled(true);

            }
        }
    }
}

void LSEmbeddedShellHost::actionSortRequested()
{
    auto act = qobject_cast<QAction*>(sender());
    int column = -1;

    if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_NAME))
        column = LSFSModel::Name;
    if(p->m_currentModel == Filesystem)
    {
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_SIZE))
            column = LSFSModel::Size;
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_KIND))
            column = LSFSModel::Kind;
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_MODIFIED))
            column = LSFSModel::ModificationDate;
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_OWNER))
            column = LSFSModel::Owner;
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_GROUP))
            column = LSFSModel::Group;
        if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_COMMENT))
            column = LSFSModel::Comment;

        if(column != -1)
            doSort(column, p->m_model->sortOrder());
    }
}

void LSEmbeddedShellHost::actionSortOrderRequested()
{
    auto act = qobject_cast<QAction*>(sender());
    Qt::SortOrder order = Qt::AscendingOrder;

    if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_ASC))
        order = Qt::AscendingOrder;
    if(act == p->m_actions->shellAction(HWShell::ACT_VIEW_SORT_DESC))
        order = Qt::DescendingOrder;
    if(p->m_currentModel == Filesystem)
        doSort(p->m_model->sortColumn(), order);

}

void LSEmbeddedShellHost::disableActionsForNoSelection()
{
    p->m_actions->openWithMenu()->setEnabled(false);
    p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setText(tr("&Rename"));
    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open"));
    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN_WITH)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
    p->m_actions->shellAction(HWShell::ACT_FILE_GET_INFO)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_FILE_ARCHIVE)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_FILE_TRASH)->setDisabled(true);

    p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setText(tr("C&opy"));
    p->m_actions->shellAction(HWShell::ACT_EDIT_CUT)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_EDIT_INV_SEL)->setDisabled(true);

    if(p->m_currentModel != Filesystem)
    {
        p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FOLDER)->setEnabled(false);
        p->m_actions->shellAction(HWShell::ACT_FILE_NEW_FILE)->setDisabled(true);
    }
    else
    {
        enableNoSelectionWritableActions();
    }
}

void LSEmbeddedShellHost::enableActionsForFileSelection(bool multiple)
{
    if(!multiple)
    {
        if(p->m_currentModel == Filesystem)
        {
            auto idx = p->m_curSelModel->selectedIndexes().first();
            p->m_model->fileInfo(idx);

            auto path = p->m_model->fileInfo(idx).absoluteFilePath();
            QMimeDatabase db;
            auto mime = db.mimeTypeForFile(path);
            if(mime.name() != "inode/directory")
            {
                if(p->m_model->isDesktop(idx))
                {
                    auto *desktop = new LSDesktopEntry();
                    QString di;
                    if(desktop->load(p->m_model->desktopFileForIndex(idx)))
                        di = QString(desktop->value("Icon").toString());

                    if(!di.isEmpty())
                    {
                        QIcon dico = QIcon::fromTheme(di);
                        if(!dico.isNull())
                            p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(dico);
                    }
                    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(QString("%1 %2")
                                                                     .arg(tr("&Open")).arg(desktop->value("Name").toString()));
                    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
                }
                else
                {
                    p->m_actions->openWithMenu()->setEnabled(true);
                    p->m_actions->shellAction(HWShell::ACT_FILE_OPEN_WITH)->setEnabled(true);
                    auto app = p->m_mimeapps->defaultApp(mime.name());
                    if(app)
                    {
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open with %1").arg(app->value("Name").toString()));
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(app->icon());
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
                    }
                    else
                    {
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open"));
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
                        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
                        p->m_actions->openWithMenu()->setEnabled(true);
                    }
                }
            }
            else
            {
                // single option - directory
                p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open"));
                p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
                p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
                p->m_actions->openWithMenu()->setEnabled(false);
            }
            p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(true);
            p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(p->m_clipboardHasPaste);
            p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(true);
        }
        else
        {
            // single selection
            // not filesystem - ie: trash/applications/etc
            p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open"));
            p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
            p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
            p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(false);
            p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(false);
            p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(false);

        }
        p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setText(tr("&Rename"));
    }
    else
    {
        // multiple options
        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open..."));
        p->m_actions->shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
        p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setText(tr("&Rename..."));

        if(p->m_currentModel == Filesystem)
        {
            p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(true);
            p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(true);
            p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(p->m_clipboardHasPaste);
        }
        else
        {
            // trash/applications/etc
            p->m_actions->shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(false);
            p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(false);
            p->m_actions->shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(p->m_clipboardHasPaste);
        }
    }

    p->m_actions->shellAction(HWShell::ACT_FILE_TRASH)->setEnabled(true);

    p->m_actions->shellAction(HWShell::ACT_FILE_GET_INFO)->setEnabled(true);
    //p->m_actions->shellAction(HWShell::ACT_FILE_ARCHIVE)->setEnabled(true);
    if(p->m_curSelModel->selectedIndexes().count() == 1)
        p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setText(tr("C&opy 1 Item"));
    else
        p->m_actions->shellAction(HWShell::ACT_EDIT_COPY)->setText(tr("C&opy %1 Items")
                    .arg(QString::number(p->m_curSelModel->selectedIndexes().count())));

    p->m_actions->shellAction(HWShell::ACT_EDIT_CUT)->setDisabled(true);
    p->m_actions->shellAction(HWShell::ACT_EDIT_INV_SEL)->setDisabled(true);
}

void LSEmbeddedShellHost::updatePlaceModelSelection(const QUrl &place)
{
    p->m_treeFavorites->selectionModel()->clear();
    p->m_treeDevices->selectionModel()->clear();
    if(p->m_placeModel->hasPlaceForUrl(place))
    {
        const QModelIndex idx = p->m_placeModel->indexForPlace(p->m_placeModel->place(place));
        p->m_treeFavorites->selectionModel()->select(idx, QItemSelectionModel::Select);
    }

    if(place.scheme() == "file")
    {
        auto dev = p->m_driveModel->deviceForMount(place.toLocalFile());
        if(dev != nullptr)
        {
            const QModelIndex idx = p->m_driveModel->indexOf(dev);
            p->m_treeDevices->selectionModel()->select(idx, QItemSelectionModel::Select);
        }
    }
}

QString LSEmbeddedShellHost::generateStatusBarMsg() const
{
    QString fileName, fileSize, fileType;
    QModelIndex current;
    bool selected = false;
    int count = 0;
    QLocale l;
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_ICONS:
        current = p->m_filesList->currentIndex();
        selected = p->m_filesList->selectionModel()->hasSelection();
        count = selected ? p->m_filesList->selectionModel()->selectedIndexes().count() : 0;
        break;
    case HWShell::VIEW_LIST:
        current = p->m_filesTable->currentIndex();
        selected = p->m_filesTable->selectionModel()->hasSelection();
        count = selected ? p->m_filesTable->selectionModel()->selectedIndexes().count() : 0;
        break;
    case HWShell::VIEW_COLUMN:
        current = p->m_filesColumn->currentIndex();
        selected = p->m_filesColumn->selectionModel()->hasSelection();
        count = selected ? p->m_filesColumn->selectionModel()->selectedIndexes().count() : 0;
        break;
    default:
        break;
    }

    if(count == 1)
    {
        if(p->m_currentModel == Filesystem)
        {
            QMimeDatabase db;
            QFileInfo info =
                    p->m_model->fileInfo(p->m_filesList->selectionModel()->selectedIndexes().first());
            fileName = info.fileName();
            fileSize = l.formattedDataSize(info.size());
            fileType = db.mimeTypeForFile(info.fileName()).comment();
            return tr("\"%1\" (%2) - %3").arg(fileName).arg(fileSize).arg(fileType);
        }
    }
    else if(count > 1)
        return tr("%1 items selected").arg(count);
    else
    {
        if(p->m_currentModel == Filesystem)
            return tr("%1 item").arg(p->m_model->rowCount(current));
    }
    return QString();
}

void LSEmbeddedShellHost::showFolderPermissionError(const QString &folder)
{
    LSCommonFunctions::instance()->showErrorDialog(
        tr("The folder \"%1\" can not be opened because you do not have permissions to see its contents.").arg(folder),
        tr("A system administrator can set the relevant execute flag to allow this."),
        this,
        QMessageBox::Information);
}

void LSEmbeddedShellHost::disconnectViewSlots()
{
    disconnect(p->m_filesColumn, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);
    disconnect(p->m_filesList, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);
    disconnect(p->m_filesTable, &QAbstractItemView::customContextMenuRequested, this, &LSEmbeddedShellHost::viewContextMenuRequested);

    disconnect(p->m_filesColumn, &QColumnView::activated, this, &LSEmbeddedShellHost::viewActivated);
    disconnect(p->m_filesColumn->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
    disconnect(p->m_filesList, &QListView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    disconnect(p->m_filesList, &QListView::activated, this, &LSEmbeddedShellHost::viewActivated);
    disconnect(p->m_filesList->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);

    disconnect(p->m_filesTable, &QTreeView::clicked, this, &LSEmbeddedShellHost::viewClicked);
    disconnect(p->m_filesTable, &QTreeView::activated, this, &LSEmbeddedShellHost::viewActivated);
    disconnect(p->m_filesTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LSEmbeddedShellHost::viewSelectionChanged);
}

void LSEmbeddedShellHost::updateBackForwardList(const QUrl &url, bool goback)
{
    if(!goback)
    {
        QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
        p->m_backLists[uuid].prepend(url);
        p->m_forwardLists[uuid].clear();
    }
    updateNavigationButtonStatus();
}

void LSEmbeddedShellHost::
    updateNavigationButtonStatus()
{
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();
    auto bc = p->m_backLists[uuid].count();
    auto fc = p->m_forwardLists[uuid].count();

    if(bc == 0)
        p->m_actions->shellAction(HWShell::ACT_GO_BACK)->setEnabled(false);
    else
        p->m_actions->shellAction(HWShell::ACT_GO_BACK)->setEnabled(true);

    if(fc == 0)
        p->m_actions->shellAction(HWShell::ACT_GO_FORWARD)->setEnabled(false);
    else
        p->m_actions->shellAction(HWShell::ACT_GO_FORWARD)->setEnabled(true);

}

void LSEmbeddedShellHost::
    swapToModel(ShellModel model)
{
    if(p->m_curSelModel)
        disconnect(p->m_curSelModel, &QItemSelectionModel::selectionChanged,
               this, &LSEmbeddedShellHost::viewSelectionChanged);

    switch(model)
    {
    case Applications:
        p->m_filesColumn->setModel(p->m_apps);
        p->m_filesList->setModel(p->m_apps);
        p->m_filesTable->setModel(p->m_apps);
        p->m_currentModel = Applications;
        p->m_filesColumn->setEditTriggers(QColumnView::NoEditTriggers);
        p->m_filesList->setEditTriggers(QListView::NoEditTriggers);
        p->m_filesTable->setEditTriggers(QTableView::NoEditTriggers);
        disableActionsForNoSelection();
        adjustColumnHeaders();
        p->m_filesList->setDragEnabled(true);
        break;
    case Trash:
        p->m_filesColumn->setModel(p->m_trash);
        p->m_filesList->setModel(p->m_trash);
        p->m_filesTable->setModel(p->m_trash);
        p->m_currentModel = Trash;
        p->m_filesColumn->setEditTriggers(QColumnView::NoEditTriggers);
        p->m_filesList->setEditTriggers(QListView::NoEditTriggers);
        p->m_filesTable->setEditTriggers(QTableView::NoEditTriggers);
        disableActionsForNoSelection();
        adjustColumnHeaders();
        break;
    case Filesystem:
    default:
        p->m_filesColumn->setModel(p->m_model);
        p->m_filesList->setModel(p->m_model);
        p->m_filesTable->setModel(p->m_model);
        p->m_currentModel = Filesystem;
        p->m_filesColumn->setEditTriggers(QColumnView::EditKeyPressed);
        p->m_filesList->setEditTriggers(QListView::EditKeyPressed);
        p->m_filesTable->setEditTriggers(QTableView::EditKeyPressed);
        doSort(p->m_sortColumn, p->m_sortOrder);
        disableActionsForNoSelection();
        filesystemSortingChanged();
        adjustColumnHeaders();
        break;
    }
    resetSelectionModel();
    connect(p->m_curSelModel, &QItemSelectionModel::selectionChanged,
        this, &LSEmbeddedShellHost::viewSelectionChanged);
}

void LSEmbeddedShellHost::swapModelForUrl(const QUrl &url)
{
    if(url.isLocalFile())
    {
        if(p->m_currentModel != Filesystem)
            swapToModel(Filesystem);

        return;
    }

    if(url.scheme() == QLatin1String("applications"))
    {
        if(p->m_currentModel != Applications)
            swapToModel(Applications);

        return;
    }

    if(url.scheme() == QLatin1String("trash"))
    {
        if(p->m_currentModel != Trash)
            swapToModel(Trash);

        return;
    }
}

void LSEmbeddedShellHost::resetSelectionModel()
{
    if(p->m_viewMode == HWShell::VIEW_ICONS)
        p->m_curSelModel = p->m_filesList->selectionModel();

    if(p->m_viewMode == HWShell::VIEW_LIST)
        p->m_curSelModel = p->m_filesTable->selectionModel();

    if(p->m_viewMode == HWShell::VIEW_COLUMN)
        p->m_curSelModel = p->m_filesColumn->selectionModel();
}

void LSEmbeddedShellHost::openItem(const QModelIndex &idx)
{
    if(p->m_currentModel == Filesystem)
    {
        QFileInfo fileInfo(p->m_model->fileInfo(idx));
        if(fileInfo.isDir())
        {
            // TODO: Handle a spacitial mode
            if(fileInfo.isReadable() && fileInfo.isExecutable())
            {
                updateBackForwardList(QUrl::fromLocalFile(fileInfo.canonicalFilePath()));
                updateRootIndex(idx);
            }
            else
                showFolderPermissionError(fileInfo.fileName());
        }

        // handle a .desktop file
        if(p->m_model->isDesktop(idx))
        {
            LSCommonFunctions::instance()->executeDesktopOverDBus(p->m_model->desktopFileForIndex(idx));
            return;
        }

        LSCommonFunctions::instance()->openFileOverDBusWithDefault(fileInfo.canonicalFilePath());
        // TODO: handle executing an executable file???
    }

    if(p->m_currentModel == Applications)
    {
        auto da = p->m_apps->data(idx, Qt::UserRole+1);
        LSCommonFunctions::instance()->executeDesktopOverDBus(da.toString());
    }

    if(p->m_currentModel == Trash)
    {
        // just show properties for a trashed item
    }
}

void LSEmbeddedShellHost::updateRootIndex(const QModelIndex &idx, bool internal)
{
    Q_UNUSED(internal)
    QSettings settings("originull", "hollywood");
    bool showFullPath = settings.value("Preferences/ShowFullPathName", false).toBool();
    QFileIconProvider icons;
    QFileInfo info(p->m_model->fileInfo(idx));
    p->m_filesList->setRootIndex(idx);

    // we shouldn't do the following two in desktop mode
    p->m_filesColumn->setRootIndex(idx);
    p->m_filesTable->setRootIndex(idx);
    while(p->m_model->canFetchMore(idx))
        p->m_model->fetchMore(idx);

    p->m_currentFsRoot = idx;

    // update our tab and window title
    QString directory = info.canonicalFilePath();
    QIcon ico = icons.icon(info);

    if(showFullPath)
    {
        emit updateWindowTitle(directory);
        p->m_tabs->setTabText(p->m_tabs->currentIndex(), directory);
    }
    else
    {
        QString shortDir = info.fileName();
        if(shortDir.isEmpty())
            shortDir = "/";
        emit updateWindowTitle(shortDir);
        p->m_tabs->setTabText(p->m_tabs->currentIndex(), shortDir);
    }
    p->m_tabs->setTabIcon(p->m_tabs->currentIndex(), ico);
    QUuid uuid = p->m_tabs->tabData(p->m_tabs->currentIndex()).toUuid();

    p->m_tabLocations[uuid] = QUrl::fromLocalFile(info.canonicalFilePath());
    emit updateWindowIcon(ico);
    QUrl url = QUrl::fromLocalFile(directory);
    p->m_location->setPath(url);

    updatePlaceModelSelection(url);
    // update our up (enclosing folder) action
    if(idx.parent() != QModelIndex())
        p->m_actions->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(true);
    else
        p->m_actions->shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER)->setEnabled(false);

    emit updateStatusBar(generateStatusBarMsg());

}

QAction* LSEmbeddedShellHost::shellAction(HWShell::ShellActions shellAction)
{
    return p->m_actions->shellAction(shellAction);
}

QActionGroup *LSEmbeddedShellHost::groupViewMode()
{
    return p->m_actions->groupViewMode();
}

QActionGroup *LSEmbeddedShellHost::groupViewColumn()
{
    return p->m_actions->groupViewColumn();
}

QActionGroup *LSEmbeddedShellHost::groupViewOrder()
{
    return p->m_actions->groupViewOrder();
}

QMenu *LSEmbeddedShellHost::openWithMenu()
{
    return p->m_actions->openWithMenu();
}

QMenu *LSEmbeddedShellHost::newMenu()
{
    return p->m_actions->newMenu();
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
    switch(p->m_viewMode)
    {
    case HWShell::VIEW_COLUMN:
        currentIndex = p->m_filesColumn->rootIndex();
        break;
    case HWShell::VIEW_LIST:
        currentIndex = p->m_filesTable->rootIndex();
        break;
    case HWShell::VIEW_ICONS:
    default:
        currentIndex = p->m_filesList->rootIndex();
    }
    updateBackForwardList(QUrl::fromLocalFile(p->m_model->fileInfo(currentIndex).canonicalFilePath()));
    updateRootIndex(currentIndex.parent());
}


