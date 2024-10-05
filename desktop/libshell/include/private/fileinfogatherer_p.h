/****************************************************************************
** Copyright (C) The Cat and Arion OS team.
** Portions Copyright (C) 2015 The Qt Company Ltd.
**
** This file is originated from the QtWidgets module
** of the Qt Toolkit version 5.6 - the last version to be licensed
** under LGPL 2.1.
**
** This file, like the rest of libshell is licensed under LGPL 2.1.
****************************************************************************/

#pragma once

#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qfilesystemwatcher.h>
#include <qfileiconprovider.h>
#include <qpair.h>
#include <qstack.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qelapsedtimer.h>

#include "fileinfo.h"
#include "hwfileiconprovider.h"

class HWFileIconProvider;
class LSFileInfoGatherer : public QThread
{
Q_OBJECT

Q_SIGNALS:
    void updates(const QString &directory, const QVector<QPair<QString, QFileInfo> > &updates);
    void newListOfFiles(const QString &directory, const QStringList &listOfFiles) const;
    void nameResolved(const QString &fileName, const QString &resolvedName) const;
    void directoryLoaded(const QString &path);

public:
    explicit LSFileInfoGatherer(QObject *parent = 0);
    ~LSFileInfoGatherer();

    QStringList watchedFiles() const;
    QStringList watchedDirectories() const;
    void watchPaths(const QStringList &paths);
    void unwatchPaths(const QStringList &paths);

    bool isWatching() const;
    void setWatching(bool v);

    // only callable from this->thread():
    void clear();
    void removePath(const QString &path);
    LSExtendedFileInfo getInfo(const QFileInfo &info) const;
    HWFileIconProvider *iconProvider() const;
    bool resolveSymlinks() const;

public Q_SLOTS:
    void list(const QString &directoryPath);
    void fetchExtendedInformation(const QString &path, const QStringList &files);
    void updateFile(const QString &path);
    void setResolveSymlinks(bool enable);
    void setIconProvider(HWFileIconProvider *provider);

private:
    void run() Q_DECL_OVERRIDE;
    // called by run():
    void getFileInfos(const QString &path, const QStringList &files);
    void fetch(const QFileInfo &info, QElapsedTimer &base, bool &firstTime, QVector<QPair<QString, QFileInfo> > &updatedFiles, const QString &path);

private:
    mutable QMutex mutex;
    // begin protected by mutex
    QWaitCondition condition;
    QStack<QString> path;
    QStack<QStringList> files;
    // end protected by mutex
    QAtomicInt abort;

#ifndef QT_NO_FILESYSTEMWATCHER
    QFileSystemWatcher *watcher;
#endif
#ifdef Q_OS_WIN
    bool m_resolveSymlinks; // not accessed by run()
#endif
    HWFileIconProvider *m_iconProvider; // not accessed by run()
    HWFileIconProvider defaultProvider;
    bool m_watching = true;
};
