#ifndef OPMANAGER_H
#define OPMANAGER_H

#include <QObject>
#include "libshell_int.h"

// ****** WARNING ******
// THIS CLASS IS NOT FOR PUBLIC CONSUMPTION
// THERE IS ONLY ONE SINGLE INSTANCE OF THIS IN THE MAIN SHELLFM INSTANCE
// all other consumers outside this process need to use the d-bus API

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
    QUuid trashFiles(const QList<QUrl> &sources);

    void showDialog();
    bool dialogIsVisible() const;
    uint activeOperations() const;
signals:
    void operationFinished(const QUuid &token);
    void operationHasError(const Error error);
private:
    OperationManagerPrivate *p;
};

extern "C" {
    OperationManager *__hwshell_operationManager();
}
#endif // OPMANAGER_H
