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
    QString tabTitle();
    QString windowTitle();
    void setProfile(TerminalProfile* profile);
    bool canCopy();
    bool hasSelection();
private:
    explicit TerminalHost(TerminalProfile *profile, QWidget *parent = nullptr);
signals:
    void requestClose();
    void titleChanged();
    void canCopyChanged(bool canCopy);
private slots:
    void terminalTitleChanged();
    void profileChanged();
    void terminalFinished();
    QString getCwd();
    QString getProcessLoop(uint pid);
    QString getProcessName(uint pid);
    void copyAvailable(bool canCopy);
private:
    QByteArray m_id;
    QTermWidget *m_widget;
    TerminalProfile *m_profile;
    bool m_copy = false;
};

#endif // TERMINALHOST_H
