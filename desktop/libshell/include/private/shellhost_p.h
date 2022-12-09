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

#include "libshell_int.h"
#include "shellhost.h"

class ApplicationModel;
class LSLocationBar;
class LSActionManager;
class LSViewOptionsDialog;
class LSFSModel;
class LSPlaceModel;
class LSPlaceView;
class LSColumnPreview;
class LSFSItemDelegate;

typedef LSFSModel FilesystemModel;
typedef QList<QUrl> UrlList;

class LSEmbeddedShellHost;
class LSEmbeddedShellHostPrivate
{
private:
    friend class LSEmbeddedShellHost;
    LSEmbeddedShellHost *d;
    LSEmbeddedShellHostPrivate(LSEmbeddedShellHost *parent);
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

    HWShell::ViewMode m_viewMode;
    FilesystemModel *m_model;

    ApplicationModel *m_apps;

    LSPlaceModel *m_placeModel;
    QMap<QUuid, QUrl> m_tabLocations;
    QMap<QUuid, HWShell::ViewMode> m_tabViewMode;

    QMap<QUuid, UrlList> m_backLists;
    QMap<QUuid, UrlList> m_forwardLists;

    QTabBar *m_tabs;
    QVBoxLayout *m_tabWndHostLayout;
    QWidget *m_tabWndHost;
    LSViewOptionsDialog *m_viewOptions;
    LSFSItemDelegate *m_delegate = nullptr;

    QItemSelectionModel *m_curSelModel = nullptr;

};

#endif // SHELLHOST_P_H
