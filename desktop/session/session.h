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
private:
    bool startCompositor();
    QProcessEnvironment createCompositorEnvironment();
    bool verifyCompositorSocketAvailable();
    bool startMenuServer();
    bool startElevator();
    bool startDesktop();
    bool startStage();
    QProcessEnvironment createDesktopEnvironment();
    bool verifyXdgRuntime();
    bool stopSession();
private slots:
    void processStdOut();
    void processStdError();
    void compositorStopped();
    void menuServerStopped();
    void elevatorStopped();
    void desktopStopped();
    void stageStopped();
private:
    QProcess *m_compProcess = nullptr;
    QString m_compSocket;
    QProcess *m_menuProcess = nullptr;
    QProcess *m_stageProcess = nullptr;
    QProcess *m_desktopProcess = nullptr;
    QProcess *m_elevatorProcess = nullptr;
    QLocalServer* m_socket = nullptr;
    SessionDBus *m_dbus = nullptr;
    QString m_xdg_runtime_dir;
    bool m_sessionStarted = false;
};

#endif // SMAPPLICATION_H
