// Hollywood Shell Library
// (C) 2023 Originull Software
// This class derived from Qt Solutions
// (C) 2009 Nokia Corporation
// SPDX-License-Identifier: LGPL-2.1

#include "fileoperation.h"
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QStack>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QMetaType>
#include <QDebug>

#include "trashmodel.h"

OperationThread::OperationThread(FileOperation *fileCopier)
    : QThread(QCoreApplication::instance()),
      copier(fileCopier),
      waitingForInteraction(false),
      stopRequest(false),
      skipAllRequest(false),
      overwriteAllRequest(false),
      cancelRequest(false),
      currentId(-1),
      autoReset(true)
{
    qRegisterMetaType<FileOperation::Error>("FileCopier::Error");
    connect(this, SIGNAL(error(int, QtFileCopier::Error, bool)),
                copier, SLOT(copyError(int, QtFileCopier::Error, bool)));
    connect(this, SIGNAL(started(int)),
                copier, SLOT(copyStarted(int)));
    connect(this, SIGNAL(dataTransferProgress(int, qint64)),
                copier, SIGNAL(dataTransferProgress(int, qint64)));
    connect(this, SIGNAL(finished(int, bool)),
                copier, SLOT(copyFinished(int, bool)));
    connect(this, SIGNAL(canceled()),
                copier, SLOT(copyCanceled()));
    connect(copier, SIGNAL(destroyed()),
                this, SLOT(copierDestroyed()));
}

OperationThread::~OperationThread()
{
    if (isRunning()) {
        wait();
    }
}

void OperationThread::copierDestroyed()
{
    QMutexLocker l(&mutex);
    stopRequest = true;
    newCopyCondition.wakeOne();
    interactionCondition.wakeOne();
}

void OperationThread::restart()
{
    start();
    newCopyCondition.wakeOne();
}

void OperationThread::copy(int id, const CopyRequest &request)
{
    Request r(request);
    QMutexLocker l(&mutex);
    requestQueue[id] = r;
//    newCopyCondition.wakeOne();
}

void OperationThread::copy(const QMap<int, CopyRequest> &requests)
{
    QMap<int, CopyRequest>::ConstIterator it = requests.constBegin();
    QMutexLocker l(&mutex);
    while (it != requests.constEnd()) {
        Request r(it.value());
        requestQueue[it.key()] = r;
        it++;
    }
//    newCopyCondition.wakeOne();
}

void OperationThread::cancelChildRequests(int id)
{
    QMap<int, Request>::Iterator it = requestQueue.find(id);
    if (it != requestQueue.end()) {
        Request &r = it.value();
        r.canceled = true;
        QListIterator<int> itChild(r.request.childrenQueue);
        while (itChild.hasNext())
            cancelChildRequests(itChild.next());
    }
}

void OperationThread::overwriteChildRequests(int id)
{
    QMap<int, Request>::Iterator it = requestQueue.find(id);
    if (it != requestQueue.end()) {
        Request &r = it.value();
        r.overwrite = true;
        QListIterator<int> itChild(r.request.childrenQueue);
        while (itChild.hasNext())
            overwriteChildRequests(itChild.next());
    }
}

void OperationThread::cancel()
{
    QMutexLocker l(&mutex);
    QMutableMapIterator<int, Request> it(requestQueue);
    while (it.hasNext())
        it.next().value().canceled = true;
    cancelRequest = true;
    /*
    // if waitingForInteraction is true wake must be done by retry other method.
    if (waitingForInteraction)
        interactionCondition.wakeOne();
    */
}

void OperationThread::cancel(int id)
{
    QMutexLocker l(&mutex);
    cancelChildRequests(id);
    /*
    if (waitingForInteraction && currentId == id)
        interactionCondition.wakeOne();
    */
}

void OperationThread::skip()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    cancelChildRequests(currentId);
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void OperationThread::skipAll()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    cancelChildRequests(currentId);
    skipAllRequest = true;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void OperationThread::overwrite()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    overwriteChildRequests(currentId);
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void OperationThread::overwriteAll()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    overwriteAllRequest = true;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void OperationThread::retry()
{
    QMutexLocker l(&mutex);
    if (!waitingForInteraction)
        return;
    interactionCondition.wakeOne();
    waitingForInteraction = false;
}

void OperationThread::resetOverwrite()
{
    QMutexLocker l(&mutex);
    overwriteAllRequest = true;
}

void OperationThread::resetSkip()
{
    QMutexLocker l(&mutex);
    skipAllError.clear();
}

