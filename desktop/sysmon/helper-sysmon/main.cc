#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDateTime>
#include <QProcess>
#include <sys/time.h>
#include <unistd.h>
#include <hollywood/hollywood.h>

#include <errno.h>
#include <signal.h>

bool checkRoot()
{
    bool root = false;
    if(getuid() == 0)
        root = true;

    if(!root)
        qCritical() << QCoreApplication::translate("sysmon-helper", "This application must run as root.");

    return root;
}

int send_signal(QString pid, QString signal)
{
    if(!checkRoot())
        return -1;

    pid_t mypid = pid.toUInt();
    int mysig = signal.toInt();
    errno = 0;
    int mykill = kill(mypid, mysig);

    if(mykill == 0)
        return 0;

    return errno;
}

bool renice(QString pid, QString nice)
{
    if(!checkRoot())
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    a.setApplicationName("System Monitor Helper");

    QCommandLineParser p;
    p.setApplicationDescription(QCoreApplication::translate("sysmon-helper", "Hollywood System Monitor Root Helper"));
    p.addHelpOption();
    p.addVersionOption();

    p.addOptions({
     {{"n", "nice"},
         QCoreApplication::translate("sysmon-helper", "Re-Nice the process"),
         QCoreApplication::translate("sysmon-helper", "niceness")},
     {{"s", "send-signal"},
         QCoreApplication::translate("sysmon-helper", "Send signal to process"),
         QCoreApplication::translate("sysmon-helper", "signal")},
    });
    p.process(a);

    bool nice = false;
    bool signal = false;
    QString niceness;
    QString signalid;

    if(p.isSet("nice"))
    {
        nice = true;
        if(p.isSet("niceness"))
            niceness = p.value("niceness");
    }

    if(p.isSet("send-signal"))
    {
        signal = true;
        nice = false;
        if(p.isSet("signal"))
            signalid = p.value("signal");
    }

    auto pid = p.positionalArguments().first();
    if(!signal && !nice)
        p.showHelp(0);

    if(nice)
        return renice(pid, niceness);

    if(signal)
        return send_signal(pid, signalid);

    return 0;
}
