#ifndef MSAPPLICATION_H
#define MSAPPLICATION_H

#include <QApplication>
#include "dbusmenu_interface.h"

#include <QPointer>
#include <QMenu>

#define ARION_MENUSERVER_VERSION "1.0"

#if defined(msApp)
#undef msApp
#endif
#define msApp (MSApplication::instance())

class AIPrivateWaylandProtocol;
class QDBusServiceWatcher;
class MSMenuServer;
class DBusMenuImporter;
class MenuImporter;
class QMenu;
class OriginullMenuServerClient;
class MSApplication : public QApplication
{
    Q_OBJECT
public:
    MSApplication(int &argc, char **argv);
    void startMenuServer();
signals:
    void showRequest(const QString &serviceName, const QDBusObjectPath &menuObjectPath, int actionId);
    void menuShown(const QString &service, const QDBusObjectPath &objectPath);
    void menuHidden(const QString &service, const QDBusObjectPath &objectPath);
private slots:
    void itemActivationRequested(int actionId, uint timeStamp);
    void privateProtocolReady();
    void slotWindowRegistered(WId id, const QString &serviceName, const QDBusObjectPath &menuObjectPath);
    void slotShowMenu(int x, int y, const QString &serviceName, const QDBusObjectPath &menuObjectPath, int actionId);
    void reconfigure();
    void menuChanged(const QString &serviceName, const QString &objectPath);
    void dbusMenuUpdated(QMenu *menu);
private:
    void hideMenu();
private:
    MSMenuServer *m_serverPrimary;
    MenuImporter* m_menuImporter = nullptr;
    DBusMenuImporter *getImporter(const QString &service, const QString &path);
    AIPrivateWaylandProtocol *m_protocol = nullptr;
    QDBusServiceWatcher *m_menuViewWatcher;
    QPointer<DBusMenuImporter> m_importer;
    QPointer<QMenu> m_menu;
    QString m_serviceName;
    QString m_menuObjectPath;
    OriginullMenuServerClient *m_ms = nullptr;
};

#endif // MSAPPLICATION_H
