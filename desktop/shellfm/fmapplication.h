#ifndef FMAPPLICATION_H
#define FMAPPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QString>
#include <aboutdialog.h>

#include "filewindow.h"

class AIPrivateWaylandProtocol;
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
public slots:
    void aboutApplication();
    void newFileWindow();
    void newFileWindow(const QUrl &path);
    void createDesktop();
    void showWallpaperSettings();
private slots:
    void privateProtocolReady();
    void openFolderFromDesktop(const QUrl &path);
private:
    QStringList m_args;
    QList<FileWindow*> m_fileWindows;
    DesktopWindow *m_desktop = nullptr;
    AIPrivateWaylandProtocol *m_protocol = nullptr;

};

#endif // FMAPPLICATION_H
