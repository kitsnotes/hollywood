#include "session.h"
#include <hollywood/hollywood.h>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QLocalServer>
#include <signal.h>
#include <QDBusConnection>
#include <QLoggingCategory>
#include <QDBusInterface>
#include <QApplication>

#include <sys/types.h>
#include <sys/stat.h>

#include "dbus.h"

#define LOGIND_SERVICE    "org.freedesktop.login1"
#define LOGIND_PATH       "/org/freedesktop/login1"
#define LOGIND_INTERFACE  "org.freedesktop.login1.Manager"

QLoggingCategory lcSession("hollywood.session");
QLoggingCategory lcCompositor("hollywood.compositor");
QLoggingCategory lcStage("hollywood.stage");
QLoggingCategory lcDesktop("hollywood.shell");
QLoggingCategory lcMenuServer("hollywood.shell");
QLoggingCategory lcElevator("hollywood.elevator");
QLoggingCategory lcWireplumber("wireplumber");
QLoggingCategory lcPipewire("pipewire");
QLoggingCategory lcPipewirePulse("pipewire.pulse");

QString g_logfile;

void handleShutDownSignal(int /* signalId */)
{
    return;
}

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(type)
    QFile outFile(g_logfile);
    if(outFile.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        QTextStream s(&outFile);
        s << context.category << msg << "\n";
        outFile.close();
    }
}

SMApplication::SMApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("SessionManager");
    qputenv("DESKTOP_SESSION", "Hollywood");

    qputenv("XDG_CURRENT_DESKTOP", "Hollywood");
    qputenv("XDG_SESSION_TYPE", "wayland");
    qputenv("XDG_MENU_PREFIX", "hollywood-");

    // see if we are started by a display manager

    auto ppid = getppid();

    QFile f(QLatin1String("/proc/%1/cmdline").arg(QString::number(ppid)));
    if(f.exists())
    {
        struct stat statbuf;
        if(stat(f.fileName().toUtf8().data(), &statbuf) != -1)
        {
            if(statbuf.st_uid == 0)
                m_startedDisplayManager = true;
        }
    }
    qCInfo(lcSession) << "Starting new session.";
    if(m_startedDisplayManager)
        qCInfo(lcSession) << "Session started via display manager.";

}

