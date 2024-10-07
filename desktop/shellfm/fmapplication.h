#ifndef FMAPPLICATION_H
#define FMAPPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QString>
#include <QLocalServer>
#include <hollywood/aboutdialog.h>

#include "private/disks.h"
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
    OperationManager* operationManager();
    void removeWindow(FileWindow *wnd);
public slots:
    bool executeDesktopOverDBus(const QString &desktop);
    bool openFileOverDBusWithDefault(const QString &file);
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
    void setupWindowMenu(QMenu *window);
    void openHelp();
private slots:
    void openFolderFromDesktop(const QUrl &path);
    void settingsChanged();
    void canUndoChanged();
    void canRedoChanged();
    void disksUpdated();
    void mediaChanged(QString path, bool media);
    void mountpointChanged(QString path, QString mountpoint);
    void deviceErrorMessage(QString path, QString error);
    void foundNewDevice(QString path);
    void removedDevice(QString path);
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
};

#endif // FMAPPLICATION_H
