// Hollywood Stage
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CSAPPLICATION_H
#define CSAPPLICATION_H

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

class AIPrivateWaylandProtocol;
class PlasmaWindowManagement;
class PlasmaWindow;
class StageHost;
class MenuServer;
class LSDesktopEntry;
class QDBusServiceWatcher;
class DBusMenuImporter;
class MenuImporter;
class OriginullMenuServerClient;
class NotifierHost;
class StageClock;
class BatteryMonitor;
class StatusNotifierButton;
class StageApplication : public QApplication
{
    Q_OBJECT
public:
    StageApplication(int &argc, char **argv);
    static StageApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }
    bool executeDesktop(const QString &desktop);
    bool callSessionDBus(const QString &exec);
    bool displayManagerStart() { return m_started_dm; }
    QMenu* systemMenu() { return m_context; }
    void playBell();
    AIPrivateWaylandProtocol* privateProtocol() { return m_protocol; }
    bool isSouthernMode() const { return m_southern; }
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
    void slotWindowRegistered(WId id, const QString &serviceName, const QDBusObjectPath &menuObjectPath);
    void menuChanged(const QString &serviceName, const QString &objectPath);
    void createStatusButton(StatusNotifierButton *btn);
    void statusButtonRemoved(StatusNotifierButton *btn);
Q_SIGNALS:
    void clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm);
    void settingsChanged();    
private:
    void loadSettings();
    void transferLayoutModes(bool useSouthern);
    void setupMenuServer();
    void destroyMenuServer();
    void moveToStage();
    void moveToMenubar();
private:
    QList<StatusNotifierButton*> m_traybtns;
    QFileSystemWatcher *m_cfgwatch = nullptr;
    StageHost *m_host = nullptr;
    MenuServer *m_menu = nullptr;
    QPointer<QMenu> m_menubar;
    QList<QProcess*> m_processes;
    QString m_configfile;
    QMenu *m_context = nullptr;

    QSoundEffect m_bell;
    bool m_started_dm = false;
    bool m_southern = false;
    bool m_has_menu_responder = false;
    NotifierHost *m_notifier = nullptr;
    QDBusServiceWatcher *m_menuViewWatcher;
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    MenuImporter* m_menuImporter = nullptr;
    QPointer<DBusMenuImporter> m_importer;
    QString m_serviceName;
    QString m_menuObjectPath;
    DBusMenuImporter *getImporter(const QString &service, const QString &path);
    OriginullMenuServerClient *m_ms = nullptr;
    StageClock *m_clock = nullptr;
    BatteryMonitor *m_battery = nullptr;
};

#endif // CSAPPLICATION_H