void OperationThread::setAutoReset(bool on)
{
    QMutexLocker l(&mutex);
    autoReset = on;
}

void OperationThread::progress()
{
    progressRequest = 1;
}

struct ChainNode {
    ChainNode(ChainNode *nextInChain) {
        n = nextInChain;
        err = FileOperation::NoError;
    }
    virtual ~ChainNode() {
        if (n)
            delete n;
    }
    FileOperation::Error error() const {
        if (n)
            return n->error();
        return err;
    }
    virtual bool handle() {
        if (n)
            return n->handle();
        return false;
    }
protected:
    virtual CopyRequest &request() {
        return n->request();
    }
    virtual OperationThread *thread() const {
        if (n)
            return n->thread();
        return 0;
    }
    virtual int currentId() const {
        if (n)
            return n->currentId();
        return -1;
    }
    void setError(FileOperation::Error e) {
        if (n)
            n->setError(e);
        else
            err = e;
    }
private:
    ChainNode *n;
    FileOperation::Error err;
};

struct CanceledNode : public ChainNode {
    CanceledNode(ChainNode *nextInChain, bool canceled) : ChainNode(nextInChain) {
        c = canceled;
    }
    bool handle() {
        if (c) {
            CopyRequest &r = request();
            if (r.dir) {
                thread()->lockCancelChildren(currentId());
                //while (!r.childrenQueue.isEmpty())
                //    thread()->cancelChildren(r.childrenQueue.dequeue());
            }

            setError(FileOperation::Canceled);
            return true;
        }
        return ChainNode::handle();
    }
private:
    bool c;
};

struct SourceExistsNode : public ChainNode {
    SourceExistsNode(ChainNode *nextInChain) : ChainNode(nextInChain) {}
    bool handle() {
        CopyRequest &r = request();
        if(r.source.scheme() == "file")
        {
            QFileInfo fis(r.source.toLocalFile());
            if (!fis.exists() && !fis.isSymLink()) {
                setError(FileOperation::SourceNotExists);
                return false;
            }
            return ChainNode::handle();
        }
        return false;
    }
};

struct OverwriteNode : public ChainNode {
    OverwriteNode(ChainNode *nextInChain, bool overwrite) : ChainNode(nextInChain) {
        o = overwrite;
    }
    bool handle() {
        CopyRequest &r = request();
        if(r.dest.scheme() == "file")
        {
            QFileInfo fid(r.dest.toLocalFile());
            bool overwrite = r.copyFlags & FileOperation::NonInteractive ?
                    true : // true is default for non interactive mode
                    o;
            if ((fid.exists() || fid.isSymLink()) && overwrite == false) {
                setError(FileOperation::DestinationExists);
                return false;
            }
    #if 0
            if (!fis.isDir() || fis.isSymLink()) {
                QFileInfo fid(dest());
                if ((fid.exists() || fid.isSymLink()) && o == false) {
                    setError(QtFileCopier::DestinationFileExists);
                    return false;
                }
            }
    #endif
            return ChainNode::handle();
        }

        return false;
    }
private:
    bool o;
};

struct MakeLinksNode : public ChainNode {
    MakeLinksNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        if(r.source.scheme() != "file")
            return false;
        if(r.source.scheme() != "file")
            return false;

        if (r.copyFlags & FileOperation::MakeLinks) {
            QFileInfo fis(r.source.toLocalFile());
            QFileInfo fid(r.dest.toLocalFile());
            QDir dir = fid.dir();
            QString linkName = dir.relativeFilePath(fis.filePath());
            QFile sourceFile(linkName);
            if (sourceFile.link(r.dest.toLocalFile()))
                return true;
            setError(FileOperation::CannotCreateSymLink);
            return false;
        }
        return ChainNode::handle();
    }
};

struct FollowLinksNode : public ChainNode {
    FollowLinksNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
#if QT_VERSION >= 0x060000
        CopyRequest &r = request();
        QFileInfo fis(r.source.toLocalFile());
        if (fis.isSymLink() && !(r.copyFlags & FileOperation::FollowLinks)) {
            QFileInfo fil(fis.readSymLink());
            QString linkName = fil.filePath();
            if (fil.isAbsolute()) {
                QDir dir = fis.dir();
                linkName = dir.relativeFilePath(linkName);
            }
            QFile linkTarget(linkName);
            if (linkTarget.link(r.dest.toLocalFile()))
                return true;
            setError(FileOperation::CannotCreateSymLink);
            return false;
        }
#endif
        return ChainNode::handle();
    }
};

