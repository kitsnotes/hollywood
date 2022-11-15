#ifndef CSAPPLICATION_H
#define CSAPPLICATION_H

#include <QApplication>
#include <QObject>
#include <QStandardItemModel>
#include <QMenu>
#include <QProcess>
#include <QFileSystemWatcher>
#include <hollywood/layershellinterface.h>

class AIPrivateWaylandProtocol;
class ProgramManager;
class PlasmaWindowManagement;
class PlasmaWindow;
class StageHost;
class LSDesktopEntry;
class StageApplication : public QApplication
{
    Q_OBJECT
public:
    StageApplication(int &argc, char **argv);
    static StageApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }
    void createWindows();
    void aboutApplication();
    QStandardItemModel* model();
    void executeDesktop(const QString &desktop);
    PlasmaWindowManagement* windowManager() { return m_wndmgr; }
    bool callSessionDBus(const QString &exec);
    bool displayManagerStart() { return m_started_dm; }
    QMenu* systemMenu() { return m_context; }
public slots:
    void launchSysmon();
    void launchSettings();
    void launchTerminull();
    void launchAbout();
    void run();
    void logoffSession();
    void showProgramManager();
    void restartSystem();
    void shutdownSystem();
private slots:
    void privateProtocolReady();
    void windowMinimized();
    void newPlasmaWindow(PlasmaWindow *c);
    void configChanged();
Q_SIGNALS:
    void clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm);
    void settingsChanged();
private:
    void startProcess(const QString &exec);
    void loadSettings();
private:
    QFileSystemWatcher *m_cfgwatch = nullptr;
    StageHost *m_host = nullptr;
    QList<QProcess*> m_processes;
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    PlasmaWindowManagement *m_wndmgr = nullptr;
    QStandardItemModel *m_model = nullptr;
    ProgramManager *m_pm = nullptr;
    QString m_configfile;
    QMenu *m_context = nullptr;


    bool m_started_dm = false;
};

#endif // CSAPPLICATION_H
