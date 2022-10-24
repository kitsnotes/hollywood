#ifndef CSAPPLICATION_H
#define CSAPPLICATION_H

#include <QApplication>
#include <QObject>
#include <QStandardItemModel>
#include <QMenu>
#include <QProcess>
#include <hollywood/layershellinterface.h>

class AIPrivateWaylandProtocol;
class ProgramManager;
class PlasmaWindowManagement;
class PlasmaWindow;
class StageHost;
class StageApplication : public QApplication
{
    Q_OBJECT
public:
    StageApplication(int &argc, char **argv);
    static StageApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }
    void createWindows();
    void aboutApplication();
    QStandardItemModel* model();
    void executeDesktop(const QString &dt, const QStringList args = QStringList());
    PlasmaWindowManagement* windowManager() { return m_wndmgr; }
public slots:
    void launchSysmon();
    void launchSettings();
    void launchTerminull();
    void run();
    void logoffSession();
    void showProgramManager();
private slots:
    void privateProtocolReady();

    void windowMinimized();
    void newPlasmaWindow(PlasmaWindow *c);
private:
    void startProcess(const QString &exec);
private:
    StageHost *m_host;
    QList<QProcess*> m_processes;
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    PlasmaWindowManagement *m_wndmgr = nullptr;
    QStandardItemModel *m_model = nullptr;

    ProgramManager *m_pm = nullptr;
};

#endif // CSAPPLICATION_H
