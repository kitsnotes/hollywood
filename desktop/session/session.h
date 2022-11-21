#ifndef SMAPPLICATION_H
#define SMAPPLICATION_H

#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QtNetwork/QLocalSocket>
#include <unistd.h>

#if defined(wcApp)
#undef wcApp
#endif
#define wcApp (SMApplication::instance())

class QLocalServer;
class SessionDBus;
class SMApplication : public QCoreApplication
{
public:
    SMApplication(int &argc, char **argv);
    static SMApplication* instance() { return static_cast<SMApplication*>(QCoreApplication::instance()); }
    bool startSession();
    void logout();
    void shutdown();
    void reboot();
    bool canShutdown();
    bool canReboot();

    void setUseDBus(bool use) { m_useDBus = use; }
    void setUseElevator(bool use) { m_useElevator = use; }
    void setUsePipewire(bool use) { m_usePipewire = use; }
    bool displayManagerStart() { return m_startedDisplayManager; }
    bool haveSDDMAutoStart();
private:
    bool startCompositor();
    QProcessEnvironment createCompositorEnvironment();
    bool verifyCompositorSocketAvailable();
    bool startDBusSession();
    bool startMenuServer();
    bool startElevator();
    bool startDesktop();
    bool startStage();
    bool startPipewire();
    bool startWireplumber();
    bool startPipewirePulse();
    QProcessEnvironment createDesktopEnvironment();
    bool verifyXdgRuntime();
    bool stopSession();
    bool callLogindDbus(const QString &command);
    void disconnectProcessWatchers();
private slots:
    void dbusStdOut();
    void processStdOut();
    void processStdError();
    void compositorStopped();
    void menuServerStopped();
    void elevatorStopped();
    void desktopStopped();
    void stageStopped();
    void dbusProcessStopped();
    void pipewireStopped();
    void wireplumberStopped();
    void pipewirePulseStopped();
private:
    QProcess *m_compProcess = nullptr;
    QString m_compSocket;
    QProcess *m_menuProcess = nullptr;
    QProcess *m_stageProcess = nullptr;
    QProcess *m_desktopProcess = nullptr;
    QProcess *m_elevatorProcess = nullptr;
    QProcess *m_dbusSessionProcess = nullptr;
    QProcess *m_pipewireProcess = nullptr;
    QProcess *m_pipewirePulseProcess = nullptr;
    QProcess *m_wireplumberProcess = nullptr;
    QLocalServer* m_socket = nullptr;
    SessionDBus *m_dbus = nullptr;
    QString m_xdg_runtime_dir;
    bool m_sessionStarted = false;

    QString m_dbusSessionVar;
    bool m_useDBus = true;
    bool m_useElevator = true;
    bool m_usePipewire = true;

    bool m_startedDisplayManager = false;
};

#endif // SMAPPLICATION_H