struct RenameNode : public ChainNode {
    RenameNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        if (r.move) {
            QFileInfo fis(r.source.toLocalFile());
            QDir dir = fis.dir();
            if (!(r.copyFlags & FileOperation::FollowLinks) || (!r.dir && !fis.isSymLink())) {
                if (dir.rename(fis.fileName(), r.dest.toLocalFile())) {
                    QFileInfo fid(r.dest.toLocalFile());
                    if (r.dir)
                        while (!r.childrenQueue.isEmpty())
                            thread()->renameChildren(r.childrenQueue.dequeue());
                    else
                        thread()->emitProgress(currentId(), fid.size());
                    return true;
                }
            }
        }
        return ChainNode::handle();
    }
};

struct MoveNode : public ChainNode {
    MoveNode(ChainNode *nextInChain) : ChainNode(nextInChain) { }
    bool handle() {
        CopyRequest &r = request();
        bool done = true;
        if (!thread()->isMoveError(currentId()))
            done = ChainNode::handle();
        if (done && error() == FileOperation::NoError && r.move) {
            bool moveError = false;
            QFileInfo fis(r.source.toLocalFile());
            QDir sourceDir = fis.dir();
            if (fis.isDir() && !fis.isSymLink()) {
                if (!sourceDir.rmdir(fis.fileName()))
                    moveError = true;
            } else if (!sourceDir.remove(fis.fileName()))
                moveError = true;

            thread()->setMoveError(currentId(), moveError);
            if (moveError) {
                setError(FileOperation::CannotRemoveSource);
                done = false;
            }
        }
        return done;
    }
};

struct CopyDirNode : public ChainNode {
    CopyDirNode(ChainNode *nextInChain)
        : ChainNode(nextInChain) {
        }
    bool handle() {
        CopyRequest &r = request();
        if (!r.dir)
            return ChainNode::handle();
        QFileInfo fis(r.source.toLocalFile());
        if (!fis.isDir()) {
            setError(FileOperation::SourceFileOmitted);
            return false;
        }
        QFileInfo fid(r.dest.toLocalFile());
        if (!fid.exists()) {
            QDir destDir = fid.dir();
            if (!destDir.exists()) {
                setError(FileOperation::PathToDestinationNotExists);
                return false;
            } else if (!destDir.mkdir(fid.fileName())) {
                setError(FileOperation::CannotCreateDestinationDirectory);
                return false;
            }
        } else if (fid.isSymLink() || fid.isFile()) {
            setError(FileOperation::CannotCreateDestinationDirectory);
            return false;
        }

        //if (fid.isDir())
        while (!r.childrenQueue.isEmpty())
            thread()->handle(r.childrenQueue.dequeue());
        if (thread()->isCanceled(currentId()))
            setError(FileOperation::Canceled); // canceled
        return true;
    }
};

struct CopyFileNode : public ChainNode {
    CopyFileNode(ChainNode *nextInChain, int currentId, const CopyRequest &request,
                OperationThread *thread)
        : ChainNode(nextInChain) {
        id = currentId;
        r = request;
        t = thread;
    }
    CopyRequest &request() {
        return r;
    }
    OperationThread *thread() const {
        return t;
    }
    int currentId() const {
        return id;
    }
    bool handle() {
        //CopyRequest &r = request();
        if (r.dir) {
            setError(FileOperation::SourceDirectoryOmitted);
            return false;
        }
        QFile sourceFile(r.source.toLocalFile());
        QFile destFile(r.dest.toLocalFile());
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            setError(FileOperation::CannotOpenSourceFile); // cannot open source file
            return false;
        }
        if (!destFile.open(QIODevice::WriteOnly)) {
            bool done = false;
            if (r.copyFlags & FileOperation::Force) {
                QFileInfo fid(r.dest.toLocalFile());
                QDir dir = fid.dir();
                if (!dir.remove(fid.fileName()))
                    setError(FileOperation::CannotRemoveDestinationFile);
                else if (!destFile.open(QIODevice::WriteOnly))
                    setError(FileOperation::CannotOpenDestinationFile);
                else
                    done = true;
            } else
                setError(FileOperation::CannotOpenDestinationFile); // cannot open dest file

            if (!done) {
                sourceFile.close();
                return false;
            }
        }
        qint64 progress = 0;
        char block[4096];
        bool done = false;
        while (1) {
            if (t->isCanceled(id)) {
                setError(FileOperation::Canceled); // canceled
                done = true;
                break;
            }
            qint64 in = sourceFile.read(block, 4096);
            if (in == 0) {
                t->emitProgress(id, progress);
                break;
            }
            if (in == -1) {
                setError(FileOperation::CannotReadSourceFile); // cannot read
                break;
            }
            if (in != destFile.write(block, in)) {
                setError(FileOperation::CannotWriteDestinationFile); // cannot write
                break;
            }
            progress += in;
            if (t->isProgressRequest())
                t->emitProgress(id, progress);
        }
        destFile.close();
        sourceFile.close();
        if (error() != FileOperation::NoError)
            destFile.remove();
        else {
            destFile.setPermissions(sourceFile.permissions());
            done = true;
        }
        return done;
    }
