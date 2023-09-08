#ifndef FMAPPLICATION_H
#define FMAPPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QString>
#include <QLocalServer>
#include <hollywood/aboutdialog.h>

#include "filewindow.h"

class DesktopWindow;
class FileWindow;
class FMApplication : public QApplication
{
    Q_OBJECT
public:
    FMApplication(int &argc, char **argv);
    static FMApplication* instance() { return static_cast<FMApplication*>(QApplication::instance()); }
    enum ViewMode {
        VIEW_ICONS,
        VIEW_LIST,
        VIEW_COLUMN
    };
    void createDBusInterfaces();
    bool checkSocket();
public slots:
    bool executeDesktopOverDBus(const QString &desktop);
    void ShowFolders(const QStringList& urlList, const QString& startupId);
    void ShowItems(const QStringList& urlList, const QString& startupId);
    void ShowItemProperties(const QStringList& urlList, const QString& startupId);
    void aboutApplication();
    void newFileWindow();
    void newFileWindow(const QUrl &path);
    void createDesktop();
    void showWallpaperSettings();
    void showPermissionError(const QUrl &path);
private slots:
    void openFolderFromDesktop(const QUrl &path);
private:
    void checkForSessionStartup();
private:
    QStringList m_args;
    QList<FileWindow*> m_fileWindows;
    DesktopWindow *m_desktop = nullptr;
    bool m_sessionStarted = false;
    QLocalServer* m_socket = nullptr;
    bool m_dbusStarted = false;
};

#endif // FMAPPLICATION_H
