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

#include <hollywood/layershellinterface.h>

class AIPrivateWaylandProtocol;
class PlasmaWindowManagement;
class PlasmaWindow;
class StageHost;
class LSDesktopEntry;
class StageApplication : public QApplication
{
    Q_OBJECT
public:
    StageApplication(int &argc, char **argv);
    static StageApplication* instance() { return static_cast<StageApplication*>(QApplication::instance()); }
    void createWindows();
    bool executeDesktop(const QString &desktop);
    bool callSessionDBus(const QString &exec);
    bool displayManagerStart() { return m_started_dm; }
    QMenu* systemMenu() { return m_context; }
    void playBell();
public slots:
    void launchSysmon();
    void launchSettings();
    void launchTerminull();
    void launchAbout();
    void run();
    void logoffSession();
    void restartSystem();
    void shutdownSystem();
private slots:
    void privateProtocolReady();
    void configChanged();
Q_SIGNALS:
    void clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm);
    void settingsChanged();
private:
    void loadSettings();
private:
    QFileSystemWatcher *m_cfgwatch = nullptr;
    StageHost *m_host = nullptr;
    QList<QProcess*> m_processes;
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    QString m_configfile;
    QMenu *m_context = nullptr;

    QSoundEffect m_bell;
    bool m_started_dm = false;
};

#endif // CSAPPLICATION_H