private:
    CopyRequest r;
    OperationThread *t;
    int id;
};

void OperationThread::renameChildren(int id)
{
    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    CopyRequest r = it.value().request;
    int oldCurrentId = currentId;
    currentId = it.key();
    mutex.unlock();
    emit started(id);

    while (!r.childrenQueue.isEmpty())
        renameChildren(r.childrenQueue.dequeue());

    if (!r.dir) {
        QFileInfo fid(r.dest.toLocalFile());
        emitProgress(id, fid.size());
    }

    emit finished(id, false);
    mutex.lock();
    currentId = oldCurrentId;
    requestQueue.remove(id);
    mutex.unlock();
}

void OperationThread::lockCancelChildren(int id)
{
    QMutexLocker l(&mutex);
    cancelChildren(id);
}

void OperationThread::cancelChildren(int id)
{
//    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    if (it == requestQueue.constEnd()) {
//        mutex.unlock();
        return;
    }
    CopyRequest r = it.value().request;
//    int oldCurrentId = currentId;
//    currentId = it.key();
//    mutex.unlock();
//    emit started(id);

    while (!r.childrenQueue.isEmpty()) {
        int childId = r.childrenQueue.dequeue();
        cancelChildren(childId);

        requestQueue.remove(childId);
    }

//    emit error(id, QtFileCopier::Canceled, false);

//    emit finished(id, true);
//    mutex.lock();
//    currentId = oldCurrentId;

//    requestQueue.remove(id);
//    mutex.unlock();
}

void OperationThread::handle(int id)
{
    if (cancelRequest)
        return;
    mutex.lock();
    QMap<int, Request>::ConstIterator it = requestQueue.find(id);
    Request r = it.value();
    int oldCurrentId = currentId;
    currentId = it.key();
    mutex.unlock();

    emit started(id);
    bool done = false;
    FileOperation::Error err = FileOperation::NoError;
    while (!done) {
        mutex.lock();
        QMap<int, Request>::ConstIterator it = requestQueue.find(id);
        r = it.value();
        bool overwriteAll = overwriteAllRequest;
        mutex.unlock();
        CopyRequest copyRequest = r.request;

        ChainNode *n = 0;
        if(r.canceled)
            n = new CanceledNode(n, r.canceled);
        else if(r.overwrite == true || overwriteAll)
            n = new OverwriteNode(n, r.overwrite || overwriteAll);
        else if(copyRequest.dir == false && copyRequest.move == false && r.overwrite == false)
            n = new CopyFileNode(n, id, copyRequest, this);
        else if(copyRequest.dir == true && copyRequest.move == false)
            n = new CopyDirNode(n);
        else if(copyRequest.dir == false && copyRequest.move == true)
            n = new MoveNode(n);
        /*n = new RenameNode(n);
        n = new FollowLinksNode(n);
        n = new MakeLinksNode(n);
        n = new SourceExistsNode(n);*/

        done = n->handle();
        err = n->error();
        delete n;

        if (done || copyRequest.copyFlags & FileOperation::NonInteractive) {
            done = true;
            if (err != FileOperation::NoError)
                emit error(id, err, false);
        } else {
            mutex.lock();
            if (stopRequest || skipAllError.contains(err)) {
                done = true;
                if (!stopRequest)
                    emit error(id, err, false);
            } else {
                emit error(id, err, true);
                waitingForInteraction = true;
                interactionCondition.wait(&mutex);
                if (skipAllRequest) {
                    skipAllRequest = false;
                    skipAllError.insert(err);
                }
                waitingForInteraction = false;
            }
            mutex.unlock();
        }
    }

    emit finished(id, err != FileOperation::NoError);
    mutex.lock();
    currentId = oldCurrentId;
    requestQueue.remove(id);
    mutex.unlock();
}

