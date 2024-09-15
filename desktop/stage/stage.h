// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QApplication>
#include <QObject>
#include <QStandardItemModel>
#include <QMenu>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QSoundEffect>
#include <QDBusObjectPath>
#include <QPointer>
#include <hollywood/layershellinterface.h>
#include <dbusmenuimporter.h>

class HWPrivateWaylandProtocol;
class PlasmaWindowManagement;
class PlasmaWindow;
class StageHost;
class MenuServer;
class LSDesktopEntry;
class QDBusServiceWatcher;
class DBusMenuImporter;
class MenuRegistrarImporter;
class OriginullMenuServerClient;
class NotifierHost;
class StageClock;
class BatteryMonitor;
class StatusNotifierButton;
class SurfaceManager;
class PlasmaWindowManagement;
class StageApplication : public QApplication
{
    Q_OBJECT
public:
    StageApplication(int &argc, char **argv, bool mini);
    static StageApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }
    bool executeDesktop(const QString &desktop);
    bool openSettingsApplet(const QString &settings);
    bool executeFilesystemUrl(const QString &fsurl);
    bool callSessionDBus(const QString &exec);
    bool displayManagerStart() { return m_started_dm; }
    QMenu* systemMenu() { return m_context; }
    DBusMenuImporter* importer() { return qobject_cast<DBusMenuImporter*>(m_importer.data()); }
    PlasmaWindowManagement* windowManager() { return m_wndmgr; }
    void playBell();
    HWPrivateWaylandProtocol* privateProtocol() { return m_protocol; }
    bool isSouthernMode() const { return m_southern; }
    Qt::Edge dockEdge() const { return m_dock; }
public slots:
    void launchSysmon();
    void launchSettings();
    void launchTerminull();
    void launchAbout();
    void run();
    void logoffSession();
    void restartSystem();
    void shutdownSystem();
protected slots:
    friend class MenuServer;
    void privateProtocolReady();
private slots:
    void setupPrivateProtocolResponder();
    void configChanged();
    void menuChanged(const QString &serviceName, const QString &objectPath);
    void createStatusButton(StatusNotifierButton *btn);
    void statusButtonRemoved(StatusNotifierButton *btn);
signals:
    void clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm);
    void settingsChanged();    
private:
    void loadSettings();
    void createSystemContextMenu();
    void transferLayoutModes(bool useSouthern);
    void setupMenuServer();
    void destroyMenuServer();
    void moveToStage();
    void moveToMenubar();
    DBusMenuImporter *getImporter(const QString &service, const QString &path);
private:
    friend class DBusMenuImporter;
    QString m_configfile;
    QFileSystemWatcher *m_cfgwatch = nullptr;
    // The global plasma window wayland protocol
    PlasmaWindowManagement *m_wndmgr = nullptr;
    // Notification Icon List
    QList<StatusNotifierButton*> m_traybtns;
    // The primary screen stage
    StageHost *m_host = nullptr;
    // The menu server
    MenuServer *m_menu = nullptr;
    QDBusServiceWatcher *m_menuViewWatcher;
    HWPrivateWaylandProtocol *m_protocol = nullptr;
    MenuRegistrarImporter* m_menuImporter = nullptr;
    QPointer<DBusMenuImporter> m_importer;
    QString m_serviceName;
    QString m_menuObjectPath;
    OriginullMenuServerClient *m_ms = nullptr;
    QPointer<QMenu> m_menubar;

    QList<QProcess*> m_processes;
    QMenu *m_context = nullptr;

    QSoundEffect m_bell;

    // Configuration Variables

    bool m_started_dm = false;
    bool m_southern = false;
    bool m_has_menu_responder = false;
    bool m_combine_apps = false;
    Qt::Edge m_dock = Qt::BottomEdge;

    NotifierHost *m_notifier = nullptr;


    StageClock *m_clock = nullptr;
    BatteryMonitor *m_battery = nullptr;

    bool m_mini = false;
};

