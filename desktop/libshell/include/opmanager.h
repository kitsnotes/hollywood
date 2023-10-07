#ifndef OPMANAGER_H
#define OPMANAGER_H

#include <QObject>
#include "libshell_int.h"

class OperationManagerPrivate;
class LIBSHELL_EXPORT OperationManager : public QObject
{
    Q_OBJECT
public:
    explicit OperationManager();
    enum OperationType {
        Move,
        Copy,
        Symlink,
        Delete,
        Trash
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

    QUuid moveFiles(const QList<QUrl> &sources, const QUrl &destinationPath);
    QUuid copyFiles(const QList<QUrl> &sources, const QUrl &destinationPath);
    QUuid symlinkFiles(const QList<QUrl> &sources, const QUrl &destinationPath);
    QUuid trashFiles(const QList<QUrl> &sources, const QUrl &destinationPath);

signals:
    void operationFinished(const QUuid &token);
    void operationHasError(const Error error);
private:
    OperationManagerPrivate *p;
};

extern "C" {
    OperationManager *operationManager();
}

#endif // OPMANAGER_H
