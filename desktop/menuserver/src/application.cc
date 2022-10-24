#include "application.h"
#include "menuserver.h"
#include "menuimporter.h"
#include "dbusmenuimporter.h"

#include <client/privateprotocol.h>
#include <hollywood/hollywood.h>

#include "../coreshell/wndmgmt.h"
#define HOLLYWOOD_MENUSERVER_DESKTOP    "org.originull.menuserver.desktop"

static const QByteArray s_AppMenuServiceNamePropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_SERVICE_NAME");
static const QByteArray s_AppMenuObjectPathPropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_OBJECT_PATH");

MSApplication::MSApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_serverPrimary(new MSMenuServer(0))
    , m_protocol(new AIPrivateWaylandProtocol)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setApplicationName("MenuServer");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setDesktopFileName(HOLLYWOOD_MENUSERVER_DESKTOP);

    connect(m_protocol, &AIPrivateWaylandProtocol::activeChanged,
            this, &MSApplication::privateProtocolReady);
    // setup our DBusMenu

    bool use_dbusmenu = false;
    if(use_dbusmenu)
    {
        m_menuViewWatcher = new QDBusServiceWatcher(QStringLiteral("org.originull.appmenuview"),
                                                        QDBusConnection::sessionBus(),
                                                        QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration,
                                                        this);

        QDBusConnection::sessionBus().connect({},
                                              {},
                                              QStringLiteral("com.canonical.dbusmenu"),
                                              QStringLiteral("ItemActivationRequested"),
                                              this,
                                              SLOT(itemActivationRequested(int, uint)));

        // Setup a menu importer if needed
        if (!m_menuImporter) {
            m_menuImporter = new MenuImporter(this);
            connect(m_menuImporter, &MenuImporter::WindowRegistered, this, &MSApplication::slotWindowRegistered);
            m_menuImporter->connectToBus();
        }

        connect(m_menuViewWatcher, &QDBusServiceWatcher::serviceUnregistered, this, [this](const QString &service) {
            Q_UNUSED(service)
            QDBusConnection::sessionBus().disconnect({},
                                                     {},
                                                     QStringLiteral("com.canonical.dbusmenu"),
                                                     QStringLiteral("ItemActivationRequested"),
                                                     this,
                                                     SLOT(itemActivationRequested(int, uint)));
            delete m_menuImporter;
            m_menuImporter = nullptr;
        });
    }
}

void MSApplication::startMenuServer()
{
    m_serverPrimary->setVisible(true);
}

void MSApplication::privateProtocolReady()
{
    if(m_protocol->isActive())
    {
        auto ms = m_protocol->setMenuServerForWindow(m_serverPrimary->windowHandle());
        m_ms = ms;
        connect(ms, &OriginullMenuServerClient::menuChanged, this, &MSApplication::menuChanged);
    }
}

void MSApplication::slotWindowRegistered(WId id, const QString &serviceName, const QDBusObjectPath &menuObjectPath)
{
    qDebug() << "MSApplication::slotWindowRegistered winId: " << id << "serviceName" << serviceName;
    menuChanged(serviceName, menuObjectPath.path());

}

void MSApplication::slotShowMenu(int x, int y, const QString &serviceName, const QDBusObjectPath &menuObjectPath, int actionId)
{

}

void MSApplication::reconfigure()
{

}

void MSApplication::menuChanged(const QString &serviceName, const QString &objectPath)
{
    qDebug() << "privateProtocol: menuChanged" << serviceName << objectPath;
    if (m_serviceName == serviceName && m_menuObjectPath == objectPath) {
        if (m_importer) {
            QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);
        }

        return;
    }

    if(m_importer != nullptr)
        m_importer->deleteLater();


    m_importer = new DBusMenuImporter(serviceName, objectPath, this);
    QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);

    connect(m_importer.data(), &DBusMenuImporter::menuUpdated, this, [ = ](QMenu * menu) {
        m_menu = m_importer->menu();

        if (m_menu.isNull() || menu != m_menu) {
            return;
        }

        //cache first layer of sub menus, which we'll be popping up
        for (QAction *a : m_menu->actions()) {
            // signal dataChanged when the action changes
            connect(a, &QAction::changed, this, [this, a] {
                if (/* m_wm && m_wm->menuAvailable() &&*/ m_menu)
                {
                    /*const int actionIdx = m_menu->actions().indexOf(a);

                    if (actionIdx > -1) {
                        const QModelIndex modelIdx = index(actionIdx, 0);
                        emit dataChanged(modelIdx, modelIdx);
                    }*/
                }
            });

            //connect(a, &QAction::destroyed, this, &AppMenuModel::modelNeedsUpdate);

            auto am = QMenu::menuInAction(a);
            if (am) {
                m_importer->updateMenu(am);
            }
        }

        //m_wm->setMenuAvailable(true);
        //emit modelNeedsUpdate();
        m_serverPrimary->installMenu(m_menu);
    });


    connect(m_importer.data(), &DBusMenuImporter::actionActivationRequested, this, [this](QAction * action) {
        // TODO submenus
        if (/* !m_wm || !m_wm->menuAvailable() || */ !m_menu) {
            return;
        }

        const auto actions = m_menu->actions();
        auto it = std::find(actions.begin(), actions.end(), action);

        /*if (it != actions.end()) {
            requestActivateIndex(it - actions.begin());
        }*/
    });
}

void MSApplication::dbusMenuUpdated(QMenu *menu)
{
    qDebug() << "MSApplication::slotShowMenu dbusMenuUpdated menuActionCount:" << menu->actions().count();
    for(auto i : menu->actions())
    {
        qDebug() << i->text();
    }
    qDebug() << "endTest";
    m_serverPrimary->installMenu(menu);
}

void MSApplication::itemActivationRequested(int actionId, uint timeStamp)
{
    Q_UNUSED(timeStamp);
    //emit showRequest(message().service(), QDBusObjectPath(message().path()), actionId);
}


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
    QApplication::setStyle("hollywood");
    MSApplication a(argc, argv);

    a.startMenuServer();

    return a.exec();
}
