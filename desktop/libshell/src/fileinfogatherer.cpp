/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "fileinfogatherer_p.h"
#include <qdebug.h>
#include <qdiriterator.h>
#  include <unistd.h>
#  include <sys/types.h>
#include "fileinfo.h"

#ifdef QT_BUILD_INTERNAL
static QBasicAtomicInt fetchedRoot = Q_BASIC_ATOMIC_INITIALIZER(false);
Q_AUTOTEST_EXPORT void qt_test_resetFetchedRoot()
{
    fetchedRoot.store(false);
}

Q_AUTOTEST_EXPORT bool qt_test_isFetchedRoot()
{
    return fetchedRoot.load();
}
#endif

/*!
    Creates thread
*/
LSFileInfoGatherer::LSFileInfoGatherer(QObject *parent)
    : QThread(parent), abort(false),
#ifndef QT_NO_FILESYSTEMWATCHER
      watcher(0),
#endif
#ifdef Q_OS_WIN
      m_resolveSymlinks(true),
#endif
      m_iconProvider(&defaultProvider)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    watcher = new QFileSystemWatcher(this);
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(list(QString)));
    connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateFile(QString)));
#endif
    start(LowPriority);
}

/*!
    Destroys thread
*/
LSFileInfoGatherer::~LSFileInfoGatherer()
{
    abort.storeRelaxed(true);
    QMutexLocker locker(&mutex);
    condition.wakeAll();
    locker.unlock();
    wait();
}

QStringList LSFileInfoGatherer::watchedFiles() const
{
#if QT_CONFIG(filesystemwatcher)
    if (watcher)
        return watcher->files();
#endif
    return {};
}

QStringList LSFileInfoGatherer::watchedDirectories() const
{
#if QT_CONFIG(filesystemwatcher)
    if (watcher)
        return watcher->directories();
#endif
    return {};
}

void LSFileInfoGatherer::watchPaths(const QStringList &paths)
{
#if QT_CONFIG(filesystemwatcher)
    if (watcher && !paths.isEmpty())
        watcher->removePaths(paths);
#else
    Q_UNUSED(paths)
#endif
}

void LSFileInfoGatherer::unwatchPaths(const QStringList &paths)
{
#if QT_CONFIG(filesystemwatcher)
    if (watcher && !paths.isEmpty())
        watcher->removePaths(paths);
#else
    Q_UNUSED(paths);
#endif
}

bool LSFileInfoGatherer::isWatching() const
{
    bool result = false;
#if QT_CONFIG(filesystemwatcher)
    QMutexLocker locker(&mutex);
    result = m_watching;
#endif
    return result;
}

void LSFileInfoGatherer::setWatching(bool v)
{
    QMutexLocker locker(&mutex);
    /* if (v != m_watching) {
        if (!v) {
            delete watcher;
            watcher = nullptr;
        }
        m_watching = v;
    } */
}

void LSFileInfoGatherer::setResolveSymlinks(bool enable)
{
    Q_UNUSED(enable);
#ifdef Q_OS_WIN
    m_resolveSymlinks = enable;
#endif
}

bool LSFileInfoGatherer::resolveSymlinks() const
{
#ifdef Q_OS_WIN
    return m_resolveSymlinks;
#else
    return false;
#endif
}

void LSFileInfoGatherer::setIconProvider(HWFileIconProvider *provider)
{
    m_iconProvider = provider;
}

HWFileIconProvider *LSFileInfoGatherer::iconProvider() const
{
    return m_iconProvider;
}

/*!
    Fetch extended information for all \a files in \a path

    \sa updateFile(), update(), resolvedName()
*/
void LSFileInfoGatherer::fetchExtendedInformation(const QString &path, const QStringList &files)
{
    QMutexLocker locker(&mutex);
    // See if we already have this dir/file in our queue
    int loc = this->path.lastIndexOf(path);
    while (loc > 0)  {
        if (this->files.at(loc) == files) {
            return;
        }
        loc = this->path.lastIndexOf(path, loc - 1);
    }
    this->path.push(path);
    this->files.push(files);
    condition.wakeAll();

#ifndef QT_NO_FILESYSTEMWATCHER
    if (files.isEmpty()
        && !path.isEmpty()
        && !path.startsWith(QLatin1String("//")) /*don't watch UNC path*/) {
        if (!watcher->directories().contains(path))
            watcher->addPath(path);
    }
#endif
}

/*!
    Fetch extended information for all \a filePath

    \sa fetchExtendedInformation()
*/
void LSFileInfoGatherer::updateFile(const QString &filePath)
{
    QString dir = filePath.mid(0, filePath.lastIndexOf(QDir::separator()));
    QString fileName = filePath.mid(dir.length() + 1);
    fetchExtendedInformation(dir, QStringList(fileName));
}

/*
    List all files in \a directoryPath

    \sa listed()
*/
void LSFileInfoGatherer::clear()
{
#ifndef QT_NO_FILESYSTEMWATCHER
    QMutexLocker locker(&mutex);
    watcher->removePaths(watcher->files());
    watcher->removePaths(watcher->directories());
#endif
}

