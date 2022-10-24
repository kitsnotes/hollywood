#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QActionGroup>

#include <locationbar.h>
#include <shellhost.h>
#include "fileshell.h"
#include "viewoptionsdialog.h"

class ViewOptionsDialog;
class FileWindow : public QMainWindow
{
    Q_OBJECT
    friend class FMApplication;

public:
    FileWindow(ArionShell::WindowMode windowMode = ArionShell::WINDOW_BROWSER,
               QWidget *parent = nullptr);
    ~FileWindow();
protected:
    void closeEvent(QCloseEvent* event);
    LSEmbeddedShellHost* shellHost() { return m_shellHost; }
public slots:
    // tab functions
private slots:
    void shWindowTitleChanged(const QString &title);
    // desktop functions
    void desktopGeometryChanged(const QRect& geom);
    // view options
    void statusBarActionToggled(bool);
    void goQuickAction();
private:
    void setupActions();
    void setupMenuBar();
    void setupMainView();
    void setupDesktopView();
private:
    LSEmbeddedShellHost *m_shellHost;
    ArionShell::WindowMode m_windowMode;
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
    QMenu *menuRecent_Locations;
    QMenu *menu_Bookmark;
    QMenu *menu_Help;
    QStatusBar *m_statusBar;
    QLabel *m_statusLabel;
    QToolBar *m_toolBar;

};

#endif // FILEWINDOW_H
