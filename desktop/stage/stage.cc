// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "stage.h"
#include "stagehost.h"
#include "wndmgmt.h"
#include "stageclock.h"

#include "menuserver/menuserver.h"
#include "menuserver/menuregistrarimporter.h"
#include "statusnotifier/notifierhost.h"
#include "battery.h"
#include "surfacemanager.h"

#include <hollywood/hollywood.h>
#include <hollywood/layershellwindow.h>
#include <executor.h>
#include <hollywood/messagebox.h>
#include <desktopentry.h>
#include <QDBusInterface>
#include <QIcon>
#include <dbusmenuimporter.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include "client/privateprotocol.h"

#define HWSETTINGS_APP      "org.originull.hwsettings.desktop"
#define HWSYSMON_APP        "org.originull.sysmon.desktop"
#define TERMINULL_APP       "org.originull.terminull.desktop"
#define ABOUT_APP           "org.originull.about.desktop"

StageApplication::StageApplication(int &argc, char **argv, bool mini = false)
    : QApplication(argc, argv)
    , m_cfgwatch(new QFileSystemWatcher(this))
    , m_context(new QMenu(0))
    , m_menuViewWatcher(new QDBusServiceWatcher(this))
    , m_protocol(new HWPrivateWaylandProtocol())
    , m_battery(new BatteryMonitor(0))
    , m_mini(mini)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("Stage");
    setWindowIcon(QIcon::fromTheme("system-file-manager"));

    if(callSessionDBus("startedByDisplayManager"))
        m_started_dm = true;

    if(!m_mini)
    {
        m_wndmgr = new PlasmaWindowManagement();
        m_notifier = new NotifierHost();
        m_host = new StageHost(primaryScreen());
        m_clock = new StageClock();
    }

    loadSettings();
    m_cfgwatch = new QFileSystemWatcher();
    m_cfgwatch->addPath(m_configfile);
    connect(m_cfgwatch, &QFileSystemWatcher::fileChanged,
            this, &StageApplication::configChanged);

    connect(m_protocol, &HWPrivateWaylandProtocol::activeChanged,
            this, &StageApplication::privateProtocolReady);

    createSystemContextMenu();

    if(!m_mini)
    {
        connect(m_notifier, &NotifierHost::buttonAdded, this, &StageApplication::createStatusButton);
        connect(m_notifier, &NotifierHost::buttonRemoved, this, &StageApplication::statusButtonRemoved);
        if(m_southern)
            setupMenuServer();

        m_host->setClock(m_clock);
        m_host->setBattery(m_battery);
        if(!m_southern)
        {
            m_host->takeBattery();
            m_host->takeClock();
        }

        m_host->show();
    }
    else
    {
        setupMenuServer();
    }
}

bool StageApplication::executeDesktop(const QString &desktop)
{
    QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);
    if(!ldb.isValid())
    {
        qDebug() << QString("Could not call session DBUS interface. Command: executeDesktop");
        return false;
    }

    QDBusMessage msg = ldb.call("executeDesktop", desktop, QStringList(), QStringList());

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

