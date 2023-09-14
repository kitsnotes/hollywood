#include "application.h"
#include "settings.h"
#include <QCommandLineParser>
#include <hollywood/hollywood.h>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

SettingsApplication::SettingsApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
#ifdef HW_BUILD_VER
    setApplicationVersion(QString("%1.%2").arg(HOLLYWOOD_OS_VERSION, QString::number(HW_BUILD_VER)));
#else
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
#endif
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("System Settings");
    setWindowIcon(QIcon::fromTheme("preferences-system"));
}

bool SettingsApplication::registerDBus()
{
    new SettingsDBus(this);
    bool reg = QDBusConnection::sessionBus().registerService(QStringLiteral("org.originull.hwsettings"));
    if(reg)
        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/originull/hwsettings"), this);

    return reg;
}

void SettingsApplication::showMainWindow(const QString &prefpane)
{
    createMainWindow();
    if(!prefpane.isEmpty())
        m_mainWnd->showAppletById(prefpane);
    m_mainWnd->show();
}

void SettingsApplication::showSingleWindow(const QString &prefpane)
{
    // TODO: check if valid prefpane
    SettingsWindow w;
    w.setSingleMode(true);
    w.showAppletById(prefpane);
    w.show();
}

void SettingsApplication::activate()
{
    createMainWindow();
    m_mainWnd->raise();
    m_mainWnd->activateWindow();
}

void SettingsApplication::openPane(const QString &prefpane)
{
    createMainWindow();
    activate();
    m_mainWnd->showAppletById(prefpane);
}

void SettingsApplication::createMainWindow()
{
    if(m_mainWnd == nullptr)
        m_mainWnd = new SettingsWindow;
}

int main(int argc, char *argv[])
{
    SettingsApplication a(argc, argv);

    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood System Settings"));
    p.addHelpOption();
    p.addVersionOption();

    p.addOptions({
                  {{"S", "single"},
                   QCoreApplication::translate("main", "Start in single applet mode.")},
                  });
    p.process(a);

    auto execapplet = QString();

    if(p.positionalArguments().count() > 0)
        execapplet = p.positionalArguments().first();

    bool single = false;
    if(p.isSet("single"))
        single = true;

    if(p.isSet("single") && p.positionalArguments().count() == 0)
    {
        qInfo() << "You need to specify a preference pane ID for single mode. Exiting.";
        return 0;
    }

    if(single)
        a.showSingleWindow(execapplet);
    else
    {
        QDBusConnection bus = QDBusConnection::sessionBus();
        if(!bus.isConnected())
        {
            // if we have no dbus, just show the window
            // lets not care too much about single instance
            // settings is too important to break over that :(

            a.showMainWindow(execapplet);
        }
        else
        {
            if(a.registerDBus())
            {
                // we're the first instance!
                a.showMainWindow(execapplet);
            }
            else
            {
                qDebug() << "Transfering to main process:" << execapplet;
                QDBusInterface ldb(QStringLiteral("org.originull.hwsettings"),
                                   QStringLiteral("/org/originull/hwsettings"),
                                   QStringLiteral("org.freedesktop.Application"));
                if(!ldb.isValid())
                {
                    qDebug() << QString("Could not call session DBUS interface.");
                    return 0;
                }
                ldb.call("Open", QStringList() << execapplet, QVariantMap());
                return 0;
            }
        }
    }

    return a.exec();
}
