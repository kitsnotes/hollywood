// Hollywood Shell Library
// (C) 2023 Originull Software
// This class derived from Qt Solutions
// (C) 2009 Nokia Corporation
// SPDX-License-Identifier: LGPL-2.1

#ifndef FILEOPERATION_H
#define FILEOPERATION_H

#include <QtCore/QObject>
#include <QUrl>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QAtomicInt>
#include <QtCore/QThread>

class FileOperationPrivate;

class FileOperation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int progressInterval READ progressInterval WRITE setProgressInterval)
    Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
public:

    FileOperation(QObject *parent = 0);
    ~FileOperation();

    enum State {
        Idle,
        Busy,
        WaitingForInteraction
    };

    enum CopyFlag {
        NonInteractive = 0x01,
        Force = 0x02,
        MakeLinks = 0x04,
        FollowLinks = 0x08 // if not set links are copied
    };

    enum Error {
        NoError,
        SourceNotExists,
        DestinationExists,
        SourceDirectoryOmitted,
        SourceFileOmitted,
        PathToDestinationNotExists,
        CannotCreateDestinationDirectory,
        CannotOpenSourceFile,
        CannotOpenDestinationFile,
        CannotRemoveDestinationFile,
        CannotCreateSymLink,
        CannotReadSourceFile,
        CannotWriteDestinationFile,
        CannotRemoveSource,
        Canceled
    };

    Q_DECLARE_FLAGS(CopyFlags, CopyFlag)
    Q_FLAG(CopyFlag)

    int copy(const QUrl &sourceFile, const QUrl &destinationPath,
             CopyFlags flags = (CopyFlags)0);
    QList<int> copyFiles(const QList<QUrl> &sourceFiles, const QUrl &destinationDir,
                CopyFlags flags = (CopyFlags)0);
    QList<int> copyDirectory(const QUrl &sourceDir, const QUrl &destinationDir,
                CopyFlags flags = (CopyFlags)0);

    int move(const QUrl &sourceFile, const QUrl &destinationPath,
                CopyFlags flags = (CopyFlags)0);
    QList<int> moveFiles(const QList<QUrl> &sourceFiles, const QUrl &destinationDir,
                CopyFlags flags = (CopyFlags)0);
    QList<int> moveDirectory(const QUrl &sourceDir, const QUrl &destinationDir,
                CopyFlags flags = (CopyFlags)0);

    QList<int> pendingRequests() const;
    QUrl sourceFilePath(int id) const;
    QUrl destinationFilePath(int id) const;
    bool isDir(int id) const;
    QList<int> entryList(int id) const;
    int currentId() const;

    State state() const;

    void setAutoReset(bool on);
    bool autoReset() const;
    int progressInterval() const;
    void setProgressInterval(int ms);

public slots:

    void cancelAll();
    void cancel(int id);

    void skip();
    void skipAll();
    void retry();

    void overwrite();
    void overwriteAll();

    void reset();
    void resetSkip();
    void resetOverwrite();

signals:
    void error(int id, FileOperation::Error error, bool stopped);

    void stateChanged(FileOperation::State state);

    void done(bool error);
    void started(int id);
    void dataTransferProgress(int id, qint64 progress);
    void finished(int id, bool error);
    void canceled();

private:

    FileOperationPrivate *d_ptr;
    Q_DECLARE_PRIVATE(FileOperation)
    Q_DISABLE_COPY(FileOperation)

    Q_PRIVATE_SLOT(d_func(), void copyStarted(int))
    Q_PRIVATE_SLOT(d_func(), void copyFinished(int, bool))
    Q_PRIVATE_SLOT(d_func(), void copyCanceled())
    Q_PRIVATE_SLOT(d_func(), void copyError(int, FileOperation::Error, bool))
    Q_PRIVATE_SLOT(d_func(), void progressRequest())
};


struct CopyRequest {
    CopyRequest() {
        move = false;
        dir = false;
    }
    QQueue<int> childrenQueue;
    QUrl source;
    QUrl dest;
    bool move;
    bool dir;
    FileOperation::CopyFlags copyFlags;
};

class OperationThread : public QThread
{
    Q_OBJECT
public:
    OperationThread(FileOperation *fileCopier);
    ~OperationThread();

    struct Request {
        Request() {
            canceled = false;
            overwrite = false;
            moveError = false;
        }
        Request(const CopyRequest &r) {
            request = r;
            canceled = false;
            overwrite = false;
            moveError = false;
        }
        CopyRequest request;
        bool canceled;
        bool overwrite;
        bool moveError;
    };

    void emitProgress(int id, qint64 progress) {
        QMutexLocker l(&mutex);
        emit dataTransferProgress(id, progress);
        progressRequest = 0;
    }
    bool isCanceled(int id) const {
        QMutexLocker l(&mutex);
        if (cancelRequest)
            return true;
        if (requestQueue.empty())
            return false;
        return requestQueue[id].canceled;
    }
    bool isMoveError(int id) const {
        QMutexLocker l(&mutex);
        if (requestQueue.empty())
            return false;
        return requestQueue[id].moveError;
    }
    bool isProgressRequest() const {
        return (progressRequest != 0);
    }
    void setMoveError(int id, bool error) {
        QMutexLocker l(&mutex);
        if (requestQueue.empty())
            return;
        requestQueue[id].moveError = error;
    }
    void handle(int id);
    void lockCancelChildren(int id);
    void renameChildren(int id);
    void cancelChildRequests(int id);
    void overwriteChildRequests(int id);

    void setAutoReset(bool on);
public slots:
    void restart();

    void copy(int id, const CopyRequest &request);
    void copy(const QMap<int, CopyRequest> &requests);

    void cancel();
    void cancel(int id);

    void skip();
    void skipAll();
    void retry();

    void overwrite();
    void overwriteAll();

    void resetOverwrite();
    void resetSkip();

    void progress();
signals:
    void error(int id, FileOperation::Error error, bool stopped);
    void started(int id);
    void dataTransferProgress(int id, qint64 progress);
    void finished(int id, bool error);
    void canceled();
protected:
    void run();
protected slots:
    void copierDestroyed();
private:

    void cancelChildren(int id);

    FileOperation *copier;
    QMap<int, Request> requestQueue;
    mutable QMutex mutex;
    QWaitCondition newCopyCondition;
    QWaitCondition interactionCondition;
    bool waitingForInteraction;
    bool stopRequest;
    bool skipAllRequest;
    QSet<FileOperation::Error> skipAllError;
    bool overwriteAllRequest;
    bool cancelRequest;
    int currentId;
    QAtomicInt progressRequest;
    bool autoReset;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FileOperation::CopyFlags)

#endif
