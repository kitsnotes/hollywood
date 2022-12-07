#ifndef SHELLHOST_H
#define SHELLHOST_H

#include "libshell_int.h"

class ApplicationModel;
class LSLocationBar;
class LSActionManager;
class ViewOptionsDialog;
class LSFSModel;
class LSPlaceModel;
class LSPlaceView;
class LSColumnPreview;
class LSFSItemDelegate;

typedef LSFSModel FilesystemModel;
typedef QList<QUrl> UrlList;

class LIBSHELL_EXPORT LSEmbeddedShellHost : public QWidget
{
    Q_OBJECT
public:
    enum ShellModel
    {
        Filesystem,
        Applications,
        Network,
        Trash
    };
    explicit LSEmbeddedShellHost(QWidget *parent = nullptr);
    QAction* shellAction(ArionShell::ShellActions shellAction);
    LSLocationBar* locationBar();
public slots:
    bool navigateToPath(const QString &path);
    bool navigateToUrl(const QUrl &path);
    void newTabWithPath(const QUrl &path);
    void setIconListView(bool updateSettings = true);
    void setTableListView(bool updateSettings = true);
    void setColumnView(bool updateSettings = true);
    // view actions
    void goBack();
    void goForward();
    void goEnclosingFolder();
    void toggleViewOptions(bool);
signals:
    void updateWindowTitle(const QString &title);
    void updateWindowIcon(const QIcon &icon);
    void updateStatusBar(const QString &status);
protected:
    void resizeEvent(QResizeEvent* event);
private slots:
    void locationBarEditingFinished();
    // view options
    void viewOptionsFinished(int);
    void currentTabChanged(int index);
    void tabCloseRequested(int index);
    // view slots
    void viewClicked(const QModelIndex &idx);
    void viewActivated(const QModelIndex &idx);
    void viewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void placeClicked(const QModelIndex &idx);
    void updateColumnWidget(const QModelIndex &idx);
    // model slots
    void modelRootPathChanged(const QString &path);
    void createNewTab();
    void viewContextMenuRequested(const QPoint &pos);
private:
    bool executeDesktopOverDBus(const QString &desktop);
    bool openFileOverDBusWithDefault(const QString &file);
    void disableActionsForNoSelection();
    void enableActionsForFileSelection(bool multiple);
    void updatePlaceModelSelection(const QUrl &place);
    QString generateStatusBarMsg() const;
    void setupLocationBar();
    void restoreViewModeFromSettings();
    void updateRootIndex(const QModelIndex &idx, bool internal = false);
    void showFolderPermissionError(const QString &folder);
    void disconnectViewSlots();
    void updateBackForwardList(const QUrl &url, bool goback = false);
    void updateNavigationButtonStatus();
    void swapToModel(ShellModel model);
    void swapModelForUrl(const QUrl &url);
private:
    ShellModel m_currentModel = ShellModel::Filesystem;
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

    ArionShell::ViewMode m_viewMode;
    FilesystemModel *m_model;

    ApplicationModel *m_apps;

    LSPlaceModel *m_placeModel;
    QMap<QUuid, QUrl> m_tabLocations;
    QMap<QUuid, ArionShell::ViewMode> m_tabViewMode;

    QMap<QUuid, UrlList> m_backLists;
    QMap<QUuid, UrlList> m_forwardLists;

    QTabBar *m_tabs;
    QVBoxLayout *m_tabWndHostLayout;
    QWidget *m_tabWndHost;
    ViewOptionsDialog *m_viewOptions;
    LSFSItemDelegate *m_delegate = nullptr;

    QItemSelectionModel *m_curSelModel = nullptr;
};

#endif // SHELLHOST_H
