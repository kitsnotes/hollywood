#ifndef OPERATIONTHREAD_H
#define OPERATIONTHREAD_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QUrl>

#include <sys/file.h>
#include <sys/stat.h>
class OperationWorker : public QThread
{
    Q_OBJECT
public:
    enum OperationType {
        Move,
        Copy,
        Symlink,
        Delete,
        Trash
    };
    struct OpItem {
        ino_t inode;
        mode_t mode;
        uid_t uid;
        gid_t gid;
        off_t size;
        QUrl u;
        int fd = -1;
    };

    explicit OperationWorker(const QList<OpItem> &sources, const OperationType type, const QUrl target, QObject *parent = nullptr);
    void run();
signals:
    void progressChanged(const uint progress);
    void statusChanged(const QString &status);
private:
    void prepareFiles();
    void fsMoveOperation();
    void fsCopyOperation();
    void fsSymlinkOperation();
    void fsDeleteOperation();
    void fsTrashOperation();

    void archiveCopyOperation();
    void archiveDeleteOperation();

    bool moveCrossingFilesystem();
    quint64 copyDataCost();
private:
    QMutex m_mutex;

    QList<OpItem> m_sources;
    OperationType m_optype;
    QUrl m_targetPath;

    bool m_source_archive = false;
    bool m_dest_archive = false;

    uint m_progress = 0;
    QString m_status;

    QVector<QString> m_readarchive;
    QVector<QString> m_writearchive;

};

#endif // OPERATIONTHREAD_H
