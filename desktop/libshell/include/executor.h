// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

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
    void terminate();
    void kill();
    uint runningProcesses() const;
signals:
    void terminated(int signal);
private:
    ExecutorPrivate *p;
};
