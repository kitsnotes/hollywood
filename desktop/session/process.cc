// Hollywood Session Manager
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "process.h"
#include "session.h"

#include <QLoggingCategory>


QLoggingCategory lcSessionS("hollywood.session");

QLoggingCategory lcCompositor("hollywood.compositor");
QLoggingCategory lcStage("hollywood.stage");
QLoggingCategory lcDesktop("hollywood.shell");
QLoggingCategory lcElevator("hollywood.elevator");
QLoggingCategory lcNotification("hollywood.notificationd");
QLoggingCategory lcWireplumber("wireplumber");
QLoggingCategory lcPipewire("pipewire");
QLoggingCategory lcPipewirePulse("pipewire.pulse");
QLoggingCategory lcDBus("dbus.session");
QLoggingCategory lsOther("other.process");

ManagedProcess::ManagedProcess(Process desired, QObject *parent)
    : QProcess(parent)
    , m_process(desired)
{
    auto app = processExecutablePath();
    setProgram(app);
    setProgram(processExecutablePath());
    setArgumentsForProcess();

    connect(this, &QProcess::finished, this, &ManagedProcess::processStopped);
    connect(this, &QProcess::errorOccurred, this, &ManagedProcess::processStopped);
    connect(this, &QProcess::readyReadStandardError, this, &ManagedProcess::logStdErr);
    connect(this, &QProcess::readyReadStandardOutput, this, &ManagedProcess::logStdOut);
}

bool ManagedProcess::initialize()
{
    setEnvironmentForProcess();
    if(m_process == DBusSession)
    {
        if(!smApp->useDbus())
        {
            m_ok = true;
            return true;
        }

        if(checkForExistingDBusSession())
        {
            m_ok = true;
            logInfo("Not starting a user dbus session as one already exists.");
            return true;
        }
    }

    start();

    if(m_process == Compositor)
        return watchForCompositor();

    if(isOpen())
    {
        logInfo(QString("started process (pid %1)").arg(processId()));
        return true;
    }
    m_fail++;
    return false;
}

bool ManagedProcess::deconstruct()
{
    logInfo(QString("process %1 terminating...").arg(processId()));
    terminate();
    while(waitForFinished(150))
    {
        if(state() == QProcess::NotRunning)
        {
            logInfo("terminated successfully");
            continue;
        }
    }

    logInfo("failed to close after 150 seconds: force closing");
    kill();
    return true;
}

void ManagedProcess::setAutoRestart(bool restart)
{
    m_restart = restart;
}

void ManagedProcess::processStopped()
{
    if(!m_restart)
        return;

    // if we're shutting down don't reap the process
    if(smApp->doingShutdown())
        return;

    if(m_process == Compositor)
    {
        // we let the session manager handle this
        // since we need to bring back up other processes
        emit compositorDied();
        return;
    }
    uint failcount = 10;
    if(m_process == Elevator)
        failcount = 1;
    if(m_fail > failcount)
    {
        logInfo("process over 10 failures, so giving up.  your desktop may be degraded.");
        return;
    }

    logInfo(QString("process stopped with return %1 after %2 failures. Restarting...")
          .arg(QString::number(exitCode()), QString::number(m_fail)));

    m_fail++;
    start();
}

void ManagedProcess::logStdOut()
{
    auto lines = readAllStandardOutput().split('\n');
    for(auto &line : lines)
    {
        if(line.isEmpty())
            continue;

        if(m_process == DBusSession)
        {
            // we need to check for our DBUS_SESSION_BUS_ADDRESS
            if(line.startsWith("unix:"))
            {
               emit dbusSessionBusAddressAvailable(line);
               disconnect(this, &QProcess::readyReadStandardOutput, this, &ManagedProcess::logStdOut);
            }
        }
        logInfo(QString("%1").arg(line));
    }
}

void ManagedProcess::logStdErr()
{
    auto lines = readAllStandardError().split('\n');
    for(auto line : lines)
    {
        if(line.isEmpty())
            continue;

        logInfo(QString("%1").arg(line));
    }
}

