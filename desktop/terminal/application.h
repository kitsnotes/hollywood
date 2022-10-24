#ifndef TERMINALAPPLICATION_H
#define TERMINALAPPLICATION_H

#include <QObject>
#include <QApplication>


class TerminalProfile;
class TerminalWindow;
class TerminalApplication : public QApplication
{
    Q_OBJECT
public:
    TerminalApplication(int &argc, char **argv);
    static TerminalApplication* instance() { return static_cast<TerminalApplication*>(QApplication::instance()); }
    QList<TerminalProfile*> profiles();
    TerminalProfile* profileByName(const QString &name);
    QString defaultProfileName();
    bool createProfile(const QString &name);
    bool removeProfile(const QString &name);
signals:
    void settingsChanged();
    void profilesChanged();
    void profileRemoved(const QString &name);
public slots:
    void newWindow();
private slots:
    void windowClosed();
private:
    void loadProfiles();
private:
    QStringList m_args;
    QList<TerminalWindow*> m_windows;
    QList<TerminalProfile*> m_profiles;
};

#endif // TERMINALAPPLICATION_H
