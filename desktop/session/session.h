// Hollywood Session Manager
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SMAPPLICATION_H
#define SMAPPLICATION_H

#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QtNetwork/QLocalSocket>
#include <unistd.h>
#include <QLoggingCategory>

#if defined(wcApp)
#undef smApp
#endif
#define smApp (SMApplication::instance())

class LSMimeApplications;
class QLocalServer;
class SessionDBus;
class ManagedProcess;
class LSExecutor;
class SMApplication : public QCoreApplication
{
public:
    enum LocaleVariable
    {
        LANG,
        LCNUMERIC,
        LCTIME,
        LCCURRENCY,
        LCMEASURMENT,
        LCCOLLATE
    };
    SMApplication(int &argc, char **argv);
    static SMApplication* instance() { return static_cast<SMApplication*>(QCoreApplication::instance()); }
    bool startSession();
    void logout();
    void shutdown();
    void reboot();
    bool canShutdown();
    bool canReboot();
    void restartSession();
    bool executeDesktop(const QString &desktop, const QStringList &arguments, const QStringList &env);
    bool openFileWithDefault(const QString &file);
    bool useDbus() const { return m_useDBus; }
    void setUseDBus(bool use) { m_useDBus = use; }
    bool useElevator() const { return m_useElevator; }
    void setUseElevator(bool use) { m_useElevator = use; }
    bool usePipewire() const { return m_usePipewire; }
    void setUsePipewire(bool use) { m_usePipewire = use; }
    bool displayManagerStart() { return m_startedDisplayManager; }
    bool haveSDDMAutoStart();
    bool doingShutdown() const { return m_doingShutdown; }
    QString expectedCompositorSocket() const;
    QByteArray dbusSocket() { return m_dbusSessionVar; }
    QString localeVariable(LocaleVariable v);
    bool detailedLocaleSettings() const { return m_detailedregion; }
private:
    void startDBusReliantServices();
    bool verifyXdgRuntime();
    bool stopSession();
    bool callLogindDbus(const QString &command);
    void disconnectProcessWatchers();
    void restartCompositorReliantProcesses();
    void verifyTrashFolder();
private slots:
    void compositorDied();
    void userProcessTerminated(int result);
    bool terminateUserProcesses();
    void dbusAvailable(const QString &socket);
    void reloadLocaleSettings();
private:
    QString m_compSocket;

    LSMimeApplications *m_mime = nullptr;
    ManagedProcess *m_dbusSessionProcess = nullptr;
    ManagedProcess *m_compositorProcess = nullptr;
    ManagedProcess *m_pipewireProcess = nullptr;
    ManagedProcess *m_wireplumberProcess = nullptr;
    ManagedProcess *m_elevatorProcess = nullptr;
    ManagedProcess *m_stageProcess = nullptr;
    ManagedProcess *m_desktopProcess = nullptr;
    ManagedProcess *m_pipewirePulseProcess = nullptr;

    QLocalServer* m_socket = nullptr;
    SessionDBus *m_dbus = nullptr;
    QString m_xdg_runtime_dir;
    bool m_sessionStarted = false;

    QByteArray m_dbusSessionVar = "";
    bool m_useDBus = true;
    bool m_useElevator = true;
    bool m_usePipewire = true;

    bool m_startedDisplayManager = false;

    bool m_doingShutdown = false;
    QList<LSExecutor*> m_userprocs;

    QString m_lang = "C.UTF8";
    bool m_detailedregion = false;
    QString m_numeric = "";
    QString m_time = "";
    QString m_currency = "";
    QString m_measurement = "";
    QString m_collation = "";
};

#endif // SMAPPLICATION_H
