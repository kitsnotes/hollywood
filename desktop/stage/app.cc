#include "app.h"
#include "stagehost.h"
#include "progman.h"
#include "wndmgmt.h"

#include <hollywood/hollywood.h>
#include <hollywood/layershellwindow.h>

#include <desktopentry.h>

#include <QIcon>

#define HWSETTINGS_APP      "org.originull.hwsettings.desktop"
#define HWSYSMON_APP        "org.originull.hwsysmon.desktop"
#define TERMINULL_APP       "org.originull.terminull.desktop"

StageApplication::StageApplication(int &argc, char **argv)
    :QApplication(argc, argv),
     m_wndmgr(new PlasmaWindowManagement()),
     m_model(new QStandardItemModel(this))
{
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("CoreShell");
    setWindowIcon(QIcon::fromTheme("system-file-manager"));

    connect(m_wndmgr, &PlasmaWindowManagement::windowCreated,
            this, &StageApplication::newPlasmaWindow);
}

void StageApplication::createWindows()
{
    m_host = new StageHost(primaryScreen());
    m_host->show();
    connect(m_wndmgr, &PlasmaWindowManagement::stackingOrderChanaged, m_host, &StageHost::stackingOrderChanged);
}

void StageApplication::aboutApplication()
{
    // This is assuming that this comes from an action
    // that would be a child of the mainwindow

}

QStandardItemModel *StageApplication::model()
{
    if(m_model)
        return m_model;

    return nullptr;
}

void StageApplication::showProgramManager()
{
    if(m_pm == nullptr)
    {
        m_pm = new ProgramManager(0);
    }
    m_pm->show();
}

void StageApplication::privateProtocolReady()
{


}

void StageApplication::launchSysmon()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSYSMON_APP);
    //executeDesktop(df);
}

void StageApplication::launchSettings()
{
    auto df = LSDesktopEntry::findDesktopFile(HWSETTINGS_APP);
    //executeDesktop(df);
}

void StageApplication::launchTerminull()
{
    auto df = LSDesktopEntry::findDesktopFile(TERMINULL_APP);
    //executeDesktop(df);
}

void StageApplication::run()
{

}

void StageApplication::logoffSession()
{
    QMessageBox msg(QMessageBox::Question, tr("Exit Session?"),
                    tr("Do you want to exit your session? All running applications will be closed."),
                    QMessageBox::Yes|QMessageBox::No);
    if(msg.exec() == QMessageBox::Yes)
    {
        exit();
    }
}

void StageApplication::windowMinimized()
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

void StageApplication::newPlasmaWindow(PlasmaWindow *c)
{
    m_host->createWindowButton(c);
}


void StageApplication::startProcess(const QString &exec)
{
    qputenv("QT_QPA_PLATFORM", "hollywood");
    qputenv("QT_QPA_PLATFORMTHEME", "hollywood");
    QProcess *proc = new QProcess(this);
    proc->setProgram(exec);
    proc->start();
    m_processes.append(proc);
}

int main(int argc, char *argv[])
{
    StageApplication a(argc, argv);
    a.createWindows();
    return a.exec();
}
