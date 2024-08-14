#ifndef SHELLHOST_P_H
#define SHELLHOST_P_H

#include <QWidget>
#include <QAction>

#include <QSplitter>
#include <QTabWidget>
#include <QTreeView>
#include <QColumnView>
#include <QTabBar>
#include <QVBoxLayout>
#include <QItemSelectionModel>
#include <QAbstractItemModel>

#include "libshell_int.h"
#include "shellhost.h"
#include "trashmodel.h"

class ApplicationModel;
class LSLocationBar;
class LSActionManager;
class LSViewOptionsDialog;
class LSFSModel;
class LSPlaceModel;
class LSPlaceView;
class LSColumnPreview;
class LSFSItemDelegate;
class LSMimeApplications;
typedef QList<QUrl> UrlList;
class OperationManager;
class LSEmbeddedShellHost;
class LSEmbeddedShellHostPrivate
{
private:
    friend class LSEmbeddedShellHost;
    LSEmbeddedShellHost *d;
    LSEmbeddedShellHostPrivate(LSEmbeddedShellHost *parent, OperationManager *ops = nullptr);
    ~LSEmbeddedShellHostPrivate() = default;

    LSEmbeddedShellHost::ShellModel m_currentModel = LSEmbeddedShellHost::Filesystem;
    LSActionManager *m_actions;
    LSLocationBar *m_location = nullptr;
    QSplitter *m_mainSplitter;
    QTabWidget *m_treeToolbox;
    LSPlaceView *m_treeFavorites;
    QTreeView *m_treeDirs;

    QColumnView *m_filesColumn;
    LSColumnPreview *m_columnPreview = nullptr;
    QListView *m_filesList;
    QTreeView *m_filesTable;

    uint m_sortColumn;
    Qt::SortOrder m_sortOrder;
    HWShell::ViewMode m_viewMode;
    LSFSModel *m_model;
    ApplicationModel *m_apps;
    LSTrashModel *m_trash;

    LSPlaceModel *m_placeModel;
    QMap<QUuid, QUrl> m_tabLocations;
    QMap<QUuid, HWShell::ViewMode> m_tabViewMode;
    QMap<QUuid, Qt::SortOrder> m_tabSortOrder;
    QMap<QUuid, uint> m_tabSortCol;

    QMap<QUuid, UrlList> m_backLists;
    QMap<QUuid, UrlList> m_forwardLists;

    QTabBar *m_tabs;
    QVBoxLayout *m_tabWndHostLayout;
    QWidget *m_tabWndHost;
    LSViewOptionsDialog *m_viewOptions;
    LSFSItemDelegate *m_delegate = nullptr;

    QItemSelectionModel *m_curSelModel = nullptr;
    LSMimeApplications *m_mimeapps = nullptr;
};

#endif // SHELLHOST_P_H