QString ManagedProcess::processExecutablePath()
{
    // handle our out-of-tree processes
    QString fullPath;

    switch(m_process)
    {
    case DBusSession:
        return  QString("/usr/bin/dbus-daemon");
        break;
    case Pipewire:
    case PipewirePulse:
        return QString("/usr/bin/pipewire");
    case Wireplumber:
        return QString("/usr/bin/wireplumber");
    case Compositor:
        return QString("/usr/libexec/hollywood/compositor");
    case Elevator:
        return QString("/usr/libexec/hollywood/elevator");
    case Stage:
        return QString("/usr/libexec/hollywood/stage");
    case NetworkAgent:
        return QString("/usr/libexec/hollywood/netagentd");
    case NotificationDaemon:
        return QString("/usr/libexec/hollywood/notificationd");
    case XDGPortal:
        return QString("/usr/libexec/hollywood/portald");
    case Shell:
        return QString("/usr/bin/shellfm");
    default:
        return QString();
    }
}

void ManagedProcess::setEnvironmentForProcess()
{
    auto env = QProcessEnvironment::systemEnvironment();

    // handle a debug environment
#ifdef QT_DEBUG
//    env.insert("LD_LIBRARY_PATH", smApp->applicationDirPath().toUtf8());
#endif
    // we *always* use the hollywood platformtheme
    env.remove("QT_QPA_PLATFORMTHEME");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
    env.insert("XDG_CURRENT_DESKTOP", "Hollywood");
    env.insert("XDG_SESSION_TYPE", "wayland");
    env.insert("XDG_MENU_PREFIX", "hollywood-");

    // handle our locale env
    if(env.contains("LANG"))
        env.remove("LANG");

    env.insert("LANG", smApp->localeVariable(SMApplication::LANG));
    if(smApp->detailedLocaleSettings())
    {
        if(env.contains("LC_COLLATE"))
            env.remove("LC_COLLATE");
        env.insert("LC_COLLATE", smApp->localeVariable(SMApplication::LCCOLLATE));
        if(env.contains("LC_TIME"))
            env.remove("LC_TIME");
        env.insert("LC_TIME", smApp->localeVariable(SMApplication::LCTIME));
        if(env.contains("LC_MONETARY"))
            env.remove("LC_MONETARY");
        env.insert("LC_MONETARY", smApp->localeVariable(SMApplication::LCCURRENCY));
        if(env.contains("LC_NUMERIC"))
            env.remove("LC_NUMERIC");
        env.insert("LC_NUMERIC", smApp->localeVariable(SMApplication::LCNUMERIC));
        if(env.contains("LC_MEASUREMENT"))
            env.remove("LC_MEASUREMENT");
        env.insert("LC_MEASUREMENT", smApp->localeVariable(SMApplication::LCMEASURMENT));
    }
    else
    {
        if(env.contains("LC_COLLATE"))
            env.remove("LC_COLLATE");
        env.insert("LC_COLLATE", smApp->localeVariable(SMApplication::LANG));
        if(env.contains("LC_TIME"))
            env.remove("LC_TIME");
        env.insert("LC_TIME", smApp->localeVariable(SMApplication::LANG));
        if(env.contains("LC_MONETARY"))
            env.remove("LC_MONETARY");
        env.insert("LC_MONETARY", smApp->localeVariable(SMApplication::LANG));
        if(env.contains("LC_NUMERIC"))
            env.remove("LC_NUMERIC");
        env.insert("LC_NUMERIC", smApp->localeVariable(SMApplication::LANG));
        if(env.contains("LC_MEASUREMENT"))
            env.remove("LC_MEASUREMENT");
        env.insert("LC_MEASUREMENT", smApp->localeVariable(SMApplication::LANG));
    }

    if(m_process == Compositor)
    {
        // special eglfs handling for the compositor
        if(!env.contains("DISPLAY"))
        {
            env.remove("QT_QPA_PLATFORM");
            if(smApp->useQtBuiltinEglfs())
                env.insert("QT_QPA_PLATFORM", "eglfs");
            else
                env.insert("QT_QPA_PLATFORM", "hollywood-eglfs");

            // Apple GPU doesn't support hardware cursors
            if(smApp->isAsahiKernel())
                env.insert("QT_QPA_EGLFS_SWCURSOR", "1");
        }
        env.insert("QT_QPA_NO_SIGNAL_HANDLER", "1");
    }
    else
    {
        env.remove("QT_QPA_PLATFORM");
        env.insert("QT_QPA_PLATFORM", "wayland");
    }

    // exclude items not wayland dependent
    if(m_process != DBusSession &&
       m_process != Compositor)
    {
        // TODO: proper detection of wayland display
        env.insert("WAYLAND_DISPLAY", "wayland-0");
        env.insert("DISPLAY", ":0");

        env.insert("DBUS_SESSION_BUS_ADDRESS", QLatin1String(smApp->dbusSocket()));
    }

    // setup specific processes with qt-shell
    /*if(m_process == Elevator ||
       m_process == Shell ||
       m_process == NetworkAgent)
        env.insert("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell"); */

    // setup stage with layer-shell and twilight
    if(m_process == Stage)
    {
        env.insert("QT_WAYLAND_SHELL_INTEGRATION", "hw-layer-shell");
        env.insert("HW_TWILIGHT_SHELL", "1");
#ifdef QT_DEBUG
        env.insert("LD_LIBRARY_PATH", smApp->applicationDirPath().toUtf8());
#else
        env.insert("LD_LIBRARY_PATH", "/usr/lib/qt6/plugins/wayland-shell-integration/");
#endif
    }

    if(m_process == NotificationDaemon)
        env.insert("HW_TWILIGHT_SHELL", "1");

    setProcessEnvironment(env);
}