bool SMApplication::startSession()
{
    auto sp = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if(!sp.isEmpty())
    {
        auto hd = sp.first().toUtf8().data();
        int res = chdir(hd);
        Q_UNUSED(res);
    }
    auto def_xdg_config = sp.first() + "/.config/";
    auto env_xdg_config = qgetenv("XDG_CONFIG_DIR");

    auto originull_config = QString("%1/originull/hollywood.conf").arg(env_xdg_config.isEmpty() ? def_xdg_config : env_xdg_config);
    QFile f(originull_config);
    if(!f.exists())
    {
        f.open(QFile::WriteOnly);
        f.write("");
        f.close();
    }

    if(!verifyXdgRuntime())
    {
        qCCritical(lcSession) << "Unable to verify XDG_RUNTIME_DIR. Exiting.";
        return false;
    }

    auto xdg_runtime_sock = QString("%1/hollywood.sock").arg(m_xdg_runtime_dir);
    QLocalSocket* socket = new QLocalSocket();
    socket->connectToServer(xdg_runtime_sock);
    bool b_connected = socket->waitForConnected(500);
    socket->abort();
    delete socket;
    if (b_connected) {
       qCCritical(lcSession) <<  QCoreApplication::translate("Session", "Another Hollywood session is running. This instance is aborting");
       return false;
    }

    m_socket = new QLocalServer(this);
    m_socket->removeServer(xdg_runtime_sock);
    m_socket->listen(xdg_runtime_sock);

    if(m_useDBus && !startDBusSession())
    {
        qCWarning(lcSession) << "Unable to launch dbus-daemon process. The desktop experience may be degraded.";
        if(m_dbusSessionProcess->isOpen())
            m_dbusSessionProcess->kill();
    }

    if(!startCompositor())
    {
        qCCritical(lcSession) << "Unable to launch compositor process";
        return false;
    }
    else
    {
        // TODO: proper detection of wayland display
        qputenv("WAYLAND_DISPLAY", "wayland-0");
        qputenv("DISPLAY", ":0");

        if(m_usePipewire && !startPipewire())
        {
            qCCritical(lcSession) << "Unable to launch pipewire process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();

            return false;
        }

        if(m_useElevator && !startElevator())
        {
            qCCritical(lcSession) << "Unable to launch elevator process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            return false;
        }

        if(m_useDBus)
        {
            int count = 0;
            for(;;)
            {
                QApplication::processEvents();
                if(!m_dbusSessionVar.isNull())
                    break;
                sleep(1);
                count++;

                if(count > 10)
                    break;
            }
        }

        if(m_usePipewire && !startWireplumber())
        {
            qCCritical(lcSession) << "Unable to launch wireplumber process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();
            if(m_pipewireProcess->isOpen())
                m_pipewireProcess->kill();

            return false;
        }

        /* if(m_usePipewire && !startPipewirePulse())
        {
            qCCritical(lcSession) << "Unable to launch pipewire (pulse emulation) process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_pipewireProcess->isOpen())
                m_pipewireProcess->kill();
            if(m_wireplumberProcess->isOpen())
                m_wireplumberProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();

            return false;
        }*/

        /*if(!startMenuServer())
        {
            qCCritical(lcSession) << "Unable to launch menuserver process";
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();

            return false;
        }*/

        if(!startDesktop())
        {
            qCCritical(lcSession) << "Unable to launch desktop process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_pipewireProcess->isOpen())
                m_pipewireProcess->kill();
            if(m_wireplumberProcess->isOpen())
                m_wireplumberProcess->kill();
            if(m_pipewirePulseProcess->isOpen())
                m_pipewirePulseProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();
            if(m_menuProcess->isOpen())
                m_menuProcess->kill();


            return false;
        }

        usleep(300*1000);
        if(!startStage())
        {
            qCCritical(lcSession) << "Unable to launch stage process";
            if(m_dbusSessionProcess->isOpen())
                m_dbusSessionProcess->kill();
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_pipewireProcess->isOpen())
                m_pipewireProcess->kill();
            if(m_wireplumberProcess->isOpen())
                m_wireplumberProcess->kill();
            if(m_pipewirePulseProcess->isOpen())
                m_pipewirePulseProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();
            if(m_menuProcess->isOpen())
                m_menuProcess->kill();
            if(m_desktopProcess->isOpen())
                m_desktopProcess->kill();
            return false;
        }

        startPipewirePulse();

        m_sessionStarted = true;
        m_dbus = new SessionDBus(this);
        QDBusConnection::sessionBus().registerService(QLatin1String(HOLLYWOOD_SESSION_DBUS));
        QDBusConnection::sessionBus().registerObject(QLatin1String(HOLLYWOOD_SESSION_DBUSOBJ), this);

        return true;
    }

    return false;
}

void SMApplication::logout()
{
    stopSession();
    int ret = 0;
    qApp->exit(ret);
}

void SMApplication::shutdown()
{
    disconnectProcessWatchers();
    callLogindDbus("PowerOff");
}

void SMApplication::reboot()
{
    disconnectProcessWatchers();
    callLogindDbus("Reboot");
}

bool SMApplication::canShutdown()
{
    return callLogindDbus("CanShutdown");
}

bool SMApplication::canReboot()
{
    return callLogindDbus("CanReboot");
}

bool SMApplication::haveSDDMAutoStart()
{
    // TODO: this

    return false;
}