void OperationThread::run()
{
    bool stop = false;

    while (!stop) {
        mutex.lock();
        if (requestQueue.isEmpty()) {
            if (stopRequest) {
                mutex.unlock();
                stop = true;
            } else {
                progressRequest = 0;
                cancelRequest = false;
                newCopyCondition.wait(&mutex);
                if (autoReset) {
                    overwriteAllRequest = false;
                    skipAllError.clear();
                }
                mutex.unlock();
            }
        } else {
            if (cancelRequest) {
                requestQueue.clear();
                cancelRequest = false;
                emit canceled();
                mutex.unlock();
            } else {
                mutex.unlock();
                handle(requestQueue.constBegin().key());
            }
        }
    }
    deleteLater();
}

class FileOperationPrivate
{
    FileOperation *q_ptr;
    Q_DECLARE_PUBLIC(FileOperation)
public:
    FileOperationPrivate();

    void setState(FileOperation::State s);
    void copyError(int id, FileOperation::Error error, bool stopped);
    void copyStarted(int id);
    void copyFinished(int id, bool err);
    void copyCanceled();
    int copy(const QUrl &sourceFile, const QUrl &destinationPath,
            FileOperation::CopyFlags flags, bool move);
    QList<int> copyFiles(const QList<QUrl> &sourceFiles,
        const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move);
    QList<int> copyDirectory(const QUrl &sourceDir,
        const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move);
    QMap<int, CopyRequest> copyDirectoryContents(const QUrl &sourceDir,
            const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move);
    QList<int> trash(const QList<QUrl> &sourceFiles);

    void progressRequest();

    void removeChildren(int id);
    CopyRequest prepareRequest(bool checkPath, const QUrl &sourceFile,
            const QUrl &destinationPath, FileOperation::CopyFlags flags,
            bool move, bool dir, bool trash) const;
    void startThread();

    OperationThread *copyThread;
    QTimer *progressTimer;
    FileOperation::State state;
    bool error;
    int idCounter;
    QStack<int> currentStack;
    QMap<int, CopyRequest> requests;
    bool autoReset;
};

FileOperationPrivate::FileOperationPrivate()
{
    idCounter = 0;
    state = FileOperation::Idle;
    error = false;
    autoReset = true;
}

void FileOperationPrivate::setState(FileOperation::State s)
{
    if (state == s)
        return;
    Q_Q(FileOperation);
    if (s == FileOperation::Busy)
    {
        progressTimer->start();
        if (state == FileOperation::Idle)
            error = false;
    }
    else
    {
        progressTimer->stop();
    }
    emit q->stateChanged(s);
    state = s;
}

void FileOperationPrivate::copyError(int id, FileOperation::Error error, bool stopped)
{
    Q_Q(FileOperation);
    if (stopped == true)
        setState(FileOperation::WaitingForInteraction);
    emit q->error(id, error, stopped);
}

void FileOperationPrivate::copyStarted(int id)
{
    Q_Q(FileOperation);
    setState(FileOperation::Busy);
    currentStack.push(id);
    emit q->started(id);
}

void FileOperationPrivate::copyFinished(int id, bool err)
{
    Q_Q(FileOperation);
    int pop = currentStack.pop();
    Q_ASSERT(pop == id);
    Q_UNUSED(pop);
    emit q->finished(id, err);
    if (err) {
        error = err;
        removeChildren(id);
    }
    requests.remove(id);
    if (requests.isEmpty()) {
        setState(FileOperation::Idle);
        emit q->done(error);
    }
}

void FileOperationPrivate::removeChildren(int id)
{
    if (requests.contains(id)) {
        CopyRequest r = requests[id];
        QList<int> children = r.childrenQueue;
        QListIterator<int> it(children);
        while (it.hasNext()) {
            int childId = it.next();
            removeChildren(childId);
            requests.remove(childId);
        }
    }
}

void FileOperationPrivate::copyCanceled()
{
    Q_Q(FileOperation);
    requests.clear();
    emit q->canceled();
    setState(FileOperation::Idle);
    emit q->done(false);
}

