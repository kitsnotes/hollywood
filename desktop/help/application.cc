#include "application.h"
#include "helpwindow.h"
#include <QCommandLineParser>
#include <hollywood/hollywood.h>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

HelpViewerApp::HelpViewerApp(int &argc, char **argv)
    : QApplication(argc, argv)
{
#ifdef HW_BUILD_VER
    setApplicationVersion(QString("%1.%2").arg(HOLLYWOOD_OS_VERSION, QString::number(HW_BUILD_VER)));
#else
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
#endif
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("Help Viewer");
    setWindowIcon(QIcon::fromTheme("help-browser"));
    setDesktopFileName(QLatin1String("org.originull.help"));
    setStyle(new HelpProxyStyle);
}

bool HelpViewerApp::registerDBus()
{
    new SettingsDBus(this);
    bool reg = QDBusConnection::sessionBus().registerService(QStringLiteral("org.originull.help"));
    if(reg)
        QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/originull/help"), this);

    return reg;
}

void HelpViewerApp::showMainWindow(const QUrl &url)
{
    createMainWindow();
    m_mainWnd->show();
}

void HelpViewerApp::activate()
{
    createMainWindow();
    m_mainWnd->raise();
    m_mainWnd->activateWindow();
}

void HelpViewerApp::openUrl(const QUrl &url)
{
    createMainWindow();
    activate();
    //m_mainWnd->showAppletById(prefpane);
}

void HelpViewerApp::openIndex()
{
}

void HelpViewerApp::createMainWindow()
{
    if(m_mainWnd == nullptr)
        m_mainWnd = new HelpWindow(HW_DEFAULT_HELP);
}

int main(int argc, char *argv[])
{
    HelpViewerApp a(argc, argv);

    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood Help"));
    p.addHelpOption();
    p.addVersionOption();

    p.process(a);

    auto execurl = QUrl();

    if(p.positionalArguments().count() > 0)
        execurl = QUrl(p.positionalArguments().first());

    QDBusConnection bus = QDBusConnection::sessionBus();
    if(!bus.isConnected())
    {
        // if we have no dbus, just show the window
        // lets not care too much about single instance
        // settings is too important to break over that :(

        a.showMainWindow(execurl);
    }
    else
    {
        if(a.registerDBus())
        {
            // we're the first instance!
            a.showMainWindow(execurl);
        }
        else
        {
            qDebug() << "Transfering to main process:" << execurl;
            QDBusInterface ldb(QStringLiteral("org.originull.help"),
                               QStringLiteral("/org/originull/help"),
                               QStringLiteral("org.freedesktop.Application"));
            if(!ldb.isValid())
            {
                qDebug() << QString("Could not call session DBUS interface.");
                return 0;
            }
            ldb.call("Open", QStringList() << execurl.toString(), QVariantMap());
            return 0;
        }
    }

    return a.exec();
}
