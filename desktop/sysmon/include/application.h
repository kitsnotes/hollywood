#ifndef SYSMONAPPLICATION_H
#define SYSMONAPPLICATION_H

#include <QApplication>
#include <QObject>

class SysmonWindow;
class SysmonApplication : public QApplication
{
    Q_OBJECT
public:
    SysmonApplication(int &argc, char **argv);
    static SysmonApplication* instance() { return static_cast<SysmonApplication*>(QApplication::instance()); }
    void createMainSysmon();
    void createLimitedSysmon();
    int sendSignal(pid_t process, int signal);
    bool promptForSignal(pid_t process, const QString &name, int signal, QWidget *parent = nullptr);
private:
    int sendPkexecSignal(pid_t process, int signal);
    SysmonWindow *m_main = nullptr;
};

#endif // SYSMONAPPLICATION_H
