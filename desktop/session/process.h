// Hollywood Session Manager
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MANAGEDPROCESS_H
#define MANAGEDPROCESS_H

#include <QProcess>
#include <QObject>

class ManagedProcess : public QProcess
{
    Q_OBJECT
public:
    enum Process
    {
        DBusSession,
        Pipewire,
        Wireplumber,
        PipewirePulse,
        ATSPI,
        Compositor,
        Elevator,
        NetworkAgent,
        NotificationDaemon,
        XDGPortal,
        HPNS,
        Conelrad,
        UpdateAgent,
        Stage,
        Shell
    };
    ManagedProcess(Process desired, QObject *parent = nullptr);
    bool initialize();
    bool deconstruct();
    void setAutoRestart(bool restart);
Q_SIGNALS:
    void compositorDied();
    void dbusSessionBusAddressAvailable(const QString &env);
private slots:
    void processStopped();
    void logStdOut();
    void logStdErr();
private:
    QString processExecutablePath();
    void setEnvironmentForProcess();
    void setArgumentsForProcess();
    void logInfo(const QString &msg);
    bool checkForExistingDBusSession() const;
    bool verifyCompositorSocket() const;
    bool watchForCompositor();
private:
    bool m_init = false;
    bool m_ok = false;
    uint m_fail = 0;
    Process m_process;
    bool m_restart = true;
};

#endif // MANAGEDPROCESS_H
