#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDateTime>
#include <QProcess>
#include <sys/time.h>
#include <unistd.h>
#include <hollywood/hollywood.h>

bool checkRoot()
{
    bool root = false;
    if(getuid() == 0)
        root = true;

    if(!root)
        qCritical() << QCoreApplication::translate("dthelper", "This application must run as root.");

    return root;
}

bool update_ntpd_boot(bool useNtp)
{
    QString method = QLatin1String("add");
    if(!useNtp)
        method = QLatin1String("del");
    QProcess p;
    p.setProgram(HOLLYWOOD_SERVICE_UPDATE);
    p.setArguments(QStringList() << method << HOLLYWOOD_SERVICE_NTPD
                                 << QLatin1String("boot"));

    p.start();
    while(p.waitForFinished(150))
    {
        if(p.state() == QProcess::NotRunning)
            continue;
    }
    if(p.exitCode() == 0)
        return true;

    return false;
}

bool start_stop_ntpd(bool start)
{
    QString method = QLatin1String("start");
    if(!start)
        method = QLatin1String("stop");
    QProcess p;
    p.setProgram(HOLLYWOOD_SERVICE_SVC);
    p.setArguments(QStringList() << HOLLYWOOD_SERVICE_NTPD << method);

    p.start();
    while(p.waitForFinished(150))
    {
        if(p.state() == QProcess::NotRunning)
            continue;
    }
    if(p.exitCode() == 0)
        return true;

    return false;
}

bool configure_ntp(bool useNtp, QString ntpServer = QString())
{
    if(!checkRoot())
        return false;

    if(!ntpServer.isEmpty())
    {
        // insert our ntp server into ntpd.conf
    }

    bool val = true;
    val = update_ntpd_boot(useNtp);
    if(val)
        val = start_stop_ntpd(useNtp);

    return val;
}

bool configure_date(QString date)
{
    if(!checkRoot())
        return false;

    QDateTime dt = QDateTime::fromString(date);
    if(!dt.isValid())
    {
        qCritical() << QCoreApplication::translate("dthelper", "Invalid time string specified.");
        return false;
    }

    timeval tv;
    tv.tv_sec = dt.toSecsSinceEpoch();
    tv.tv_usec = 0;

    int res = settimeofday(&tv, NULL);
    if(res == 0)
        return true;

    qCritical() << QCoreApplication::translate("dthelper", "There was a system error with setting time.");
    return false;
}

bool configure_tz(QString tz)
{
    if(!checkRoot())
        return false;

    QFile f(QString("/usr/share/zoneinfo"));
    if(!f.exists())
    {
        qCritical() << QCoreApplication::translate("dthelper", "Invalid timezone specified.");
        return false;
    }

    QFile lt("/etc/localtime");
    if(lt.exists())
        lt.remove();

    if(!f.link("/etc/localtime"))
    {
        qCritical() << QCoreApplication::translate("dthelper", "Failed to create symbolic link to /etc/localtime.");
        return false;
    }

    QFile etz("/etc/timezone");
    if(etz.open(QFile::WriteOnly))
    {
        etz.write(tz.toUtf8());
        etz.close();
    }
    else
        qWarning() << QCoreApplication::translate("dthelper", "Failed to insert timezone in /etc/timezone.  This should not be a file relied upon but applications are weird.  Not treating as critical.");

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    a.setApplicationName("Date & Time Helper");

    QCommandLineParser p;
    p.setApplicationDescription(QString("Hollywood Date/Time Root Helper"));
    p.addHelpOption();
    p.addVersionOption();

    p.addOptions({
     {{"n", "use-ntp"},
         QCoreApplication::translate("dthelper", "Use the Network Time Protocol to set time.")},
     {{"s", "ntp-server"},
         QCoreApplication::translate("dthelper", "Set the server to use for NTP (IP or DNS)."),
         QCoreApplication::translate("dthelper", "server")},
     {{"t", "timezone"},
         QCoreApplication::translate("dthelper", "Set the system timezone."),
         QCoreApplication::translate("dthelper", "timezone")},
     {{"d", "date"},
         QCoreApplication::translate("dthelper", "Set the system date & time."),
         QCoreApplication::translate("dthelper", "datetime")},
      {{"l", "localtime"},
         QCoreApplication::translate("dthelper", "Supplied system date & time is local time.")},
    });
    p.process(a);

    bool tz = false;
    bool date = false;
    bool ntp = false;
    QString ntpServer;
    QString dateTime;
    QString timeZone;

    if(p.isSet("date"))
    {
        date = true;
        if(p.isSet("date"))
            ntpServer = p.value("date");
    }

    if(p.isSet("use-ntp"))
    {
        ntp = true;
        date = false;
        if(p.isSet("ntp-server"))
            ntpServer = p.value("ntp-server");
    }

    if(p.isSet("timezone"))
    {
        tz = true;
        timeZone = p.value("timezone");
    }

    if(!tz && !date && !ntp)
        p.showHelp(0);

    if(tz)
        return configure_tz(timeZone);

    if(ntp)
        return configure_ntp(true, ntpServer);

    if(date)
    {
        configure_ntp(false);
    }

    return 0;
}
