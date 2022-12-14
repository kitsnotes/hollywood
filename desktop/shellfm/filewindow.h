#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QActionGroup>

#include <locationbar.h>
#include <shellhost.h>
#include "fileshell.h"
#include "viewoptionsdialog.h"

class LSViewOptionsDialog;
class FileWindow : public QMainWindow
{
    Q_OBJECT
    friend class FMApplication;

public:
    FileWindow(HWShell::WindowMode windowMode = HWShell::WINDOW_BROWSER,
               QWidget *parent = nullptr);
    ~FileWindow();
protected:
    void closeEvent(QCloseEvent* event);
    LSEmbeddedShellHost* shellHost() { return m_shellHost; }
public slots:
    // tab functions
private slots:
    void shWindowTitleChanged(const QString &title);
    // view options
    void statusBarActionToggled(bool);
    void goQuickAction();
    void viewModeChanged();
    void sortOrderChanged();
private:
    void setupActions();
    void setupMenuBar();
    void setupMainView();
private:
    LSEmbeddedShellHost *m_shellHost;
    HWShell::WindowMode m_windowMode;
    QFileSystemModel *m_desktopModel;
private:
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
    QMenu *menu_Sort;
    QMenu *menuRecent_Locations;
    QMenu *menu_Bookmark;
    QMenu *menu_Help;
    QStatusBar *m_statusBar;
    QLabel *m_statusLabel;
    QToolBar *m_toolBar;
    QToolButton *m_sort = nullptr;
    QToolButton *m_view = nullptr;
    QLineEdit *m_search;
};

#endif // FILEWINDOW_H
