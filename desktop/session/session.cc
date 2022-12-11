// Hollywood Session Manager
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <QByteArray>
#include <executor.h>
#include <mimeapps.h>
#include <QMimeType>
#include <QMimeDatabase>
#include <desktopentry.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "dbus.h"
#include "process.h"

#define LOGIND_SERVICE    "org.freedesktop.login1"
#define LOGIND_PATH       "/org/freedesktop/login1"
#define LOGIND_INTERFACE  "org.freedesktop.login1.Manager"

QLoggingCategory lcSession("hollywood.session");

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
        s << context.category << " " << msg << "\n";
        outFile.close();
    }
}

SMApplication::SMApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
    , m_mime(new LSMimeApplications())
    , m_dbusSessionProcess(new ManagedProcess(ManagedProcess::DBusSession, this))
    , m_compositorProcess(new ManagedProcess(ManagedProcess::Compositor, this))
    , m_pipewireProcess(new ManagedProcess(ManagedProcess::Pipewire, this))
    , m_wireplumberProcess(new ManagedProcess(ManagedProcess::Wireplumber, this))
    , m_elevatorProcess(new ManagedProcess(ManagedProcess::Elevator, this))
    , m_stageProcess(new ManagedProcess(ManagedProcess::Stage, this))
    , m_desktopProcess(new ManagedProcess(ManagedProcess::Shell, this))
    , m_pipewirePulseProcess(new ManagedProcess(ManagedProcess::PipewirePulse, this))

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

    verifyTrashFolder();
    m_mime->processGlobalMimeCache();
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

    reloadLocaleSettings();

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

    connect(m_dbusSessionProcess, &ManagedProcess::dbusSessionBusAddressAvailable,
            this, &SMApplication::dbusAvailable);

    m_dbusSessionProcess->initialize();

    connect(m_compositorProcess, &ManagedProcess::compositorDied,
            this, &SMApplication::compositorDied);
    if(!m_compositorProcess->initialize())
    {
        qCCritical(lcSession) <<  QCoreApplication::translate("Session", "Could not launch the compositor instance. This session can not continue.");
        return false;
    }

    auto dbusenv = qgetenv("DBUS_SESSION_BUS_ADDRESS");
    if(!m_useDBus)
    {
        qCInfo(lcSession) << "Not supervising a dbus-session by user request.";
        startDBusReliantServices();
    }
    if(!dbusenv.isEmpty())
    {
        qCInfo(lcSession) << "Not supervising a dbus-session as one exists.";
        startDBusReliantServices();
    }
    return true;
}

void SMApplication::logout()
{
    m_doingShutdown = true;
    stopSession();
    int ret = 0;
    qApp->exit(ret);
}

void SMApplication::shutdown()
{
    m_doingShutdown = true;
    callLogindDbus("PowerOff");
}

