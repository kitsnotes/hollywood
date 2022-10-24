#ifndef CSAPPLICATION_H
#define CSAPPLICATION_H

#include <QApplication>
#include <QObject>
#include <QStandardItemModel>
#include <QMenu>
#include <QProcess>

class AIPrivateWaylandProtocol;
class ProgramManager;
class CSWindow;
class PlasmaWindowManagement;
class PlasmaWindow;
class CSApplication : public QApplication
{
    Q_OBJECT
public:
    CSApplication(int &argc, char **argv);
    static CSApplication* instance() { return static_cast<CSApplication*>(QApplication::instance()); }
    void createWindows();
    void aboutApplication();
    QStandardItemModel* model();
    QMenu* contextMenu();
    void executeDesktop(const QString &dt, const QStringList args = QStringList());
    PlasmaWindowManagement* windowManager() { return m_wndmgr; }
public slots:
    void showProgramManager();
private slots:
    void privateProtocolReady();
    void addScreen(QScreen *screen);
    void removeScreen(QScreen *screen);
    void primaryChanged(QScreen *screen);
    void launchSysmon();
    void launchSettings();
    void launchTerminull();
    void run();
    void logoffSession();
    void windowMinimized();
    void newPlasmaWindow(PlasmaWindow *c);
private:
    void startProcess(const QString &exec);
    void createStandardModelItems();

private:
    QList<CSWindow*> m_screenWindows;
    QList<QProcess*> m_processes;
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    PlasmaWindowManagement *m_wndmgr = nullptr;
    QStandardItemModel *m_model = nullptr;
    QMenu *m_context = nullptr;
    ProgramManager *m_pm = nullptr;
};

#endif // CSAPPLICATION_H
