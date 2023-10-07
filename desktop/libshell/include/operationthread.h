#ifndef OPERATIONTHREAD_H
#define OPERATIONTHREAD_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QUrl>

#include <sys/file.h>
#include <sys/stat.h>

#include "opmanager.h"

class OperationWorker : public QObject
{
    Q_OBJECT
public:

    struct OpItem {
        ino_t inode;
        mode_t mode;
        uid_t uid;
        gid_t gid;
        off_t size;
        QUrl u;
        int fd = -1;
    };

    explicit OperationWorker(const QList<OpItem> &sources, const OperationManager::OperationType type, const QUrl target, QObject *parent = nullptr);
public slots:
    void startWorker();
signals:
    void progressChanged(const uint progress);
    void statusChanged(const QString &status);
    void errorOccured(const QUrl &file, const OperationManager::Error error);
    void finished();
private:
    void prepareFiles();
    void doWork();
    void fsMoveOperation();
    void fsCopyOperation();
    void fsSymlinkOperation();
    void fsDeleteOperation();
    void fsTrashOperation();

    void archiveCopyOperation();
    void archiveDeleteOperation();

    bool moveCrossingFilesystem();
    bool fileCrossFilesystem(const QUrl &url);
    quint64 copyDataCost();
private:
    QList<OpItem> m_sources;
    QList<OpItem> m_processed;
    OperationManager::OperationType m_optype;
    QUrl m_targetPath;

    bool m_source_archive = false;
    bool m_dest_archive = false;

    uint m_progress = 0;
    QString m_status;

    QVector<QString> m_readarchive;
    QVector<QString> m_writearchive;
};

#endif // OPERATIONTHREAD_H