void SMApplication::reboot()
{
    m_doingShutdown = true;
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

void SMApplication::restartSession()
{
    m_elevatorProcess->setAutoRestart(false);
    m_stageProcess->setAutoRestart(false);
    m_desktopProcess->setAutoRestart(false);

    terminateUserProcesses();

    m_elevatorProcess->terminate();
    m_stageProcess->terminate();
    m_desktopProcess->terminate();

    reloadLocaleSettings();
    restartCompositorReliantProcesses();
}

bool SMApplication::executeDesktop(const QString &desktop, const QStringList &arguments, const QStringList &env)
{
    // TODO: handle environment variables
    Q_UNUSED(env)
    LSExecutor *exec = new LSExecutor(this);
    m_userprocs.append(exec);

    connect(exec, &LSExecutor::terminated, this, &SMApplication::userProcessTerminated);
    exec->setDesktopFile(desktop);
    exec->setArguments(arguments);
    return exec->launch();
}

bool SMApplication::openFileWithDefault(const QString &file)
{
    QMimeDatabase mime;
    auto mt = mime.mimeTypeForFile(file);
    qDebug() << mt;
    auto dt = m_mime->defaultApp(mt.name());
    bool res = false;
    if(dt)
        res = executeDesktop(dt->fileName(), QStringList() << file, QStringList());

    return res;
}

bool SMApplication::haveSDDMAutoStart()
{
    // TODO: this

    return false;
}

QString SMApplication::expectedCompositorSocket() const
{
    // TODO: fix bold assumption
    return QString("%1/wayland-0").arg(m_xdg_runtime_dir);
}

QString SMApplication::localeVariable(LocaleVariable v)
{
    switch(v)
    {
    case LANG:
        return m_lang;
    case LCTIME:
        return m_time;
    case LCCOLLATE:
        return m_collation;
    case LCCURRENCY:
        return m_currency;
    case LCMEASURMENT:
        return m_measurement;
    case LCNUMERIC:
        return m_numeric;
    default:
        return QString();
    }
}

void SMApplication::startDBusReliantServices()
{
    m_pipewireProcess->initialize();
    m_elevatorProcess->initialize();

    m_wireplumberProcess->initialize();
    m_desktopProcess->initialize();
    m_stageProcess->initialize();

    m_pipewirePulseProcess->initialize();
    m_sessionStarted = true;
    m_dbus = new SessionDBus(this);
    QDBusConnection::sessionBus().registerService(QLatin1String(HOLLYWOOD_SESSION_DBUS));
    QDBusConnection::sessionBus().registerObject(QLatin1String(HOLLYWOOD_SESSION_DBUSOBJ), this);
    qCInfo(lcSession) << "Session Initilaized";
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
    terminateUserProcesses();
    m_desktopProcess->deconstruct();
    m_elevatorProcess->deconstruct();
    m_pipewirePulseProcess->deconstruct();
    m_wireplumberProcess->deconstruct();
    m_pipewireProcess->deconstruct();
    // insert more sub-processes HERE

    // stop the stage ,compositor, dbus-session at the end, in that order
    m_stageProcess->deconstruct();
    m_compositorProcess->deconstruct();
    m_dbusSessionProcess->deconstruct();

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

    if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    auto response = msg.arguments().constFirst().toString();

    if(response == "yes")
        return true;

    return false;
}

void SMApplication::restartCompositorReliantProcesses()
{
    m_elevatorProcess->initialize();
    m_desktopProcess->initialize();
    m_stageProcess->initialize();

    m_elevatorProcess->setAutoRestart(true);
    m_stageProcess->setAutoRestart(true);
    m_desktopProcess->setAutoRestart(true);
}

void SMApplication::verifyTrashFolder()
{
    auto xdg_data = qgetenv("XDG_DATA_HOME");
    if(xdg_data.isEmpty())
        xdg_data = qgetenv("HOME")+"/.local/share/";
    QDir dir(xdg_data+"Trash");
    if(!dir.exists())
        dir.mkpath(xdg_data+"Trash");

    dir.mkpath(xdg_data+"Trash"+"/files");
    dir.mkpath(xdg_data+"Trash"+"/info");
}

void SMApplication::compositorDied()
{
    qCCritical(lcSession) << "compositor process died! restrting";
    m_elevatorProcess->setAutoRestart(false);
    m_stageProcess->setAutoRestart(false);
    m_desktopProcess->setAutoRestart(false);
    m_userprocs.clear();
    if(m_compositorProcess->initialize())
        restartCompositorReliantProcesses();
}

void SMApplication::userProcessTerminated(int result)
{
    auto snd = qobject_cast<LSExecutor*>(sender());
    Q_UNUSED(result)
    m_userprocs.removeOne(snd);
}

bool SMApplication::terminateUserProcesses()
{
    // TODO: flesh this out more, see if anything stalls etc
    for(auto p : m_userprocs)
        p->terminate();

    return true;
}

void SMApplication::dbusAvailable(const QString &socket)
{
    qCInfo(lcSession) << "setting DBUS_SESSION_BUS_ADDRESS" << socket;
    m_dbusSessionVar = socket.toUtf8();
    qputenv("DBUS_SESSION_BUS_ADDRESS", socket.toUtf8());
    startDBusReliantServices();
}

void SMApplication::reloadLocaleSettings()
{
    qCInfo(lcSession) << "loading locale settings";
    QSettings settings("originull", "hollywood");
    settings.beginGroup("Regional");
    // clear out our change warning in the applet
    settings.setValue("SettingsChanged", false);

    m_lang = settings.value("LANG", QString::fromLocal8Bit(qgetenv("LANG"))).toString();
    if(m_lang.isEmpty())
        m_lang = "C.UTF8";
    m_detailedregion = settings.value(QStringLiteral("UseDetailed"), false).toBool();
    m_numeric = settings.value("LC_NUMERIC", QString::fromLocal8Bit(qgetenv("LC_NUMERIC"))).toString();
    m_time = settings.value("LC_TIME", QString::fromLocal8Bit(qgetenv("LC_TIME"))).toString();
    m_collation = settings.value("LC_COLLATE", QString::fromLocal8Bit(qgetenv("LC_COLLATE"))).toString();
    m_currency = settings.value("LC_MONETARY", QString::fromLocal8Bit(qgetenv("LC_MONETARY"))).toString();
    m_measurement = settings.value("LC_MEASUREMENT", QString::fromLocal8Bit(qgetenv("LC_MEASUREMENT"))).toString();
}

int main(int argc, char *argv[])
{
    SMApplication a(argc, argv);
    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood Session Manager"));
    p.addHelpOption();
    p.addVersionOption();

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