bool StageApplication::openSettingsApplet(const QString &settings)
{
    QDBusInterface ldb(HOLLYWOOD_SESSION_DBUS, HOLLYWOOD_SESSION_DBUSOBJ, HOLLYWOOD_SESSION_DBUSPATH);

    if(!ldb.isValid())
    {
        qDebug() << QString("Could not call session DBUS interface. Command: openSettingsApplet");
        return false;
    }


    QDBusMessage msg = ldb.call("openSettingsApplet", settings);

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

bool StageApplication::executeFilesystemUrl(const QString &fsurl)
{
    QDBusInterface ldb(QStringLiteral("org.freedesktop.FileManager1"),
                QStringLiteral("/org/freedesktop/FileManager1"),
                QStringLiteral("org.freedesktop.FileManager1"));
    if(!ldb.isValid())
    {
        qDebug() << QString("Could not call session DBUS interface. Command: executeFilesystemUrl");
        return false;
    }

    QDBusMessage msg = ldb.call("ShowFolders", QStringList() << fsurl, QString());

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

void StageApplication::restartSystem()
{
    HWMessageBox msg(QMessageBox::NoIcon, tr("Restart System"), tr("Are you ready to restart your computer now?"), QMessageBox::Yes|QMessageBox::No);
    msg.setIconPixmap(QIcon::fromTheme("system-reboot").pixmap(QSize(64,64)));
    msg.setInformativeText(tr("You should save all documents before restarting."));
    playBell();
    if(msg.exec() == QMessageBox::Yes)
        callSessionDBus("reboot");
}

void StageApplication::shutdownSystem()
{
    HWMessageBox msg(QMessageBox::NoIcon, tr("Shut Down System"), tr("Are you ready to shut down your computer now?"), QMessageBox::Yes|QMessageBox::No);
    msg.setIconPixmap(QIcon::fromTheme("system-shutdown").pixmap(QSize(64,64)));
    msg.setInformativeText(tr("You should save all documents before shutting down."));
    playBell();
    if(msg.exec() == QMessageBox::Yes)
        callSessionDBus("powerOff");
}

void StageApplication::privateProtocolReady()
{
    if(!m_protocol)
        return;

    setupPrivateProtocolResponder();
}

void StageApplication::setupPrivateProtocolResponder()
{
    if(m_has_menu_responder)
        return;

    if(m_protocol->isActive())
    {
        auto ms = m_protocol->createMenuServerResponder();
        if(ms)
        {
            m_ms = ms;
            connect(ms, &OriginullMenuServerClient::menuChanged,
                    this, &StageApplication::menuChanged);
        }
        m_has_menu_responder = true;
    }
}

void StageApplication::launchSysmon()
{
    executeDesktop(HWSYSMON_APP);
}

void StageApplication::launchSettings()
{
    executeDesktop(HWSETTINGS_APP);
}

void StageApplication::launchTerminull()
{
    executeDesktop(TERMINULL_APP);
}

void StageApplication::launchAbout()
{
    executeDesktop(ABOUT_APP);
}

void StageApplication::run()
{

}

void StageApplication::logoffSession()
{
    if(displayManagerStart())
    {
        HWMessageBox msg(QMessageBox::NoIcon, tr("Log Off?"),
                        tr("Do you want to log off? All running applications will be closed."));
        msg.setIconPixmap(QIcon::fromTheme("system-log-out").pixmap(QSize(64,64)));
        msg.setInformativeText(tr("You should save all documents before logging off."));
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        playBell();
        if(msg.exec() == QMessageBox::Yes)
            callSessionDBus("logout");
    }
    else
    {
        HWMessageBox msg(QMessageBox::NoIcon, tr("Exit Session?"),
                         tr("Do you want to exit your session? All running applications will be closed."));
        msg.setIconPixmap(QIcon::fromTheme("application-exit").pixmap(QSize(64,64)));
        msg.setInformativeText(tr("You should save all documents before exiting your session."));
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        playBell();
        if(msg.exec() == QMessageBox::Yes)
            exit();
    }
}

void StageApplication::configChanged()
{
    loadSettings();
    m_cfgwatch->addPath(m_configfile);
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

void StageApplication::playBell()
{
    // TODO: move into thread so this doesn't block
    //m_bell.play();
}

void StageApplication::loadSettings()
{
    auto olddock = m_dock;

    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    if(m_configfile.isEmpty())
        m_configfile = settings.fileName();

    settings.beginGroup("Bell");
    auto bell = settings.value("AudioFile", "/usr/share/sounds/Hollywood/Bell.wav").toString();
    m_bell.setSource(QUrl::fromLocalFile(bell));
    settings.endGroup();
    settings.beginGroup("Stage");
    bool southern = settings.value("UseSouthernMode", true).toBool();
    if(m_mini)
    {
        southern = true;
        transferLayoutModes(southern);
        emit settingsChanged();
        return;
    }


    // none of these settings are required in ministage mode
    auto dockedge = settings.value("DockEdge").toString().toLower();
    if(dockedge == "top")
    {
        if(!southern)
            m_dock = Qt::TopEdge;
        else
            m_dock = Qt::BottomEdge;
    }
    else if(dockedge == "left")
        m_dock = Qt::LeftEdge;
    else if(dockedge == "right")
        m_dock = Qt::RightEdge;

    auto show_clock = settings.value("ShowClock", HOLLYWOOD_STCLK_SHOW).toBool();
    auto show_date = settings.value("ShowDateInClock", HOLLYWOOD_STCLK_USEDATE).toBool();
    auto show_seconds = settings.value("ShowSecondsInClock", HOLLYWOOD_STCLK_USESECONDS).toBool();
    auto clock_24hr = settings.value("Use24HourClock", HOLLYWOOD_STCLK_24HOUR).toBool();
    auto clock_ampm = settings.value("ShowAMPMInClock", HOLLYWOOD_STCLK_USEAMPM).toBool();

    settings.endGroup();
    m_clock->clockSettingsChanged(show_clock, show_date, show_seconds, clock_24hr, clock_ampm);
    transferLayoutModes(southern);

    if(m_dock != olddock)
    {
        m_host->hide();
        if(m_dock == Qt::LeftEdge || m_dock == Qt::RightEdge)
            m_host->setAlignment(Qt::Vertical);
        else
            m_host->setAlignment(Qt::Horizontal);
        m_host->show();
    }

    emit settingsChanged();
}

void StageApplication::createSystemContextMenu()
{
    m_context->menuAction()->setData(9195521);
    m_context->menuAction()->setIcon(QIcon::fromTheme("hollywood-logo"));

    if(!m_mini)
    {
        auto aboutSys = m_context->addAction(tr("&About This Computer..."));
        m_context->addSeparator();
        auto applications = m_context->addAction(tr("&Applications"));
        auto settings = m_context->addAction(tr("System &Settings"));
        auto sysmon = m_context->addAction(tr("System &Monitor"));
        auto term = m_context->addAction(tr("&Terminull"));

        connect(applications, &QAction::triggered, [this]() {
            executeFilesystemUrl(QStringLiteral("applications://"));
        });

        connect(aboutSys, &QAction::triggered, this, &StageApplication::launchAbout);
        connect(settings, &QAction::triggered, this, &StageApplication::launchSettings);
        connect(sysmon, &QAction::triggered, this, &StageApplication::launchSysmon);
        connect(term, &QAction::triggered, this, &StageApplication::launchTerminull);
        m_context->addSeparator();
    }
    auto restart = m_context->addAction(tr("&Restart..."));
    connect(restart, &QAction::triggered, this, &StageApplication::restartSystem);
    auto shutdown = m_context->addAction(tr("Shut &Down..."));
    connect(shutdown, &QAction::triggered, this, &StageApplication::shutdownSystem);
    if(!m_mini)
    {
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

                logoff->setText(tr("&Log off %1...").arg(display));
            }
            else
                logoff->setText(tr("&Log off..."));
        }
        connect(logoff, &QAction::triggered, this, &StageApplication::logoffSession);
    }
}

void StageApplication::transferLayoutModes(bool useSouthern)
{
    if(useSouthern == m_southern)
        return;

    if(useSouthern)
    {
        setupMenuServer();
        moveToMenubar();
        if(m_host)
            m_host->setMainEnabled(false);
    }
    else
    {
        destroyMenuServer();
        moveToStage();
        if(m_host)
        {
            m_host->takeBattery();
            m_host->takeClock();
        }
    }

    m_southern = useSouthern;
}


void StageApplication::setupMenuServer()
{
    setupPrivateProtocolResponder();
    // Setup a menu importer if needed
    if (!m_menuImporter)
    {
        m_menuImporter = new MenuRegistrarImporter(this);
        m_menuImporter->connectToBus();
    }

    if(!m_menu)
    {
        m_menu = new MenuServer(m_clock, m_battery, primaryScreen());
        m_menu->show();
    }
}

void StageApplication::destroyMenuServer()
{
    m_host->setMainEnabled(false);
    m_menuImporter->deleteLater();
    disconnect(m_ms, &OriginullMenuServerClient::menuChanged,
            this, &StageApplication::menuChanged);
    m_ms->deleteLater();
    m_menu->deleteLater();
}

void StageApplication::moveToStage()
{
    m_host->setMainEnabled(true);
    for(auto b : m_traybtns)
    {
        m_menu->statusButtonRemoved(b);
        m_host->createStatusButton(b);
    }
}

void StageApplication::moveToMenubar()
{
    if(m_host == nullptr)
        return;

    for(auto b : m_traybtns)
    {
        m_host->statusButtonRemoved(b);
        m_menu->createStatusButton(b);
    }
}

void StageApplication::menuChanged(const QString &serviceName, const QString &objectPath)
{
    if(!m_southern)
        return;

    if (m_serviceName == serviceName && m_menuObjectPath == objectPath)
    {
        if (m_importer)
            QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);

        return;
    }
    if(serviceName.isEmpty() || objectPath.isEmpty())
    {
        //todo: generic menu
        return;
    }

    m_serviceName = serviceName;
    m_menuViewWatcher->setWatchedServices(QStringList({serviceName}));
    m_menuObjectPath = objectPath;

    if(m_importer != nullptr)
        m_importer->deleteLater();

    m_importer = new DBusMenuImporter(serviceName, objectPath, m_menu->menuBar());
    QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);

    connect(m_importer.data(), &DBusMenuImporter::menuUpdated, this, [=]()
    {
        QMenu *menu = m_importer->menu();
        // filter out a submenu
        if(!m_importer->menu() || m_importer->menu() != menu)
            return;

        m_menubar = m_importer->menu();
        m_menu->installMenu(m_menubar);
    });
}

void StageApplication::createStatusButton(StatusNotifierButton *btn)
{
    m_traybtns.append(btn);
    if(m_southern)
        m_menu->createStatusButton(btn);
    else
        m_host->createStatusButton(btn);
}

void StageApplication::statusButtonRemoved(StatusNotifierButton *btn)
{
    m_traybtns.removeOne(btn);
    if(m_southern)
        m_menu->statusButtonRemoved(btn);
    else
        m_host->statusButtonRemoved(btn);
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DontShowShortcutsInContextMenus, false);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);

    bool mini = false;
    auto envvar = qgetenv("HOLLYWOOD_RECOVERY_ENV");
    if(!envvar.isEmpty())
    {
        if(envvar == "1")
            mini = true;
    }
    StageApplication a(argc, argv, mini);
    return a.exec();
}
