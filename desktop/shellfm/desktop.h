#ifndef DESKTOPWINDOW_H
#define DESKTOPWINDOW_H

#include <QMainWindow>
#include <actionmanager.h>

class LSFSModel;
class LSFSItemDelegate;
class LSActionManager;
class DesktopWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopWindow(QWidget *parent = nullptr);

signals:
    void openFolder(const QUrl &folder);
private:
    void setupActions();
    void setupMenuBar();
    void setupDesktopView();
    void desktopGeometryChanged(const QRect& geom);
    QAction* shellAction(HWShell::ShellActions shellAction);
    void activated(const QModelIndex &idx);
    void contextMenu(const QPoint &pos);
private:
    QListView *m_view = nullptr;
    LSFSModel *m_model = nullptr;
    LSFSItemDelegate *m_delegate = nullptr;
    LSActionManager *m_actions = nullptr;
    QMenu *m_rightclick = nullptr;

    QAction *a_NewWindow;
    QAction *actionGet_System_Help;
    QAction *a_About;
    QAction *a_Add_Bookmark;
    QAction *a_Toolbar;
    QAction *a_StatusBar;
    QAction *a_Close_Window;


    QAction *a_Home;
    QAction *a_Documents;
    QAction *a_Photos;
    QAction *a_Videos;
    QAction *a_Music;
    QAction *a_Desktop;
    QAction *a_Applications;
    QAction *a_Connect_to_Server;
    QAction *a_Network;

    QMenuBar *m_menuBar;
    QMenu *menu_File;
    QMenu *menu_Edit;
    QMenu *menu_View;
    QMenu *menu_Go;
    QMenu *menuRecent_Locations;
    QMenu *menu_Bookmark;
    QMenu *menu_Help;
};

#endif // DESKTOPWINDOW_H
