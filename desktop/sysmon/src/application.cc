#include "application.h"
#include "window.h"

#include <QProcess>
#include <hollywood/hollywood.h>
#include <signal.h>
#include <errno.h>
#include <QWidget>
#include <QMessageBox>

SysmonApplication::SysmonApplication(int &argc, char **argv)
    :QApplication(argc, argv)
{
    setApplicationName("System Monitor");
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setWindowIcon(QIcon::fromTheme("utilities-system-monitor"));
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setAttribute(Qt::AA_DontShowIconsInMenus, true);
}

void SysmonApplication::createMainSysmon()
{
    m_main = new SysmonWindow();
    m_main->show();
}

void SysmonApplication::createLimitedSysmon()
{

}

int SysmonApplication::sendSignal(pid_t process, int signal)
{
    errno = 0;
    int mykill = kill(process, signal);
    if(errno == EPERM)
        return sendPkexecSignal(process, signal);

    if(mykill == 0)
        return mykill;

    return errno;
}

bool SysmonApplication::promptForSignal(pid_t process, const QString &name, int signal, QWidget *parent)
{
    auto msgparent = parent == nullptr ? m_main : parent;
    auto msg = tr("Are you sure you want to send signal %1 to process %2 (PID: %3)?")
            .arg(QString::number(signal), name, QString::number(process));

    if(signal == SIGTERM)
    {
        msg = tr("Are you sure you want to terminate the process %1 (PID: %2)?")
                    .arg(name, QString::number(process));
    }

    if(signal == SIGKILL)
    {
        msg = tr("Are you sure you want to force kill the process %1 (PID: %2)?")
                    .arg(name, QString::number(process));
    }

    QMessageBox prompt(QMessageBox::Question,
                       tr("Send Signal to Process %1 %2").arg(QString::number(process), name),
                       msg, QMessageBox::Yes|QMessageBox::No, msgparent);

    int msgresult = prompt.exec();
    if(msgresult == QMessageBox::No)
        return false;

    int result = SysmonApplication::instance()->sendSignal(process, signal);
    if(result == 0)
        return true;

    QString failmsg = "Signal failed.";
    QMessageBox notify(QMessageBox::Critical,
                       tr("Signal Failed"),
                       failmsg, QMessageBox::Ok, msgparent);

    notify.exec();

    return false;
}

int SysmonApplication::sendPkexecSignal(pid_t process, int signal)
{
    QProcess proc;
    QStringList args;
    args << QLatin1String("--disable-internal-agent")
         << "/usr/libexec/hollywood/helpers/sysmon"
         << QLatin1String("--send-signal=%1").arg(QString::number(signal))
         << QString::number(process);

    proc.start(QLatin1String("pkexec"), args);
    proc.waitForFinished(-1);
    qDebug() << proc.readAllStandardError();
    auto exit = proc.exitCode();

    return exit;
}


int main(int argc, char *argv[])
{
    SysmonApplication a(argc, argv);
    a.createMainSysmon();
    return a.exec();
}
