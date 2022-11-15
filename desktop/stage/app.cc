#include "app.h"
#include "stagehost.h"
#include "progman.h"
#include "wndmgmt.h"

#include <hollywood/hollywood.h>
#include <hollywood/layershellwindow.h>
#include <executor.h>

#include <desktopentry.h>
#include <QDBusInterface>
#include <QIcon>

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#define HWSETTINGS_APP      "org.originull.hwsettings.desktop"
#define HWSYSMON_APP        "org.originull.sysmon.desktop"
#define TERMINULL_APP       "org.originull.terminull.desktop"
#define ABOUT_APP           "org.originull.about.desktop"

StageApplication::StageApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_cfgwatch(new QFileSystemWatcher(this))
    , m_wndmgr(new PlasmaWindowManagement())
    , m_model(new QStandardItemModel(this))
    , m_context(new QMenu(0))
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("Stage");
    setWindowIcon(QIcon::fromTheme("system-file-manager"));

    if(callSessionDBus("startedByDisplayManager"))
        m_started_dm = true;

    loadSettings();
    m_cfgwatch = new QFileSystemWatcher();
    m_cfgwatch->addPath(m_configfile);
    connect(m_cfgwatch, &QFileSystemWatcher::fileChanged,
            this, &StageApplication::configChanged);

    connect(m_wndmgr, &PlasmaWindowManagement::windowCreated,
            this, &StageApplication::newPlasmaWindow);

    auto aboutSys = m_context->addAction(tr("&About This Computer..."));
    m_context->addSeparator();
    auto settings = m_context->addAction(tr("System &Settings"));
    auto sysmon = m_context->addAction(tr("System &Monitor"));
    auto term = m_context->addAction(tr("&Terminull"));

    connect(aboutSys, &QAction::triggered, this, &StageApplication::launchAbout);
    connect(settings, &QAction::triggered, this, &StageApplication::launchSettings);
    connect(sysmon, &QAction::triggered, this, &StageApplication::launchSysmon);
    connect(term, &QAction::triggered, this, &StageApplication::launchTerminull);
    m_context->addSeparator();
    auto restart = m_context->addAction(tr("&Restart..."));
    connect(restart, &QAction::triggered, this, &StageApplication::restartSystem);
    auto shutdown = m_context->addAction(tr("Shut &Down..."));
    connect(shutdown, &QAction::triggered, this, &StageApplication::shutdownSystem);
    m_context->addSeparator();
    auto logoff = m_context->addAction(tr("&Exit Session..."));
    if(displayManagerStart())
    {
        struct passwd *me;
        if((me = getpwuid(getuid())) != NULL)
        {
            auto gecos = QString(me->pw_gecos);
            auto name = QString(me->pw_name);

            QString display = name;
            if(!gecos.isEmpty())
                display = gecos;

            logoff->setText(tr("Log off %1...").arg(display));
        }
        else
            logoff->setText(tr("Log off..."));

        logoff->setDisabled(true);
    }
    connect(logoff, &QAction::triggered, this, &StageApplication::logoffSession);
}

void StageApplication::createWindows()
{
    m_host = new StageHost(primaryScreen());
    m_host->show();
    connect(m_wndmgr, &PlasmaWindowManagement::stackingOrderChanaged, m_host, &StageHost::stackingOrderChanged);
}

void StageApplication::aboutApplication()
{
    // This is assuming that this comes from an action
    // that would be a child of the mainwindow

}

QStandardItemModel *StageApplication::model()
{
    if(m_model)
        return m_model;

    return nullptr;
}

void StageApplication::executeDesktop(const QString &desktop)
{
    LSExecutor *exec = new LSExecutor(this);
    exec->setDesktopFile(desktop);
    exec->launch();
    return;
}

void StageApplication::showProgramManager()
{
    if(m_pm == nullptr)
    {
        m_pm = new ProgramManager(0);
    }
    m_pm->show();
}

void StageApplication::restartSystem()
{
    QMessageBox msg(0);
    msg.setIconPixmap(QIcon::fromTheme("system-reboot").pixmap(QSize(64,64)));
    msg.setWindowTitle(tr("Restart System"));
    msg.setText(tr("<b>Are you ready to restart your computer now?</b>"));
    msg.setInformativeText(tr("You should save all documents before restarting."));
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if(msg.exec() == QMessageBox::Yes)
        callSessionDBus("reboot");
}