bool SMApplication::startCompositor()
{
    if(m_compProcess)
    {
        if(m_compProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startCompositor: reaping old compositor (pid %1)")
                       .arg(m_compProcess->processId());
            delete m_compProcess;
        }
        else
            return false;
    }

    m_compProcess = new QProcess(this);
    m_compProcess->setObjectName("WindowCompositor");
    const QString processPath = applicationDirPath() + "/compositor";
    m_compProcess->setProgram(processPath);
    auto env = QProcessEnvironment::systemEnvironment();
    env.remove("QT_QPA_PLATFORMTHEME");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
#ifdef QT_DEBUG
    env.insert("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
#endif
    if(!env.contains("DISPLAY"))
    {
        env.remove("QT_QPA_PLATFORM");
        env.insert("QT_QPA_PLATFORM", "hollywood-eglfs");
        qCInfo(lcSession) << QString("Starting compositor with hollywood-eglfs");
    }
    else
    {
        qCInfo(lcSession) << QString("Starting nested compositor session (for debugging)");
    }
    env.insert("QT_QPA_NO_SIGNAL_HANDLER", "1");

    // TODO: fix this bold assumption
    m_compSocket = QString("%1/wayland-0").arg(m_xdg_runtime_dir);
    m_compProcess->setProcessEnvironment(env);
    connect(m_compProcess, &QProcess::finished, this, &SMApplication::compositorStopped);
    connect(m_compProcess, &QProcess::errorOccurred, this, &SMApplication::compositorStopped);
    connect(m_compProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_compProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_compProcess->start();
    if(m_compProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started WindowCompositor (pid %1) - waiting for socket...")
                   .arg(m_compProcess->processId());
        sleep(1);
        int i = 1;
        for(i = 1; i <= 5; ++i)
        {
            if(verifyCompositorSocketAvailable())
            {
                qCInfo(lcSession) << QString("Compositor socket %1 available.")
                           .arg(m_compSocket);
                return true;
            }
            else
            {
                qCWarning(lcSession) << QString("Compositor socket %1 not available (check %2).")
                           .arg(m_compSocket).arg(i);
            }
            sleep(2);
        }
        qCCritical(lcSession) << QString("Compositor socket %1 not available after %2 attempts.  Giving up.")
                   .arg(m_compSocket).arg(i--);
    }
    return false;
}

bool SMApplication::verifyCompositorSocketAvailable()
{
    QLocalSocket socket(this);
    socket.setServerName(m_compSocket);
    socket.connectToServer();
    if(socket.isValid())
    {
        socket.disconnectFromServer();
        return true;
    }
    socket.disconnectFromServer();

    return false;
}

bool SMApplication::startDBusSession()
{
    if(!qgetenv("DBUS_SESSION_BUS_ADDRESS").isEmpty())
    {
        qCInfo(lcSession) << QString("Not starting a user DBUS session as one already exists.");
        return true;
    }

    if(m_dbusSessionProcess)
    {
        if(m_dbusSessionProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startDBusSession: reaping old dbus-session (pid %1)")
                       .arg(m_dbusSessionProcess->processId());
            delete m_dbusSessionProcess;
        }
        else
            return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    m_dbusSessionProcess = new QProcess(this);
    m_dbusSessionProcess->setObjectName("DBus-Session");
    const QString processPath = "/usr/bin/dbus-daemon";
    m_dbusSessionProcess->setProgram(processPath);
    m_dbusSessionProcess->setProcessEnvironment(env);

    auto args = QStringList() << "--session" << "--print-address";
    m_dbusSessionProcess->setArguments(args);

    connect(m_dbusSessionProcess, &QProcess::finished, this, &SMApplication::dbusProcessStopped);
    connect(m_dbusSessionProcess, &QProcess::errorOccurred, this, &SMApplication::dbusProcessStopped);
    connect(m_dbusSessionProcess, &QProcess::readyReadStandardError, this, &SMApplication::dbusStdOut);
    connect(m_dbusSessionProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::dbusStdOut);

    m_dbusSessionProcess->start();
    if(m_dbusSessionProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started dbus-launch (pid %1)").arg(m_dbusSessionProcess->processId());
        return true;
    }
    return false;
}

void SMApplication::compositorStopped()
{
#ifdef QT_DEBUG
    qApp->quit();
    return;
#endif
    if(m_compProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: WindowCompositor (pid %1) stopped (return %2). Restarting...")
                  .arg(m_compProcess->processId())
                  .arg(m_compProcess->exitCode());

    startCompositor();
}

void SMApplication::processStdError()
{
    auto proc = qobject_cast<QProcess*>(sender());
    const QString senderProc = sender()->objectName();
    auto lines = proc->readAllStandardError().split('\n');

    if(proc == m_pipewireProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcPipewire) << line;
    }
    if(proc == m_pipewirePulseProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcPipewirePulse) << line;
    }
    if(proc == m_wireplumberProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcWireplumber) << line;
    }
    if(proc == m_elevatorProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcElevator) << line;
    }
    if(proc == m_menuProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcMenuServer) << line;
    }
    if(proc == m_compProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcCompositor) << line;
    }
    if(proc == m_stageProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcStage) << line;
    }
    if(proc == m_desktopProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCWarning(lcDesktop) << line;
    }
}

