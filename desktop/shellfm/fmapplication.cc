#include "fmapplication.h"
#include "desktop.h"
#include <hollywood/hollywood.h>
#include <executor.h>
#include <QLocalSocket>
#include <iostream>

#include "filemanager1_adaptor.h"

FMApplication::FMApplication(int &argc, char **argv)
    :QApplication(argc, argv)
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setApplicationName("Hollywood Shell");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setWindowIcon(QIcon::fromTheme("system-file-manager"));
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
}

void FMApplication::aboutApplication()
{
    // This is assuming that this comes from an action
    // that would be a child of the mainwindow
    QObject *mySrc = sender()->parent();
    HWAboutDialog *about;
    if(mySrc != nullptr)
    {
        QWidget *mySrcWidget = qobject_cast<QWidget*>(mySrc);
        if(mySrcWidget->objectName().startsWith("FMWindow"))
            about = new HWAboutDialog(mySrcWidget);
        else
            about = new HWAboutDialog(0);
    }
    else
        about = new HWAboutDialog(0);

    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();
}

void FMApplication::newFileWindow()
{
    QString homeFolder =
            QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();

    newFileWindow(QUrl::fromLocalFile(homeFolder));
}

void FMApplication::newFileWindow(const QUrl &path)
{
    FileWindow *w = new FileWindow(ArionShell::WINDOW_BROWSER, 0);
    w->shellHost()->newTabWithPath(path);
    m_fileWindows.append(w);
    w->show();
}

void FMApplication::createDesktop()
{
    DesktopWindow *w = new DesktopWindow(0);
    connect(w, &DesktopWindow::openFolder, this,
            &FMApplication::openFolderFromDesktop);
    w->show();
    m_desktop = w;
}

void FMApplication::showWallpaperSettings()
{
    LSExecutor *e = new LSExecutor(this);
    if(e->setDesktopFile(HOLYWOOD_SETTINGS_APP))
    {
        e->addArgument(HOLLYWOOD_WP_APPLET);
        e->launch();
    }
    else
    {
        QMessageBox::critical(0, tr("Unable to launch Settings"), tr("Unable to launch the Settings application.  Please check the file %1 exists in a XDG desktop search path.").arg(HOLYWOOD_SETTINGS_APP));
        delete e;
    }
}

void FMApplication::openFolderFromDesktop(const QUrl &path)
{
    newFileWindow(path);
}

void FMApplication::createDBusInterfaces()
{
    if(m_dbusStarted)
        return;


    QDBusConnection dbus = QDBusConnection::sessionBus();
    connect(dbus.interface(), &QDBusConnectionInterface::serviceRegistered, this, [this](const QString &service) {
        if(service == "org.freedesktop.FileManager1")
        {
            QDBusConnection dbus = QDBusConnection::sessionBus();
            disconnect(dbus.interface(), &QDBusConnectionInterface::serviceRegistered, this, nullptr);
            new FileManager1Adaptor(this);
            if(!dbus.registerObject(QStringLiteral("/org/freedesktop/FileManager1"), this)) {
                qDebug() << "Could not register the FileManager1 adaptor" << dbus.lastError().message();
            }
        }
    });
    dbus.interface()->registerService("org.freedesktop.FileManager1", QDBusConnectionInterface::QueueService);
    m_dbusStarted = true;
}

bool FMApplication::checkSocket()
{
    auto xdg_runtime = qgetenv("XDG_RUNTIME_DIR");
    auto xdg_runtime_sock = QString("%1/shellfm.sock").arg(xdg_runtime);
    QLocalSocket* socket = new QLocalSocket();
    socket->connectToServer(xdg_runtime_sock);
    bool b_connected = socket->waitForConnected(500);
    socket->abort();
    delete socket;
    if (b_connected) {
       qWarning() <<  QCoreApplication::translate("Session", "Another Hollywood shell is running. This instance is aborting");
       quit();
       return false;
    }

    m_socket = new QLocalServer(this);
    m_socket->removeServer(xdg_runtime_sock);
    m_socket->listen(xdg_runtime_sock);
    return true;
}

void FMApplication::ShowFolders(const QStringList &urlList, const QString &startupId)
{
    Q_UNUSED(startupId)
    for(auto &url : urlList)
    {
        auto qurl = QUrl(url);
        if(qurl.scheme() == "file")
            newFileWindow(url);
    }
}

void FMApplication::ShowItems(const QStringList &urlList, const QString &startupId)
{

}

void FMApplication::ShowItemProperties(const QStringList &urlList, const QString &startupId)
{

}

void FMApplication::checkForSessionStartup()
{

}

int main(int argc, char *argv[])
{
    FMApplication a(argc, argv);
    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood File Manager"));
    p.addHelpOption();
    p.addVersionOption();

#ifndef QT_DEBUG
    if(!a.checkSocket())
        return 0;
    a.createDBusInterfaces();
    a.createDesktop();
#else
    a.newFileWindow();
#endif
    // required so sub-processes do not get qt-shell
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "xdg-shell");
    return a.exec();
}
