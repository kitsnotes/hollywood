#include <QApplication>
#include <QIcon>
#include <pulsecontrol.h>
#include <hollywood/hollywood.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef HW_BUILD_VER
    a.setApplicationVersion(QString("%1.%2").arg(HOLLYWOOD_OS_VERSION, QString::number(HW_BUILD_VER)));
#else
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
#endif
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    a.setApplicationName("Volume Control");
    a.setWindowIcon(QIcon::fromTheme("multimedia-volume-control"));
    a.setDesktopFileName("org.originull.volctl");
    PulseControl *pc = new PulseControl;
    pc->showMainWindow();
    return a.exec();
}
