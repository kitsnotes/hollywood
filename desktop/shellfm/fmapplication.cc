#include "fmapplication.h"
#include "desktop.h"

#include <client/privateprotocol.h>
#include <hollywood/hollywood.h>
#include <executor.h>

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
    ADAboutDialog *about;
    if(mySrc != nullptr)
    {
        QWidget *mySrcWidget = qobject_cast<QWidget*>(mySrc);
        if(mySrcWidget->objectName().startsWith("FMWindow"))
            about = new ADAboutDialog(mySrcWidget);
        else
            about = new ADAboutDialog(0);
    }
    else
        about = new ADAboutDialog(0);

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
    m_protocol = new AIPrivateWaylandProtocol;
    connect(m_protocol, &AIPrivateWaylandProtocol::activeChanged,
            this, &FMApplication::privateProtocolReady);

    DesktopWindow *w = new DesktopWindow(0);
    connect(w, &DesktopWindow::openFolder, this,
            &FMApplication::openFolderFromDesktop);
    w->show();
    m_desktop = w;
    if(m_protocol->isActive())
    {
        /*for(auto desk : m_desktop->desktops())
            m_protocol->setDesktopForWindow(desk->windowHandle());*/
        m_protocol->setDesktopForWindow(w->windowHandle());
    }
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

void FMApplication::privateProtocolReady()
{
    if(m_protocol->isActive())
    {
        m_protocol->setDesktopForWindow(m_desktop->windowHandle());
    }
}


void FMApplication::openFolderFromDesktop(const QUrl &path)
{
    newFileWindow(path);
}

int main(int argc, char *argv[])
{
    FMApplication a(argc, argv);
    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood File Manager"));
    p.addHelpOption();
    p.addVersionOption();

    p.addOptions({
        {{"D", "desktop"},
            QCoreApplication::translate("main", "Start Desktop Shell.")},
    });
    p.process(a);

    if(p.isSet("desktop"))
        a.createDesktop();
    else
        a.newFileWindow();

    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "xdg-shell");
    return a.exec();
}
