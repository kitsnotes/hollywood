#ifndef TERMINALHOST_H
#define TERMINALHOST_H

#include <QObject>

class QTermWidget;
class TerminalProfile;
class TerminalHost : public QObject
{
    Q_OBJECT
public:
    ~TerminalHost();
    static TerminalHost* create(const QString &profile, QWidget* parent);
    QByteArray id();
    QTermWidget* widget();
    TerminalProfile* profile();
    bool canClose();
    bool hasBlockingTasks();
    QString generateTabTitle();
    QString generateWindowTitle();
    void setProfile(TerminalProfile* profile);
private:
    explicit TerminalHost(TerminalProfile *profile, QWidget *parent = nullptr);
signals:
    void requestClose();
    void windowTitleChanged(const QString &title);
    void tabTitleChanged(const QString &title);
private slots:
    void terminalTitleChanged();
    void profileChanged();
    void terminalFinished();
    QString getCwd();
    QString getProcessLoop(uint pid);
    QString getProcessName(uint pid);
private:
    QByteArray m_id;
    QTermWidget *m_widget;
    TerminalProfile *m_profile;
};

#endif // TERMINALHOST_H
