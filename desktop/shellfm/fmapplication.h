#ifndef FMAPPLICATION_H
#define FMAPPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QString>
#include <QLocalServer>
#include <hollywood/aboutdialog.h>

#include "filewindow.h"

class HWPrivateWaylandProtocol;
class DesktopWindow;
class FileWindow;
class OperationManager;
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
    HWPrivateWaylandProtocol *protocol() { return m_protocol; }
public slots:
    void setupOperationManager();
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
    void rotateWallpaper();
private slots:
    void openFolderFromDesktop(const QUrl &path);
    void settingsChanged();
private:
    void checkForSessionStartup();
private:
    HWPrivateWaylandProtocol *m_protocol = nullptr;
    QStringList m_args;
    QList<FileWindow*> m_fileWindows;
    DesktopWindow *m_desktop = nullptr;
    bool m_sessionStarted = false;
    QLocalServer* m_socket = nullptr;
    bool m_dbusStarted = false;

    OperationManager* m_ops = nullptr;
};

#endif // FMAPPLICATION_H