void SMApplication::dbusStdOut()
{
    auto proc = qobject_cast<QProcess*>(sender());
    auto lines = proc->readAllStandardOutput().split('\n');

    for(auto &l : lines)
    {
        if(l.startsWith("unix:"))
        {
            // we have our dbus id
            qCInfo(lcSession) << "setting DBUS_SESSION_BUS_ADDRESS" << l;
            qputenv("DBUS_SESSION_BUS_ADDRESS", l);
            m_dbusSessionVar = l;
        }
    }
}

void SMApplication::processStdOut()
{
    auto proc = qobject_cast<QProcess*>(sender());
    const QString senderProc = sender()->objectName();
    auto lines = proc->readAllStandardOutput().split('\n');

    if(proc == m_pipewireProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcPipewire) << line;
    }
    if(proc == m_pipewirePulseProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcPipewirePulse) << line;
    }
    if(proc == m_wireplumberProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcWireplumber) << line;
    }

    if(proc == m_elevatorProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcElevator) << line;
    }
    if(proc == m_menuProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcMenuServer) << line;
    }
    if(proc == m_compProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcCompositor) << line;
    }
    if(proc == m_stageProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcStage) << line;
    }
    if(proc == m_desktopProcess)
    {
        for(auto line : lines)
            if(!line.isEmpty())
                qCInfo(lcDesktop) << line;
    }
}

