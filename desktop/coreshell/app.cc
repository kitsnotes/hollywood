#include "app.h"
#include "window.h"
#include "progman.h"
#include "wndmgmt.h"

#include <hollywood.h>
#include <aboutdialog.h>
#include <desktopentry.h>

#include <QIcon>
#include <client/privateprotocol.h>

#define HWSETTINGS_APP      "org.originull.hwsettings.desktop"
#define HWSYSMON_APP        "org.originull.hwsysmon.desktop"
#define TERMINULL_APP       "org.originull.terminull.desktop"

CSApplication::CSApplication(int &argc, char **argv)
    :QApplication(argc, argv),
     m_wndmgr(new PlasmaWindowManagement()),
     m_model(new QStandardItemModel(this)),
     m_context(new QMenu(0))
{

    m_protocol = new AIPrivateWaylandProtocol;
    connect(m_protocol, &AIPrivateWaylandProtocol::activeChanged,
            this, &CSApplication::privateProtocolReady);

    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("CoreShell");
    setWindowIcon(QIcon::fromTheme("system-file-manager"));

    connect(this, &QApplication::screenAdded,
            this, &CSApplication::addScreen);
    connect(qApp, &QApplication::screenRemoved,
            this, &CSApplication::removeScreen);
    connect(qApp, &QApplication::primaryScreenChanged,
            this, &CSApplication::primaryChanged);

    connect(m_wndmgr, &PlasmaWindowManagement::windowCreated,
            this, &CSApplication::newPlasmaWindow);

    auto aboutSys = m_context->addAction(tr("&About This Computer..."));
    auto about = m_context->addAction(tr("About &CoreShell"));
    aboutSys->setEnabled(false);
    connect(about, &QAction::triggered, this, &CSApplication::aboutApplication);
    m_context->addSeparator();
    auto progman = m_context->addAction(tr("&Program Manager"));
    connect(progman, &QAction::triggered, this, &CSApplication::showProgramManager);
    auto run = m_context->addAction(tr("&Run..."));
    connect(run, &QAction::triggered, this, &CSApplication::run);
    m_context->addSeparator();
    auto settings = m_context->addAction(tr("System &Settings"));
    auto sysmon = m_context->addAction(tr("System &Monitor"));
    auto term = m_context->addAction(tr("&Terminull"));
    connect(settings, &QAction::triggered, this, &CSApplication::launchSettings);
    connect(sysmon, &QAction::triggered, this, &CSApplication::launchSysmon);
    connect(term, &QAction::triggered, this, &CSApplication::launchTerminull);
    m_context->addSeparator();
    auto logoff = m_context->addAction(tr("&Exit Session..."));
    connect(logoff, &QAction::triggered, this, &CSApplication::logoffSession);

    createStandardModelItems();
}

void CSApplication::createWindows()
{
    auto screen = QApplication::primaryScreen();
    auto dw = new CSWindow(screen);
    m_screenWindows.append(dw);
    dw->show();
    if(m_protocol->isActive())
        m_protocol->setDesktopForWindow(dw->windowHandle());
    /*for(auto screen : QApplication::screens())
    {
        auto dw = new CSWindow(screen);
        m_screenWindows.append(dw);
        dw->show();
    }*/
}

void CSApplication::aboutApplication()
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

QStandardItemModel *CSApplication::model()
{
    if(m_model)
        return m_model;

    return nullptr;
}

QMenu *CSApplication::contextMenu()
{
    if(m_context)
        return m_context;

    return nullptr;
}

void CSApplication::showProgramManager()
{
    if(m_pm == nullptr)
    {
        m_pm = new ProgramManager(0);
    }
    m_pm->show();
}

void CSApplication::privateProtocolReady()
{
    for(auto dw : m_screenWindows)
        m_protocol->setDesktopForWindow(dw->windowHandle());
}

void CSApplication::addScreen(QScreen *screen)
{
    Q_UNUSED(screen);
}

void CSApplication::removeScreen(QScreen *screen)
{
    Q_UNUSED(screen);
}

void CSApplication::primaryChanged(QScreen *screen)
{
    Q_UNUSED(screen);
}

void CSApplication::launchSysmon()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSYSMON_APP);
    executeDesktop(df);
}

void CSApplication::launchSettings()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSETTINGS_APP);
    executeDesktop(df);
}

void CSApplication::launchTerminull()
{
    auto df = LSDesktopEntry::findDesktopFile(TERMINULL_APP);
    executeDesktop(df);
}

void CSApplication::run()
{

}

void CSApplication::logoffSession()
{
    QMessageBox msg(QMessageBox::Question, tr("Exit Session?"),
                    tr("Do you want to exit your session? All running applications will be closed."),
                    QMessageBox::Yes|QMessageBox::No);
    if(msg.exec() == QMessageBox::Yes)
    {
        exit();
    }
}

void CSApplication::windowMinimized()
{
    PlasmaWindow *w = qobject_cast<PlasmaWindow*>(sender());
    if(!w)
        return;

    auto test = new QStandardItem(w->windowTitle());
    test->setIcon(QIcon::fromTheme(QLatin1String("utilities-terminal")));
    test->setData("Window");
    test->setData(w->uuid().toString(QUuid::WithoutBraces), Qt::UserRole+2);
    m_model->appendRow(test);
}

void CSApplication::newPlasmaWindow(PlasmaWindow *c)
{
    connect(c, &PlasmaWindow::minimizedChanged,
            this, &CSApplication::windowMinimized);
}

void CSApplication::executeDesktop(const QString &dt, const QStringList args)
{
    Q_UNUSED(args);
    if(!dt.isEmpty())
    {
        auto ds = LSDesktopEntry();
        if(ds.load(dt))
        {
            auto execval = ds.value("Exec").toString();
            if(!execval.isEmpty())
                startProcess(execval);
        }
    }
}

void CSApplication::startProcess(const QString &exec)
{
    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    QProcess *proc = new QProcess(this);
    proc->setProgram(exec);
    proc->start();
    m_processes.append(proc);
}

void CSApplication::createStandardModelItems()
{
    auto root = new QStandardItem(tr("/"));
    root->setIcon(QIcon::fromTheme(QLatin1String("drive-harddisk")));
    root->setData(QUrl("file:///"));
    m_model->appendRow(root);


    auto trash = new QStandardItem(tr("Trash"));
    trash->setIcon(QIcon::fromTheme(QLatin1String("user-trash")));
    trash->setData(QUrl("trash:///"));
    m_model->appendRow(trash);
}

int main(int argc, char *argv[])
{
    CSApplication a(argc, argv);
    a.createWindows();
    return a.exec();
}