CopyRequest FileOperationPrivate::prepareRequest(bool checkPath, const QUrl &sourceFile,
        const QUrl &destinationPath, FileOperation::CopyFlags flags, bool move, bool dir, bool trash = false) const
{
    QFileInfo fis(sourceFile.toLocalFile());
    QFileInfo fid(destinationPath.toLocalFile());
    fid.makeAbsolute();
    if (checkPath && fid.isDir()) {
        QDir destDir(fid.filePath());
        fid.setFile(destDir, fis.fileName());
    }
    CopyRequest r;
    r.source = QUrl::fromLocalFile(fis.filePath());
    r.dest = QUrl::fromLocalFile(fid.filePath());
    r.copyFlags = flags;
    r.move = move;
    r.dir = dir;
    r.trash = trash;
    return r;
}

void FileOperationPrivate::startThread()
{
    QTimer::singleShot(0, copyThread, SLOT(restart()));
    /*
    if (state == QtFileCopier::Idle) {
        setState(QtFileCopier::Busy);
        error = false;
    }
    */
}

int FileOperationPrivate::copy(const QUrl &sourceFile, const QUrl &destinationPath,
            FileOperation::CopyFlags flags, bool move)
{
    CopyRequest r = prepareRequest(true, sourceFile, destinationPath, flags, move, false);
    requests[idCounter] = r;
    copyThread->copy(idCounter, r);
    startThread();
    return idCounter++;
}

QList<int> FileOperationPrivate::copyFiles(const QList<QUrl> &sourceFiles,
        const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fid(destinationDir.toLocalFile());
    if (fid.isDir())
    {
        for(auto k : sourceFiles)
        {
            QFileInfo fis(k.toLocalFile());
            if (!fis.isDir()) {
                CopyRequest r = prepareRequest(true, fis.filePath(), destinationDir,
                                               flags, move, false);
                requests[idCounter] = r;
                resultList[idCounter] = r;
                idCounter++;
            }
        }
    }
    if (resultList.isEmpty())
        return QList<int>();
    copyThread->copy(resultList);
    startThread();
    return resultList.keys();
}

QList<int> FileOperationPrivate::copyDirectory(const QUrl &sourceDir,
        const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fis(sourceDir.toLocalFile());
    fis.makeAbsolute();
    QFileInfo fid(destinationDir.toLocalFile());
    fid.makeAbsolute();
    if (fis.exists() && fis.isDir()) {
        if (fid.exists() && fid.isDir()) {
            QDir sourceDir(fis.filePath());
            QDir destDir(fid.filePath());
            fid.setFile(destDir, sourceDir.dirName());
        }
        resultList = copyDirectoryContents(fis.filePath(),
                            fid.filePath(), flags, move);
        //QFile::setPermissions(fid.filePath(), fis.permissions());
    }

    if (resultList.isEmpty())
        return QList<int>();

    QMap<int, CopyRequest>::ConstIterator it = resultList.constBegin();
    while (it != resultList.constEnd()) {
        requests[it.key()] = it.value();
        it++;
    }

    copyThread->copy(resultList);
    startThread();

    return resultList.keys();
}

QMap<int, CopyRequest> FileOperationPrivate::copyDirectoryContents(const QUrl &sourceDir,
            const QUrl &destinationDir, FileOperation::CopyFlags flags, bool move)
{
    QMap<int, CopyRequest> resultList;
    QFileInfo fis(sourceDir.toLocalFile());
    fis.makeAbsolute();
    QFileInfo fid(destinationDir.toLocalFile());
    fid.makeAbsolute();

    CopyRequest r = prepareRequest(false, fis.filePath(), destinationDir, flags, move, true);
    resultList[idCounter] = r;
    int curId = idCounter;
    idCounter++;

    if (fis.isSymLink() && !(flags & FileOperation::FollowLinks))
        return resultList;

    if (flags & FileOperation::MakeLinks)
        return resultList;

    fis.setFile(r.source.toLocalFile());
    fid.setFile(r.dest.toLocalFile());
    QDir sDir(fis.filePath());
    QDir destDir(fid.filePath());
    QFileInfoList dirList = sDir.entryInfoList(QDir::Dirs);
    QListIterator<QFileInfo> itDir(dirList);
#if QT_VERSION > 0x060000
    while (itDir.hasNext()) {
        QFileInfo newfis = itDir.next();
        newfis.makeAbsolute();
        QString dirName = newfis.fileName();
        if (newfis.isDir() && dirName != QString(".") && dirName != QString("..")) {
            QFileInfo newfid(destDir.filePath(dirName));
            QMap<int, CopyRequest> childDir = copyDirectoryContents(newfis.filePath(),
                            newfid.filePath(), flags, move);
            for(auto [k,v] : childDir.asKeyValueRange())
                resultList.insert(k,v);
            resultList[curId].childrenQueue.enqueue(childDir.constBegin().key());
        }
    }
#endif
    QFileInfoList fileList = sDir.entryInfoList(QDir::Files |
                    QDir::Hidden | QDir::System);
    QListIterator<QFileInfo> itLink(fileList);
    while (itLink.hasNext()) {
        QFileInfo newfis = itLink.next();
        if (!newfis.isDir() && newfis.isSymLink()) {
            newfis.makeAbsolute();
            CopyRequest r = prepareRequest(false, newfis.filePath(),
                        destDir.filePath(newfis.fileName()), flags, move, false);
            resultList[curId].childrenQueue.enqueue(idCounter);
            resultList[idCounter] = r;
            idCounter++;
        }
    }
    QListIterator<QFileInfo> itFile(fileList);
    while (itFile.hasNext()) {
        QFileInfo newfis = itFile.next();
        if (!newfis.isDir() && !newfis.isSymLink()) {
            newfis.makeAbsolute();
            CopyRequest r = prepareRequest(false, newfis.filePath(),
                        destDir.filePath(newfis.fileName()), flags, move, false);
            resultList[curId].childrenQueue.enqueue(idCounter);
            resultList[idCounter] = r;
            idCounter++;
        }
    }

    return resultList;
}