bool SMApplication::startMenuServer()
{
    //return true;
    if(m_menuProcess)
    {
        if(m_menuProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startMenuServer: reaping old MenuServer (pid %1)")
                       .arg(m_menuProcess->processId());
            delete m_menuProcess;
        }
        else
            return false;
    }

    m_menuProcess = new QProcess(this);
    m_menuProcess->setObjectName("MenuServer");
    const QString processPath = applicationDirPath() + "/menuserver";
    m_menuProcess->setProgram(processPath);
    qputenv("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
    //env.insert("QT_WAYLAND_SHELL_INTEGRATION", "hw-layer-shell");


    //m_desktopProcess->setProcessEnvironment(createDesktopEnvironment());
    connect(m_menuProcess, &QProcess::finished, this, &SMApplication::menuServerStopped);
    connect(m_menuProcess, &QProcess::errorOccurred, this, &SMApplication::menuServerStopped);
    connect(m_menuProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_menuProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_menuProcess->start();
    if(m_menuProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started MenuServer (pid %1)").arg(m_menuProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::startElevator()
{
    if(m_elevatorProcess)
    {
        if(m_elevatorProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startMenuServer: reaping old Elevator (pid %1)")
                       .arg(m_elevatorProcess->processId());
            delete m_elevatorProcess;
        }
        else
            return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "hollywood");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
    env.insert("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
#ifdef QT_DEBUG
    env.insert("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
#endif

    m_elevatorProcess = new QProcess(this);
    m_elevatorProcess->setObjectName("Elevator");
    const QString processPath = applicationDirPath() + "/elevator";
    m_elevatorProcess->setProgram(processPath);
    m_elevatorProcess->setProcessEnvironment(env);

    connect(m_elevatorProcess, &QProcess::finished, this, &SMApplication::elevatorStopped);
    connect(m_elevatorProcess, &QProcess::errorOccurred, this, &SMApplication::elevatorStopped);
    connect(m_elevatorProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_elevatorProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_elevatorProcess->start();
    if(m_elevatorProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started Elevator (pid %1)").arg(m_elevatorProcess->processId());
        return true;
    }
    return false;
}

void SMApplication::menuServerStopped()
{
    if(m_menuProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: MenuServer process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_menuProcess->processId())
                  .arg(m_menuProcess->exitCode());

    startMenuServer();
}

void SMApplication::elevatorStopped()
{
    if(m_elevatorProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: Elevator process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_elevatorProcess->processId())
                  .arg(m_elevatorProcess->exitCode());

    startElevator();
}

void SMApplication::stageStopped()
{
    if(m_stageProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: Stage process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_stageProcess->processId())
                  .arg(m_stageProcess->exitCode());

    startStage();
}

void SMApplication::dbusProcessStopped()
{
    if(m_dbusSessionProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: dbus-launch process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_dbusSessionProcess->processId())
                  .arg(m_dbusSessionProcess->exitCode());

    startDBusSession();
}

void SMApplication::pipewireStopped()
{
    if(m_pipewireProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: pipewire process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_pipewireProcess->processId())
                  .arg(m_pipewireProcess->exitCode());

    startPipewire();
}

void SMApplication::wireplumberStopped()
{
    if(m_wireplumberProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: wireplumber process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_wireplumberProcess->processId())
                  .arg(m_wireplumberProcess->exitCode());

    startWireplumber();
}

void SMApplication::pipewirePulseStopped()
{
    if(m_pipewirePulseProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: pipewire (pulse emulation) process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_pipewirePulseProcess->processId())
                  .arg(m_pipewirePulseProcess->exitCode());

    startPipewirePulse();
}

// ===========================================================
// ======            DESKTOP HOST FUNCTIONS             ======
// ===========================================================

bool SMApplication::startDesktop()
{
    if(m_desktopProcess)
    {
        if(m_desktopProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startDesktop: reaping old desktop (pid %1)")
                       .arg(m_desktopProcess->processId());
            delete m_desktopProcess;
        }
        else
            return false;
    }

    m_desktopProcess = new QProcess(this);
    m_desktopProcess->setObjectName("Desktop");
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "hollywood");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
    env.insert("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
#ifdef QT_DEBUG
    const QString processPath = applicationDirPath() + "/shellfm";
    env.insert("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
#else
    const QString processPath = "/usr/bin/shellfm";
#endif
    m_desktopProcess->setProgram(processPath);
    QStringList args;
    args << "--desktop";

    m_desktopProcess->setArguments(args);
    m_desktopProcess->setProcessEnvironment(env);
    connect(m_desktopProcess, &QProcess::finished, this, &SMApplication::desktopStopped);
    connect(m_desktopProcess, &QProcess::errorOccurred, this, &SMApplication::desktopStopped);
    connect(m_desktopProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_desktopProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_desktopProcess->start();
    if(m_desktopProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started Desktop (pid %1)").arg(m_desktopProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::startStage()
{
    if(m_stageProcess)
    {
        if(m_stageProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("SMApplication::startMenuServer: reaping old Stage (pid %1)")
                       .arg(m_stageProcess->processId());
            delete m_stageProcess;
        }
        else
            return false;
    }

    m_stageProcess = new QProcess(this);
    m_stageProcess->setObjectName("Stage");
    const QString processPath = applicationDirPath() + "/stage";
    m_stageProcess->setProgram(processPath);
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_QPA_PLATFORM", "hollywood");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
    env.insert("QT_WAYLAND_SHELL_INTEGRATION", "hw-layer-shell");
    env.insert("HW_TWILIGHT_SHELL", "1");
#ifdef QT_DEBUG
    env.insert("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
#else
    env.insert("LD_LIBRARY_PATH", "/usr/lib/qt6/plugins/wayland-shell-integration/");
#endif
    m_stageProcess->setProcessEnvironment(env);
    connect(m_stageProcess, &QProcess::finished, this, &SMApplication::stageStopped);
    connect(m_stageProcess, &QProcess::errorOccurred, this, &SMApplication::stageStopped);
    connect(m_stageProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_stageProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_stageProcess->start();
    if(m_stageProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started Stage (pid %1)").arg(m_stageProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::startPipewire()
{
    if(m_pipewireProcess)
    {
        if(m_pipewireProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("startPipewire: reaping old pipewire (pid %1)")
                       .arg(m_pipewireProcess->processId());
            delete m_pipewireProcess;
        }
        else
            return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    m_pipewireProcess = new QProcess(this);
    m_pipewireProcess->setObjectName("pipewire");
    const QString processPath = "/usr/bin/pipewire";
    m_pipewireProcess->setProgram(processPath);
    m_pipewireProcess->setProcessEnvironment(env);

    connect(m_pipewireProcess, &QProcess::finished, this, &SMApplication::pipewireStopped);
    connect(m_pipewireProcess, &QProcess::errorOccurred, this, &SMApplication::pipewireStopped);
    connect(m_pipewireProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_pipewireProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_pipewireProcess->start();
    if(m_pipewireProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started pipewire (pid %1)").arg(m_pipewireProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::startWireplumber()
{
    if(m_wireplumberProcess)
    {
        if(m_wireplumberProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("startWireplumber: reaping old wireplumber (pid %1)")
                       .arg(m_wireplumberProcess->processId());
            delete m_wireplumberProcess;
        }
        else
            return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    m_wireplumberProcess = new QProcess(this);
    m_wireplumberProcess->setObjectName("wireplumber");
    const QString processPath = "/usr/bin/wireplumber";
    m_wireplumberProcess->setProgram(processPath);
    m_wireplumberProcess->setProcessEnvironment(env);

    connect(m_wireplumberProcess, &QProcess::finished, this, &SMApplication::wireplumberStopped);
    connect(m_wireplumberProcess, &QProcess::errorOccurred, this, &SMApplication::wireplumberStopped);
    connect(m_wireplumberProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_wireplumberProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_wireplumberProcess->start();
    if(m_wireplumberProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started wireplumber (pid %1)").arg(m_wireplumberProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::startPipewirePulse()
{
    if(m_pipewirePulseProcess)
    {
        if(m_pipewirePulseProcess->state() != QProcess::Running)
        {
            qCInfo(lcSession) << QString("startPipewire: reaping old pipewire (pulse emulation) (pid %1)")
                       .arg(m_pipewirePulseProcess->processId());
            delete m_pipewirePulseProcess;
        }
        else
            return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    m_pipewirePulseProcess = new QProcess(this);
    m_pipewirePulseProcess->setObjectName("pipewire-pulse");
    const QString processPath = "/usr/bin/pipewire";

    m_pipewirePulseProcess->setProgram(processPath);
    QStringList args;
    args << "--c" << "pipewire-pulse.conf";

    m_desktopProcess->setArguments(args);
    m_pipewirePulseProcess->setProcessEnvironment(env);

    connect(m_pipewirePulseProcess, &QProcess::finished, this, &SMApplication::pipewirePulseStopped);
    connect(m_pipewirePulseProcess, &QProcess::errorOccurred, this, &SMApplication::pipewirePulseStopped);
    connect(m_pipewirePulseProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_pipewirePulseProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_pipewirePulseProcess->start();
    if(m_pipewirePulseProcess->isOpen())
    {
        qCInfo(lcSession) << QString("SessionManager: Started pipewire (pulse emulation) (pid %1)").arg(m_pipewirePulseProcess->processId());
        return true;
    }
    return false;
}

bool SMApplication::verifyXdgRuntime()
{
    auto env = qgetenv("XDG_RUNTIME_DIR");
    if(env.isEmpty())
    {
        // check to see if we can use /run/user/$uid/ as our dir
        QFileInfo f(QString("/run/user/%1").arg(getuid()));
        if(f.isDir() && f.isReadable() && f.isWritable())
            m_xdg_runtime_dir = QString("/run/user/%1").arg(getuid());
        else
        {
            // ok no go - let's make a /tmp dir
            auto dir = QString("/tmp/xdg-runtime-%1").arg(getuid());
            QFile::Permissions perm = QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner;
            QDir qdir;
	    QFileInfo fi(dir);
            if((fi.isDir() && fi.isWritable()) || qdir.mkdir(dir, perm))
            {
                m_xdg_runtime_dir = dir;
            }
            else
            {
                qCCritical(lcSession) << "Can not get or create a functional XDG_RUNTIME_DIR.  Can you write to /tmp? Bailing out.";
                return false;
            }
        }
        qputenv("XDG_RUNTIME_DIR", m_xdg_runtime_dir.toUtf8().data());
    }
    else
    {
        // check to see if we can write to XDG_RUNTIME_DIR
        auto dir = qgetenv("XDG_RUNTIME_DIR");
        QFileInfo file(dir);
        if(file.isDir() && file.isReadable() && file.isWritable())
            m_xdg_runtime_dir = dir;
        else
        {
            qCCritical(lcSession) << "Can not read & write to env XDG_RUNTIME_DIR" << dir << "Check permissions. Bailing out.";
            return false;
        }
    }
    return true;
}

bool SMApplication::stopSession()
{
    // stop the desktop
    // stop the elevator
    if(m_desktopProcess)
    {
        disconnect(m_desktopProcess, &QProcess::finished, this, &SMApplication::desktopStopped);
        disconnect(m_desktopProcess, &QProcess::errorOccurred, this, &SMApplication::desktopStopped);
        m_desktopProcess->terminate();

        while(m_desktopProcess->waitForFinished(150))
        {
            if(m_desktopProcess->state() == QProcess::NotRunning)
                continue;
        }
        qDebug() << "terminated desktop";
    }


    // stop the elevator
    if(m_elevatorProcess)
    {
        disconnect(m_elevatorProcess, &QProcess::finished, this, &SMApplication::elevatorStopped);
        disconnect(m_elevatorProcess, &QProcess::errorOccurred, this, &SMApplication::elevatorStopped);
        m_elevatorProcess->terminate();
        while(m_elevatorProcess->waitForFinished(150))
        {
            if(m_elevatorProcess->state() == QProcess::NotRunning)
                continue;
        }
        qDebug() << "terminated elevator";
    }

    // insert more sub-processes HERE

    // stop the menuserver (2nd to last)
    if(m_menuProcess)
    {
        disconnect(m_menuProcess, &QProcess::finished, this, &SMApplication::menuServerStopped);
        disconnect(m_menuProcess, &QProcess::errorOccurred, this, &SMApplication::menuServerStopped);
        m_menuProcess->terminate();
        while(m_menuProcess->waitForFinished(150))
        {
            if(m_menuProcess->state() == QProcess::NotRunning)
                continue;
        }
        qDebug() << "terminated menuserver";
    }

    // stop the compostior (last)
    if(m_compProcess)
    {
        disconnect(m_compProcess, &QProcess::finished, this, &SMApplication::compositorStopped);
        disconnect(m_compProcess, &QProcess::errorOccurred, this, &SMApplication::compositorStopped);
        m_compProcess->kill();
        while(m_compProcess->waitForFinished(150))
        {
            if(m_compProcess->state() == QProcess::NotRunning)
                continue;
        }
        qDebug() << "terminated compositor";
    }


    m_pipewirePulseProcess->kill();
    m_wireplumberProcess->kill();
    m_pipewireProcess->kill();

    m_dbusSessionProcess->kill();

    m_socket->close();
    return true;
}

bool SMApplication::callLogindDbus(const QString &command)
{
    QDBusInterface ldb(QLatin1String(LOGIND_SERVICE), QLatin1String(LOGIND_PATH), QLatin1String(LOGIND_INTERFACE), QDBusConnection::systemBus());
    if(!ldb.isValid())
    {
        qCCritical(lcSession) << QString("Could not call elogind DBUS interface. Command: %1")
                   .arg(command);
        return false;
    }

    QDBusMessage msg = ldb.call(command, QVariant(false));
    qDebug() << msg.arguments().constFirst().toString();

    if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    auto response = msg.arguments().constFirst().toString();

    if(response == "yes")
        return true;

    return false;

}

void SMApplication::disconnectProcessWatchers()
{
    disconnect(m_elevatorProcess, &QProcess::finished, this, &SMApplication::elevatorStopped);
    disconnect(m_elevatorProcess, &QProcess::errorOccurred, this, &SMApplication::elevatorStopped);

    disconnect(m_desktopProcess, &QProcess::finished, this, &SMApplication::desktopStopped);
    disconnect(m_desktopProcess, &QProcess::errorOccurred, this, &SMApplication::desktopStopped);

    disconnect(m_stageProcess, &QProcess::finished, this, &SMApplication::stageStopped);
    disconnect(m_stageProcess, &QProcess::errorOccurred, this, &SMApplication::stageStopped);

    disconnect(m_compProcess, &QProcess::finished, this, &SMApplication::compositorStopped);
    disconnect(m_compProcess, &QProcess::errorOccurred, this, &SMApplication::compositorStopped);

    disconnect(m_pipewireProcess, &QProcess::finished, this, &SMApplication::pipewireStopped);
    disconnect(m_pipewireProcess, &QProcess::errorOccurred, this, &SMApplication::pipewireStopped);

    disconnect(m_pipewirePulseProcess, &QProcess::finished, this, &SMApplication::pipewirePulseStopped);
    disconnect(m_pipewirePulseProcess, &QProcess::errorOccurred, this, &SMApplication::pipewirePulseStopped);

    disconnect(m_wireplumberProcess, &QProcess::finished, this, &SMApplication::wireplumberStopped);
    disconnect(m_wireplumberProcess, &QProcess::errorOccurred, this, &SMApplication::wireplumberStopped);

}

void SMApplication::desktopStopped()
{
    if(m_desktopProcess->exitStatus() == QProcess::NormalExit)
        return;     // Standard system shutdown, so don't restart

    qWarning() << QString("SessionManager: Desktop process (pid %1) stopped (return %2). Restarting...")
                  .arg(m_desktopProcess->processId())
                  .arg(m_desktopProcess->exitCode());

    startDesktop();
}

int main(int argc, char *argv[])
{
    SMApplication a(argc, argv);
    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood Session Manager"));
    p.addHelpOption();
    p.addVersionOption();

    /*{{"E", "no-elevator"},
        QCoreApplication::translate("main", "Do not launch the elevator (PolicyKit Agent) with this session.")}, */

    p.addOptions({
     {"no-elevator",
         QCoreApplication::translate("main", "Do not launch the elevator (PolicyKit Agent) with this session.")},
     {"no-notifications",
         QCoreApplication::translate("main", "Do not launch the notification daemon with this session.")},
     {"no-pipewire",
         QCoreApplication::translate("main", "Do not launch pipewire daemon with this session.")},
     {"no-dbus",
         QCoreApplication::translate("main", "Do not launch a dbus-daemon for session bus with this session.")},
    });
    p.process(a);

    if(p.isSet("no-elevator"))
        a.setUseElevator(false);
    if(p.isSet("no-dbus"))
        a.setUseDBus(false);
    if(p.isSet("no-pipewire"))
        a.setUsePipewire(false);

    QString xdg = qgetenv("XDG_DATA_HOME");
    if(xdg.isEmpty())
        xdg = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/.local/share/";

    QDir dir(xdg);
    dir.mkpath(xdg+"/hollywood");

    g_logfile = QString("%1/hollywood/session-%2.log").arg(xdg,
           QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmm"));

    qInstallMessageHandler(messageHandler);

    if(!a.startSession())
        return 1;

    return a.exec();
}
