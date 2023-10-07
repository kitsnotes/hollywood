#ifndef OPERATIONMANAGER_H
#define OPERATIONMANAGER_H

#include <QObject>
#include "operationthread.h"
#include "progresswidget.h"

class OperationManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OperationManagerDialog();
};

class OperationManager;
class OperationManagerPrivate
{
private:
    friend class OperationManager;
    OperationManagerPrivate(OperationManager *parent);
    ~OperationManagerPrivate();
    struct operation_t
    {
        OperationWorker *worker = nullptr;
        LSOpProgressWidget *widget = nullptr;
        QThread *thread = nullptr;
        QUuid uuid;
    };

    QUuid operation(const QList<QUrl> &sources, const OperationManager::OperationType type, const QUrl &destinationPath);
private slots:

private:
    OperationManager *d;
    QMap<QUuid, operation_t> m_threads;
    OperationManagerDialog *m_dialog;
};

#endif // OPERATIONMANAGER_H