void ManagedProcess::setArgumentsForProcess()
{
    if(m_process == DBusSession)
    {
        auto args = QStringList() << "--session" << "--print-address";
        setArguments(args);
        return;
    }
    if(m_process == PipewirePulse)
    {
        auto args = QStringList() << "-c" << "pipewire-pulse.conf";
        setArguments(args);
        return;
    }
}

void ManagedProcess::logInfo(const QString &msg)
{
    switch(m_process)
    {
    case DBusSession:
        qCInfo(lcDBus) << msg;
	break;
    case Pipewire:
        qCInfo(lcPipewire) << msg;
	break;
    case Wireplumber:
        qCInfo(lcWireplumber) << msg;
	break;
    case PipewirePulse:
        qCInfo(lcPipewirePulse) << msg;
	break;
    case Compositor:
        qCInfo(lcCompositor) << msg;
	break;
    case Elevator:
        qCInfo(lcElevator) << msg;
	break;
    case Stage:
        qCInfo(lcStage) << msg;
	break;
    case NotificationDaemon:
        qCInfo(lcNotification) << msg;
    break;
    case Shell:
        qCInfo(lcDesktop) << msg;
	break;
    default:
        qCInfo(lsOther) << msg;
    }
}

bool ManagedProcess::checkForExistingDBusSession() const
{
   return false;
    if(!qgetenv("DBUS_SESSION_BUS_ADDRESS").isEmpty())
        return true;

    return false;
}

bool ManagedProcess::watchForCompositor()
{
    if(isOpen())
    {
        logInfo(QString("process started (pid %1) - waiting for wayland socket...")
                   .arg(processId()));
        sleep(1);
        uint i = 1;
        uint attempts = 10;
        auto socket = smApp->expectedCompositorSocket();
        for(i = 1; i <= attempts; ++i)
        {
            if(verifyCompositorSocket())
            {
                qCInfo(lcCompositor) << QString("Compositor socket %1 available.")
                           .arg(socket);
                return true;
            }
            else
            {
                qCWarning(lcCompositor) << QString("Compositor socket %1 not available (check %2).")
                           .arg(socket, i);
            }
            sleep(2);
        }
        qCCritical(lcCompositor) << QString("Compositor socket %1 not available after %2 attempts.  Giving up.")
                   .arg(socket, i--);
    }
    return false;
}

bool ManagedProcess::verifyCompositorSocket() const
{
    QLocalSocket socket;
    socket.setServerName(smApp->expectedCompositorSocket());
    socket.connectToServer();
    if(socket.isValid())
    {
        socket.disconnectFromServer();
        return true;
    }
    socket.disconnectFromServer();

    return false;
}
