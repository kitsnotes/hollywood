#ifndef EXECUTOR_P_H
#define EXECUTOR_P_H

#include <QProcess>

class LSDesktopEntry;
class LSExecutor;
class ExecutorPrivate
{
private:
    friend class LSExecutor;
    LSExecutor *p;
    ExecutorPrivate(LSExecutor *parent);
    ~ExecutorPrivate();
    bool findBestApplication();
    bool createProcess(QString &exec, QStringList args);
    QList<QProcess*> m_processList;
    QStringList m_files;
    QStringList m_arguments;
    LSDesktopEntry *m_dt = nullptr;
};

#endif // EXECUTOR_P_H
