#ifndef LSEXECUTOR_H
#define LSEXECUTOR_H

#include <QObject>
#include "libshell_int.h"

class ExecutorPrivate;
class LSDesktopEntry;
class LIBSHELL_EXPORT LSExecutor : public QObject
{
    Q_OBJECT
public:
    explicit LSExecutor(QObject *parent = nullptr);
    ~LSExecutor();
    bool setDesktopFile(const QString &desktop);
    void setOpenFiles(const QStringList &files);
    void setArguments(const QStringList &arguments);
    void addArgument(const QString &argument);
    bool launch();
signals:

private:
    ExecutorPrivate *d;
};

#endif // LSEXECUTOR_H
