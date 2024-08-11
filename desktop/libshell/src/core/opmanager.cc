#include "opmanager_p.h"
#include "opmanager.h"

#include "progresswidget.h"

static OperationManager *g_ops = nullptr;

OperationManagerDialog::OperationManagerDialog()
    : QDialog(nullptr)
{
    setWindowTitle(tr("Operations"));
    setMinimumSize(QSize(450,350));
}

OperationManagerPrivate::OperationManagerPrivate(OperationManager *parent)
    : d(parent)
    , m_dialog(new OperationManagerDialog)
{
    //m_dialog->show();
}

OperationManagerPrivate::~OperationManagerPrivate()
{
    m_dialog->hide();
    m_dialog->deleteLater();
}

QUuid OperationManagerPrivate::operation(const QList<QUrl> &sources, const OperationManager::OperationType type, const QUrl &destinationPath)
{
    operation_t newop;
    QList<OperationWorker::OpItem> items;
    for(auto u : sources)
    {
        OperationWorker::OpItem i;
        i.u = u;
        items.append(i);
    }
    newop.worker = new OperationWorker(items, type, destinationPath);
    newop.widget = new LSOpProgressWidget(m_dialog);
    newop.uuid = QUuid::createUuid();
    newop.thread = new QThread();
    newop.worker->moveToThread(newop.thread);

    QObject::connect(newop.thread, &QThread::started, newop.worker, &OperationWorker::startWorker);

    QObject::connect(newop.worker, &OperationWorker::progressChanged,
            newop.widget, &LSOpProgressWidget::progressChanged);
    QObject::connect(newop.worker, &OperationWorker::progressChanged,
            newop.widget, &LSOpProgressWidget::progressChanged);

    m_threads.insert(newop.uuid, newop);

    //newop.worker->start();
    return newop.uuid;
}

OperationManager::OperationManager()
    : p(new OperationManagerPrivate(this)) {}

QUuid OperationManager::moveFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return p->operation(sources, OperationType::Move, destinationPath);
}

QUuid OperationManager::copyFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return p->operation(sources, OperationType::Copy, destinationPath);
}

QUuid OperationManager::symlinkFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return p->operation(sources, OperationType::Symlink, destinationPath);
}

QUuid OperationManager::trashFiles(const QList<QUrl> &sources)
{
    return p->operation(sources, OperationType::Trash, QString());
}

OperationManager *operationManager()
{
    if(g_ops == nullptr)
        g_ops = new OperationManager;

    return g_ops;
}