/*
    Remove a \a path from the watcher

    \sa listed()
*/
void LSFileInfoGatherer::removePath(const QString &path)
{
#ifndef QT_NO_FILESYSTEMWATCHER
    QMutexLocker locker(&mutex);
    watcher->removePath(path);
#else
    Q_UNUSED(path);
#endif
}

/*
    List all files in \a directoryPath

    \sa listed()
*/
void LSFileInfoGatherer::list(const QString &directoryPath)
{
    fetchExtendedInformation(directoryPath, QStringList());
}

/*
    Until aborted wait to fetch a directory or files
*/
void LSFileInfoGatherer::run()
{
    forever {
        QMutexLocker locker(&mutex);
        while (!abort.loadRelaxed() && path.isEmpty())
            condition.wait(&mutex);
        if (abort.loadRelaxed())
            return;
        const QString thisPath = path.front();
        path.pop_front();
        const QStringList thisList = files.front();
        files.pop_front();
        locker.unlock();

        getFileInfos(thisPath, thisList);
    }
}

LSExtendedFileInfo LSFileInfoGatherer::getInfo(const QFileInfo &fileInfo) const
{
    LSExtendedFileInfo info(fileInfo);
    info.icon = m_iconProvider->icon(fileInfo);
    info.displayType = m_iconProvider->type(fileInfo);
#ifndef QT_NO_FILESYSTEMWATCHER
    // ### Not ready to listen all modifications
    #if 0
        // Enable the next two commented out lines to get updates when the file sizes change...
        if (!fileInfo.exists() && !fileInfo.isSymLink()) {
            info.size = -1;
            //watcher->removePath(fileInfo.absoluteFilePath());
        } else {
            if (!fileInfo.absoluteFilePath().isEmpty() && fileInfo.exists() && fileInfo.isReadable()
                && !watcher->files().contains(fileInfo.absoluteFilePath())) {
                //watcher->addPath(fileInfo.absoluteFilePath());
            }
        }
    #endif
#endif

    return info;
}

static QString translateDriveName(const QFileInfo &drive)
{
    QString driveName = drive.absoluteFilePath();
    return driveName;
}

/*
    Get specific file info's, batch the files so update when we have 100
    items and every 200ms after that
 */
void LSFileInfoGatherer::getFileInfos(const QString &path, const QStringList &files)
{
    // List drives
    if (path.isEmpty()) {
#ifdef QT_BUILD_INTERNAL
        fetchedRoot.store(true);
#endif
        QFileInfoList infoList;
        if (files.isEmpty()) {
            infoList = QDir::drives();
        } else {
            infoList.reserve(files.count());
            for (int i = 0; i < files.count(); ++i)
                infoList << QFileInfo(files.at(i));
        }
        for (int i = infoList.count() - 1; i >= 0; --i) {
            QString driveName = translateDriveName(infoList.at(i));
            QVector<QPair<QString,QFileInfo> > updatedFiles;
            updatedFiles.append(QPair<QString,QFileInfo>(driveName, infoList.at(i)));
            emit updates(path, updatedFiles);
        }
        return;
    }

    QElapsedTimer base;
    base.start();
    QFileInfo fileInfo;
    bool firstTime = true;
    QVector<QPair<QString, QFileInfo> > updatedFiles;
    QStringList filesToCheck = files;

    QString itPath = QDir::fromNativeSeparators(files.isEmpty() ? path : QLatin1String(""));
    QDirIterator dirIt(itPath, QDir::AllEntries | QDir::System | QDir::Hidden);
    QStringList allFiles;
    while (!abort.loadRelaxed() && dirIt.hasNext()) {
        dirIt.next();
        fileInfo = dirIt.fileInfo();
        allFiles.append(fileInfo.fileName());
        fetch(fileInfo, base, firstTime, updatedFiles, path);
    }
    if (!allFiles.isEmpty())
        emit newListOfFiles(path, allFiles);

    QStringList::const_iterator filesIt = filesToCheck.constBegin();
    while (!abort.loadRelaxed() && filesIt != filesToCheck.constEnd()) {
        fileInfo.setFile(path + QDir::separator() + *filesIt);
        ++filesIt;
        fetch(fileInfo, base, firstTime, updatedFiles, path);
    }
    if (!updatedFiles.isEmpty())
        emit updates(path, updatedFiles);
    emit directoryLoaded(path);
}

void LSFileInfoGatherer::fetch(const QFileInfo &fileInfo, QElapsedTimer &base, bool &firstTime, QVector<QPair<QString, QFileInfo> > &updatedFiles, const QString &path) {
    updatedFiles.append(QPair<QString, QFileInfo>(fileInfo.fileName(), fileInfo));
    QElapsedTimer current;
    current.start();
    if ((firstTime && updatedFiles.count() > 100) || base.msecsTo(current) > 1000) {
        emit updates(path, updatedFiles);
        updatedFiles.clear();
        base = current;
        firstTime = false;
    }
}


//#include "moc_fileinfogatherer_p.cpp"