void StageApplication::shutdownSystem()
{
    QMessageBox msg(0);
    msg.setIconPixmap(QIcon::fromTheme("system-shutdown").pixmap(QSize(64,64)));
    msg.setWindowTitle(tr("Shut Down System"));
    msg.setText(tr("<b>Are you ready to shut down your computer now?</b>"));
    msg.setInformativeText(tr("You should save all documents before shutting down."));
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    if(msg.exec() == QMessageBox::Yes)
        callSessionDBus("powerOff");
}

void StageApplication::privateProtocolReady()
{


}

void StageApplication::launchSysmon()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSYSMON_APP);
    executeDesktop(df);
}

void StageApplication::launchSettings()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSETTINGS_APP);
    executeDesktop(df);
}

void StageApplication::launchTerminull()
{
    auto df = LSDesktopEntry::findDesktopFile(TERMINULL_APP);
    executeDesktop(df);
}

void StageApplication::launchAbout()
{
    auto df = LSDesktopEntry::findDesktopFile(ABOUT_APP);
    executeDesktop(df);
}
void StageApplication::run()
{

}

void StageApplication::logoffSession()
{
    QMessageBox msg(QMessageBox::Question, tr("Exit Session?"),
                    tr("Do you want to exit your session? All running applications will be closed."),
                    QMessageBox::Yes|QMessageBox::No);
    if(msg.exec() == QMessageBox::Yes)
    {
        exit();
    }
}

void StageApplication::windowMinimized()
{
    PlasmaWindow *w = qobject_cast<PlasmaWindow*>(sender());
    if(!w)
        return;

    auto test = new QStandardItem(w->windowTitle());
    test->setIcon(QIcon::fromTheme(QLatin1String("utilities-terminal")));
    test->setData("Window");
    test->setData(w->uuid().toString(QUuid::WithoutBraces), Qt::UserRole+2);
    m_model->appendRow(test);
}

void StageApplication::newPlasmaWindow(PlasmaWindow *c)
{
    m_host->createWindowButton(c);
}

void StageApplication::configChanged()
{
    loadSettings();
}

bool StageApplication::callSessionDBus(const QString &exec)
{
    QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);
    if(!ldb.isValid())
    {
         qDebug() << QString("Could not call session DBUS interface. Command: %1")
                   .arg(exec);
        return false;
    }

    QDBusMessage msg = ldb.call(exec);

    if(msg.arguments().isEmpty() || msg.arguments().constFirst().isNull())
        return true;

    auto response = msg.arguments().constFirst();

    if(msg.arguments().isEmpty())
        return true;

    if(response.isNull())
        return true;

    if(response.toBool())
        return true;

    return false;
}

void StageApplication::startProcess(const QString &exec)
{
    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    QProcess *proc = new QProcess(this);
    proc->setProgram(exec);
    proc->start();
    m_processes.append(proc);
}

void StageApplication::loadSettings()
{
    qDebug() << "Loading Settings";
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    m_configfile = settings.fileName();

    settings.beginGroup("Stage");
    auto show_clock = settings.value("ShowClock", HOLLYWOOD_STCLK_SHOW).toBool();
    auto show_date = settings.value("ShowDateInClock", HOLLYWOOD_STCLK_USEDATE).toBool();
    auto show_seconds = settings.value("ShowSecondsInClock", HOLLYWOOD_STCLK_USESECONDS).toBool();
    auto clock_24hr = settings.value("Use24HourClock", HOLLYWOOD_STCLK_24HOUR).toBool();
    auto clock_ampm = settings.value("ShowAMPMInClock", HOLLYWOOD_STCLK_USEAMPM).toBool();

    settings.endGroup();
    emit settingsChanged();
    emit clockSettingsChanged(show_clock, show_date, show_seconds, clock_24hr, clock_ampm);
}

int main(int argc, char *argv[])
{
    StageApplication a(argc, argv);
    a.createWindows();
    return a.exec();
}
