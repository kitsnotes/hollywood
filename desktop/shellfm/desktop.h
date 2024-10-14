#ifndef DESKTOPWINDOW_H
#define DESKTOPWINDOW_H

#include <QMainWindow>
#include <actionmanager.h>

#include <hollywood/layershellwindow.h>

class LSDesktopModel;
class LSFSItemDelegate;
class LSActionManager;
class DesktopViewOptions;
class DesktopWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopWindow(QWidget *parent = nullptr);
    void canUndoChanged();
    void canRedoChanged();
signals:
    void openFolder(const QUrl &folder);
public slots:
    void show();
    void enablePaste(bool enable, uint count = 0);
private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void activated(const QModelIndex &idx);
    void goQuickAction();
    void openItems();
    void copyItems();
    void paste();
    void newFolder();
    void rename();
    void archive();
    void trash();
    void eject();
    void viewOptionsChanged();
    void viewOptionsClosed();
    void getInfoRequested();
private:
    void setupActions();
    void setupMenuBar();
    void setupDesktopView();
    void desktopGeometryChanged(const QRect& geom);
    QAction* shellAction(HWShell::ShellActions shellAction);
    void contextMenu(const QPoint &pos);
    void updateSingleActionStrings();
    void resetActionStrings();
    void disableFileActions();
    void enableFileActions();
    void viewActionsToggled(bool toggle);
    void loadViewOptions();
    void saveViewOptions();
    void calculateGridSize();
private:
    QListView *m_view = nullptr;
    LSDesktopModel *m_model = nullptr;
    LSFSItemDelegate *m_delegate = nullptr;
    LSActionManager *m_actions = nullptr;
    QMenu *m_rightclick = nullptr;
    QAction *m_rotate = nullptr;

    QAction *a_NewWindow = nullptr;
    QAction *action_Help = nullptr;
    QAction *a_Add_Bookmark = nullptr;
    QAction *a_Toolbar = nullptr;
    QAction *a_StatusBar = nullptr;

    QAction *a_Home = nullptr;
    QAction *a_Documents = nullptr;
    QAction *a_Photos = nullptr;
    QAction *a_Videos = nullptr;
    QAction *a_Music = nullptr;
    QAction *a_Desktop = nullptr;
    QAction *a_Applications = nullptr;
    QAction *a_Connect_to_Server = nullptr;
    QAction *a_Network = nullptr;

    QMenuBar *m_menuBar = nullptr;
    QMenu *menu_File = nullptr;
    QMenu *menu_Open_With = nullptr;
    QMenu *menu_Edit = nullptr;
    QMenu *menu_View = nullptr;
    QMenu *menu_Go = nullptr;
    QMenu *menuRecent_Locations = nullptr;
    QMenu *menu_Bookmark = nullptr;
    QMenu *menu_Window = nullptr;
    QMenu *menu_Help = nullptr;

    DesktopViewOptions *m_viewOptions = nullptr;
    LayerShellQt::Window *m_lswnd;
};

#endif // DESKTOPWINDOW_H
