#include "session.h"
#include <hollywood/hollywood.h>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QLocalServer>
#include <signal.h>
#include <QDBusConnection>

#include "dbus.h"

void handleShutDownSignal(int /* signalId */)
{
    return;
}

SMApplication::SMApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("SessionManager");
}

bool SMApplication::startSession()
{
    auto sp = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if(!sp.isEmpty())
    {
        auto hd = sp.first().toUtf8().data();
        chdir(hd);
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
        qCritical() << "SM: Unable to verify XDG_RUNTIME_DIR. Exiting.";
        return false;
    }

    auto xdg_runtime_sock = QString("%1/hollywood.sock").arg(m_xdg_runtime_dir);
    QLocalSocket* socket = new QLocalSocket();
    socket->connectToServer(xdg_runtime_sock);
    bool b_connected = socket->waitForConnected(500);
    socket->abort();
    delete socket;
    if (b_connected) {
       qCritical() <<  QCoreApplication::translate("Session", "Another Hollywood session is running. This instance is aborting");
       return false;
    }

    m_socket = new QLocalServer(this);
    m_socket->removeServer(xdg_runtime_sock);
    m_socket->listen(xdg_runtime_sock);

    qDebug() << "XDG Runtime DIR: " << m_xdg_runtime_dir;
    if(!startCompositor())
    {
        qCritical() << "SessionManager: Unable to launch compositor process";
        return false;
    }
    else
    {
        qputenv("QT_QPA_PLATFORM", "hollywood");
        qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
        qputenv("DESKTOP_SESSION", "Hollywood");

        qputenv("XDG_CURRENT_DESKTOP", "Hollywood");
        qputenv("XDG_SESSION_TYPE", "wayland");
        qputenv("XDG_MENU_PREFIX", "hollywood-");
        if(!startElevator())
        {
            qCritical() << "SessionManager: Unable to launch elevator process";
            if(m_compProcess->isOpen())
                m_compProcess->kill();

            return false;
        }

        /*if(!startMenuServer())
        {
            qCritical() << "SessionManager: Unable to launch menuserver process";
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();

            return false;
        }*/

        if(!startStage())
        {
            qCritical() << "SessionManager: Unable to launch stage process";
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();
            if(m_menuProcess->isOpen())
                m_menuProcess->kill();

            return false;
        }

        sleep(1);
        if(!startDesktop())
        {
            qCritical() << "SessionManager: Unable to launch desktop process";
            if(m_compProcess->isOpen())
                m_compProcess->kill();
            if(m_elevatorProcess->isOpen())
                m_elevatorProcess->kill();
            if(m_menuProcess->isOpen())
                m_menuProcess->kill();
            if(m_stageProcess->isOpen())
                m_stageProcess->kill();

            return false;
        }
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
    stopSession();
    int ret = 0;
    qApp->exit(ret);
}

void SMApplication::reboot()
{
    stopSession();
    int ret = 0;
    qApp->exit(ret);
}

bool SMApplication::canShutdown()
{
    return false;
}

bool SMApplication::canReboot()
{
    return false;
}

bool SMApplication::startCompositor()
{
    if(m_compProcess)
    {
        if(m_compProcess->state() != QProcess::Running)
        {
            qInfo() << QString("SMApplication::startCompositor: reaping old compositor (pid %1)")
                       .arg(m_compProcess->processId());
            delete m_compProcess;
        }
        else
            return false;
    }

    QProcessEnvironment env = createCompositorEnvironment();
    m_compProcess = new QProcess(this);
    m_compProcess->setObjectName("WindowCompositor");
    const QString processPath = applicationDirPath() + "/compositor";
    m_compProcess->setProgram(processPath);
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    if(qgetenv("DISPLAY").isEmpty())
    {
        qputenv("QT_QPA_PLATFORM", "hollywood-eglfs");
        qInfo() << QString("SMApplication::startCompositor: starting compositor with hollywood-eglfs");
    }
    qputenv("QT_QPA_NO_SIGNAL_HANDLER", "1");

    // TODO: fix this bold assumption
    m_compSocket = QString("%1/wayland-0").arg(m_xdg_runtime_dir);
    //m_compProcess->setProcessEnvironment(createCompositorEnvironment());
    connect(m_compProcess, &QProcess::finished, this, &SMApplication::compositorStopped);
    connect(m_compProcess, &QProcess::errorOccurred, this, &SMApplication::compositorStopped);
    connect(m_compProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_compProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_compProcess->start();
    if(m_compProcess->isOpen())
    {
        qInfo() << QString("SessionManager: Started WindowCompositor (pid %1) - waiting for socket...")
                   .arg(m_compProcess->processId());
        sleep(1);
        int i = 1;
        for(i = 1; i <= 5; ++i)
        {
            if(verifyCompositorSocketAvailable())
            {
                qInfo() << QString("SessionManager: Compositor socket %1 available.")
                           .arg(m_compSocket);
                return true;
            }
            else
            {
                qWarning() << QString("SessionManager: Compositor socket %1 not available (check %2).")
                           .arg(m_compSocket).arg(i);
            }
            sleep(2);
        }
        qCritical() << QString("SessionManager: Compositor socket %1 not available after %2 attempts.  Giving up.")
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

QProcessEnvironment SMApplication::createCompositorEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if(env.contains("QT_QPA_PLATFORM"))
        env.remove("QT_QPA_PLATFORM");

    env.insert("QT_QPA_PLATFORM", "wayland");

    return env;
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
    const QString senderProc = sender()->objectName();
    qWarning() << QString("%1 (stderr): %2")
                  .arg(senderProc).arg(m_compProcess->readAllStandardError());
}

void SMApplication::processStdOut()
{
    const QString senderProc = sender()->objectName();
    qInfo() << QString("%1 (stdout): %2")
                  .arg(senderProc).arg(m_compProcess->readAllStandardOutput());
}

bool SMApplication::startMenuServer()
{
    //return true;
    if(m_menuProcess)
    {
        if(m_menuProcess->state() != QProcess::Running)
        {
            qInfo() << QString("SMApplication::startMenuServer: reaping old MenuServer (pid %1)")
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

    //m_desktopProcess->setProcessEnvironment(createDesktopEnvironment());
    connect(m_menuProcess, &QProcess::finished, this, &SMApplication::menuServerStopped);
    connect(m_menuProcess, &QProcess::errorOccurred, this, &SMApplication::menuServerStopped);
    connect(m_menuProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_menuProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_menuProcess->start();
    if(m_menuProcess->isOpen())
    {
        qInfo() << QString("SessionManager: Started MenuServer (pid %1)").arg(m_menuProcess->processId());
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
            qInfo() << QString("SMApplication::startMenuServer: reaping old Elevator (pid %1)")
                       .arg(m_elevatorProcess->processId());
            delete m_elevatorProcess;
        }
        else
            return false;
    }

    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "qt-shell");
    m_elevatorProcess = new QProcess(this);
    m_elevatorProcess->setObjectName("Elevator");
    const QString processPath = applicationDirPath() + "/elevator";
    m_elevatorProcess->setProgram(processPath);
    qputenv("LD_LIBRARY_PATH", applicationDirPath().toUtf8());

    connect(m_elevatorProcess, &QProcess::finished, this, &SMApplication::elevatorStopped);
    connect(m_elevatorProcess, &QProcess::errorOccurred, this, &SMApplication::elevatorStopped);
    connect(m_elevatorProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_elevatorProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_elevatorProcess->start();
    if(m_elevatorProcess->isOpen())
    {
        qInfo() << QString("SessionManager: Started Elevator (pid %1)").arg(m_elevatorProcess->processId());
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

// ===========================================================
// ======            DESKTOP HOST FUNCTIONS             ======
// ===========================================================

bool SMApplication::startDesktop()
{
    if(m_desktopProcess)
    {
        if(m_desktopProcess->state() != QProcess::Running)
        {
            qInfo() << QString("SMApplication::startDesktop: reaping old desktop (pid %1)")
                       .arg(m_desktopProcess->processId());
            delete m_desktopProcess;
        }
        else
            return false;
    }

    m_desktopProcess = new QProcess(this);
    m_desktopProcess->setObjectName("Desktop");
#ifdef QT_DEBUG
    const QString processPath = applicationDirPath() + "/shellfm";
#else
    const QString processPath = "/usr/bin/shellfm";
#endif
    m_desktopProcess->setProgram(processPath);
    QStringList args;
    args << "--desktop";

    m_desktopProcess->setArguments(args);
    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "xdg-shell");
    qputenv("LD_LIBRARY_PATH", applicationDirPath().toUtf8());
    //m_desktopProcess->setProcessEnvironment(createDesktopEnvironment());
    connect(m_desktopProcess, &QProcess::finished, this, &SMApplication::desktopStopped);
    connect(m_desktopProcess, &QProcess::errorOccurred, this, &SMApplication::desktopStopped);
    connect(m_desktopProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_desktopProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_desktopProcess->start();
    if(m_desktopProcess->isOpen())
    {
        qInfo() << QString("SessionManager: Started Desktop (pid %1)").arg(m_desktopProcess->processId());
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
            qInfo() << QString("SMApplication::startMenuServer: reaping old Stage (pid %1)")
                       .arg(m_stageProcess->processId());
            delete m_stageProcess;
        }
        else
            return false;
    }

    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "hw-layer-shell");
    m_stageProcess = new QProcess(this);
    m_stageProcess->setObjectName("Stage");
    const QString processPath = applicationDirPath() + "/stage";
    m_stageProcess->setProgram(processPath);
    qputenv("LD_LIBRARY_PATH", applicationDirPath().toUtf8());

    //m_desktopProcess->setProcessEnvironment(createDesktopEnvironment());
    connect(m_stageProcess, &QProcess::finished, this, &SMApplication::menuServerStopped);
    connect(m_stageProcess, &QProcess::errorOccurred, this, &SMApplication::menuServerStopped);
    connect(m_stageProcess, &QProcess::readyReadStandardError, this, &SMApplication::processStdError);
    connect(m_stageProcess, &QProcess::readyReadStandardOutput, this, &SMApplication::processStdOut);

    m_stageProcess->start();
    if(m_stageProcess->isOpen())
    {
        qInfo() << QString("SessionManager: Started Stage (pid %1)").arg(m_stageProcess->processId());
        return true;
    }
    return false;
}

QProcessEnvironment SMApplication::createDesktopEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if(env.contains("QT_QPA_PLATFORM"))
        env.remove("QT_QPA_PLATFORM");

    env.insert("QT_QPA_PLATFORM", "hollywood");
    env.insert("QT_QPA_PLATFORMTHEME", "hollywood");
    return env;
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
                qCritical() << "Can not get or create a functional XDG_RUNTIME_DIR.  Can you write to /tmp? Bailing out.";
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
            qCritical() << "Can not read & write to env XDG_RUNTIME_DIR" << dir << "Check permissions. Bailing out.";
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

    m_socket->close();
    return true;
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

    p.addOptions({
     {{"E", "no-elevator"},
         QCoreApplication::translate("main", "Do not launch the elevator (PolicyKit Agent) with this session.")},
     {{"A", "no-notifications"},
         QCoreApplication::translate("main", "Do not launch the notification daemon with this session.")},
    });
    p.process(a);

    if(!a.startSession())
        return 1;

    return a.exec();
}