QList<int> FileOperationPrivate::trash(const QList<QUrl> &sourceFiles)
{
    auto trashDir = QUrl::fromLocalFile(LSTrashModel::xdgTrashDir());
    QMap<int, CopyRequest> resultList;
    for(auto k : sourceFiles)
    {
        QFileInfo fis(k.toLocalFile());
        CopyRequest r = prepareRequest(true, fis.filePath(), trashDir,
                                       FileOperation::CopyFlags(0), true, fis.isDir(), true);
        requests[idCounter] = r;
        resultList[idCounter] = r;
        idCounter++;
    }
    if (resultList.isEmpty())
        return QList<int>();
    copyThread->copy(resultList);
    startThread();
    return resultList.keys();
}

void FileOperationPrivate::progressRequest()
{
    if (state == FileOperation::Busy)
        copyThread->progress();
}

FileOperation::FileOperation(QObject *parent) : QObject(parent)
{
    d_ptr = new FileOperationPrivate;
    d_ptr->q_ptr = this;
    d_ptr->copyThread = new OperationThread(this);
    d_ptr->progressTimer = new QTimer(this);
    d_ptr->progressTimer->setInterval(200);
    connect(d_ptr->progressTimer, SIGNAL(timeout()),
                this, SLOT(progressRequest()));
}

FileOperation::~FileOperation()
{
    Q_D(FileOperation);
    delete d;
}

int FileOperation::copy(const QUrl &sourceFile, const QUrl &destinationPath,
                CopyFlags flags)
{
    if (state() != FileOperation::Idle)
        return -1;
    QFileInfo fis(sourceFile.toLocalFile());
    if (fis.isDir())
        return -1; // Omitting Dir

  return d_ptr->copy(sourceFile, destinationPath, flags, false);
}

QList<int> FileOperation::trash(const QList<QUrl> &sourceFiles)
{
    if (state() != FileOperation::Idle)
        return QList<int>();
    return d_ptr->trash(sourceFiles);
}

QList<int> FileOperation::copyFiles(const QList<QUrl> &sourceFiles,
                const QUrl &destinationDir, CopyFlags flags)
{
    if (state() != FileOperation::Idle)
        return QList<int>();
    return d_ptr->copyFiles(sourceFiles, destinationDir, flags, false);
}

QList<int> FileOperation::copyDirectory(const QUrl &sourceDir,
                const QUrl &destinationDir, CopyFlags flags)
{
    if (state() != FileOperation::Idle)
        return QList<int>();
    return d_ptr->copyDirectory(sourceDir, destinationDir, flags, false);
}

