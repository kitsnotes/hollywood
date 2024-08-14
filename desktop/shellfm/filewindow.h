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
    LSEmbeddedShellHost *m_shellHost = nullptr;
    HWShell::WindowMode m_windowMode;
    QFileSystemModel *m_desktopModel = nullptr;
private:
    QAction *a_NewWindow = nullptr;
    QAction *actionGet_System_Help = nullptr;
    QAction *a_Add_Bookmark = nullptr;
    QAction *a_Toolbar = nullptr;
    QAction *a_StatusBar = nullptr;
    QAction *a_Close_Window = nullptr;

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
    QMenu *menu_Edit = nullptr;
    QMenu *menu_View = nullptr;
    QMenu *menu_Go = nullptr;
    QMenu *menu_Sort = nullptr;
    QMenu *menuRecent_Locations = nullptr;
    QMenu *menu_Bookmark = nullptr;
    QMenu *menu_Window = nullptr;
    QMenu *menu_Help = nullptr;
    QStatusBar *m_statusBar = nullptr;
    QLabel *m_statusLabel = nullptr;
    QToolBar *m_toolBar = nullptr;
    QToolButton *m_sort = nullptr;
    QToolButton *m_view = nullptr;
    QLineEdit *m_search = nullptr;
};

#endif // FILEWINDOW_H
