#pragma once

#include <QProcess>

class LSDesktopEntry;
class LSExecutor;
class ExecutorPrivate
{
private:
    friend class LSExecutor;
    LSExecutor *d;
    ExecutorPrivate(LSExecutor *parent);
    ~ExecutorPrivate();
    bool findBestApplication();
    bool createProcess(QString &exec, QStringList args);
    QList<QProcess*> m_processList;
    QStringList m_files;
    QStringList m_arguments;
    LSDesktopEntry *m_dt = nullptr;
};
