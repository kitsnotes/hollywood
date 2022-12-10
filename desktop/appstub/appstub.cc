#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QMimeDatabase>
#include <QMessageBox>
#include <QFile>
#include <hollywood/hollywood.h>

void installJava(const QString &file)
{
    Q_UNUSED(file)
    QMessageBox msg;
    msg.setWindowTitle(QApplication::tr("Hollywood"));
    msg.setText(QApplication::tr("To run this application you will need to install an OpenJDK runtime."));
    msg.setInformativeText(QApplication::tr("This additional software will require %1 disk space and an active connection to the Internet to download required files. Would you like to install this software now?"));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Help);
    int result = msg.exec();
    if(result == QMessageBox::Yes)
    {
        //installApk(OpenJDK);
    }
}

void installWaydroid(const QString &file)
{
    Q_UNUSED(file)
    QMessageBox msg;
    msg.setWindowTitle(QApplication::tr("Hollywood"));
    msg.setText(QApplication::tr("To run this application you will need to install Android support libraries."));
    msg.setInformativeText(QApplication::tr("This additional software will require %1 disk space and an active connection to the Internet to download required files. Would you like to install this software now?"));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Help);
    int result = msg.exec();
    if(result == QMessageBox::Yes)
    {
        //installApk(Waydroid);
    }
}

void installWine(const QString &file)
{
    Q_UNUSED(file)
    QMessageBox msg;
    msg.setWindowTitle(QApplication::tr("Hollywood"));
    msg.setText(QApplication::tr("To run this application you will need to install the Linux Subsystem for Windows."));
    msg.setInformativeText(QApplication::tr("This additional software will require %1 disk space and an active connection to the Internet to download required files. Would you like to install this software now?"));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Help);
    int result = msg.exec();
    if(result == QMessageBox::Yes)
    {
        //installApk(WINE);
    }
}

void unsupportedApplication(const QString &file)
{
    Q_UNUSED(file)
    QMessageBox msg;
    msg.setWindowTitle(QApplication::tr("Hollywood"));
    msg.setText(QApplication::tr("This application can not run on this system."));
    msg.setInformativeText(QApplication::tr("There is no support software available or there is a CPU architecture incompatibility."));
    msg.setIcon(QMessageBox::Critical);
    msg.setStandardButtons(QMessageBox::Ok|QMessageBox::Help);
    msg.exec();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    a.setApplicationName("Application Support Utility");
    a.setWindowIcon(QIcon::fromTheme("applications-all"));

    QCommandLineParser p;
    p.setApplicationDescription(QString("Application Support Utility"));
    p.addHelpOption();
    p.addVersionOption();
    p.process(a);
    QString file;

    if(p.positionalArguments().count() > 0)
        file = p.positionalArguments().first();
    else
    {
        p.showHelp();
        return 0;
    }

    QFile f(file);
    if(!f.exists())
    {
        qInfo() << "You have specified an invalid file.";
        return -1;
    }

    bool handled = false;
    QMimeDatabase d;
    auto mime = d.mimeTypeForFile(file);
    if(mime.name() == "application/x-java-archive")
    {
        installJava(file);
        handled = true;
    }

    if(mime.name() == "application/vnd.android.package-archive")
    {
        a.setWindowIcon(QIcon::fromTheme("android-package-archive"));
        installWaydroid(file);
        handled = true;
    }

    if(mime.name() == "application/x-ms-dos-executable" ||
        mime.name() == "application/x-msi")
    {
        installWine(file);
        handled = true;
    }

    if(!handled)
        unsupportedApplication(file);

    return 0;
}