int FileOperation::move(const QUrl &sourceFile, const QUrl &destinationPath,
                CopyFlags flags)
{
    if (flags & FileOperation::MakeLinks) {
        qWarning("FileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~FileOperation::MakeLinks;
    }
    if (flags & FileOperation::FollowLinks) {
        qWarning("FileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~FileOperation::FollowLinks;
    }
    QFileInfo fis(sourceFile.toLocalFile());
    if (fis.isDir())
        return -1; // Omitting Dir
    return d_ptr->copy(sourceFile, destinationPath, flags, true);
}

QList<int> FileOperation::moveFiles(const QList<QUrl> &sourceFiles,
                const QUrl &destinationDir, CopyFlags flags)
{
    if (flags & FileOperation::MakeLinks) {
        qWarning("FileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~FileOperation::MakeLinks;
    }
    if (flags & FileOperation::FollowLinks) {
        qWarning("FileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~FileOperation::FollowLinks;
    }
    return d_ptr->copyFiles(sourceFiles, destinationDir, flags, true);
}

QList<int> FileOperation::moveDirectory(const QUrl &sourceDir,
                const QUrl &destinationDir, CopyFlags flags)
{
    if (flags & FileOperation::MakeLinks) {
        qWarning("FileCopier: cannot move with MakeLinks option specified, option cleared.");
        flags &= ~FileOperation::MakeLinks;
    }
    if (flags & FileOperation::FollowLinks) {
        qWarning("FileCopier: cannot move with FollowLinks option specified, option cleared.");
        flags &= ~FileOperation::FollowLinks;
    }
    return d_ptr->copyDirectory(sourceDir, destinationDir, flags, true);
}

QList<int> FileOperation::pendingRequests() const
{
    Q_D(const FileOperation);
    return d->requests.keys();
}

QUrl FileOperation::sourceFilePath(int id) const
{
    Q_D(const FileOperation);
    if (d->requests.contains(id))
        return d->requests[id].source;
    return QUrl();
}

QUrl FileOperation::destinationFilePath(int id) const
{
    Q_D(const FileOperation);
    if (d->requests.contains(id))
        return d->requests[id].dest;
    return QUrl();
}

bool FileOperation::isDir(int id) const
{
    Q_D(const FileOperation);
    if (d->requests.contains(id))
        return d->requests[id].dir;
    return false;
}

QList<int> FileOperation::entryList(int id) const
{
    Q_D(const FileOperation);
    if (d->requests.contains(id))
        return d->requests[id].childrenQueue;
    return QList<int>();
}

int FileOperation::currentId() const
{
    Q_D(const FileOperation);
    if (d->currentStack.isEmpty())
        return -1;
    return d->currentStack.top();

}

FileOperation::State FileOperation::state() const
{
    Q_D(const FileOperation);
    return d->state;
}

void FileOperation::cancelAll()
{
    Q_D(FileOperation);
    d->copyThread->cancel();
}


void FileOperation::cancel(int id)
{
    Q_D(FileOperation);
    d->copyThread->cancel(id);
}

void FileOperation::skip()
{
    if (state() != FileOperation::WaitingForInteraction)
        return;
    Q_D(FileOperation);
    d->copyThread->skip();
    d->setState(FileOperation::Busy);
}

void FileOperation::skipAll()
{
    if (state() != FileOperation::WaitingForInteraction)
        return;
    Q_D(FileOperation);
    d->copyThread->skipAll();
    d->setState(FileOperation::Busy);
}

void FileOperation::retry()
{
    if (state() != FileOperation::WaitingForInteraction)
        return;
    Q_D(FileOperation);
    d->copyThread->retry();
    d->setState(FileOperation::Busy);
}

void FileOperation::overwrite()
{
    if (state() != FileOperation::WaitingForInteraction)
        return;
    Q_D(FileOperation);
    d->copyThread->overwrite();
    d->setState(FileOperation::Busy);
}

void FileOperation::overwriteAll()
{
    if (state() != FileOperation::WaitingForInteraction)
        return;
    Q_D(FileOperation);
    d->copyThread->overwriteAll();
    d->setState(FileOperation::Busy);
}

void FileOperation::reset()
{
    resetSkip();
    resetOverwrite();
}

void FileOperation::resetSkip()
{
    Q_D(FileOperation);
    d->copyThread->resetSkip();
}

void FileOperation::resetOverwrite()
{
    Q_D(FileOperation);
    d->copyThread->resetOverwrite();
}

void FileOperation::setAutoReset(bool on)
{
    Q_D(FileOperation);
    d->autoReset = on;
    d->copyThread->setAutoReset(on);
}

bool FileOperation::autoReset() const
{
    Q_D(const FileOperation);
    return d->autoReset;
}

int FileOperation::progressInterval() const
{
    Q_D(const FileOperation);
    return d->progressTimer->interval();
}

void FileOperation::setProgressInterval(int ms)
{
    Q_D(FileOperation);
    d->progressTimer->setInterval(ms);
}

//#include "fileoperation.moc"
#include "moc_fileoperation.cpp"
