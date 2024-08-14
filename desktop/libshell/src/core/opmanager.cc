#include "opmanager_p.h"
#include "opmanager.h"

#include "progresswidget.h"

#include <QListView>
#include <QListWidget>

OperationManager *g_global_op = nullptr;

OperationManagerDialog::OperationManagerDialog()
    : QDialog(nullptr)
    , m_list(new QListView(this))
    , m_status(new QStatusBar(this))
{
    setWindowTitle(tr("Operation Progress"));
    setMinimumSize(QSize(450,350));
    auto layout = new QVBoxLayout(this);
    layout->addWidget(m_list);
    layout->addWidget(m_status);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);
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

/*QUuid OperationManagerPrivate::operation(const QList<QUrl> &sources, const OperationManager::OperationType type, const QUrl &destinationPath)
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

    QObject::connect(newop.thread, &QThread::started,
                     newop.worker, &OperationWorker::startWorker);

    QObject::connect(newop.worker, &OperationWorker::progressChanged,
            newop.widget, &LSOpProgressWidget::progressChanged);
    QObject::connect(newop.worker, &OperationWorker::progressChanged,
            newop.widget, &LSOpProgressWidget::progressChanged);

    m_threads.insert(newop.uuid, newop);

    m_dialog->addWidget(newop.widget);
    newop.worker->startWorker();
    return newop.uuid;
}*/

OperationManager::OperationManager()
    : p(new OperationManagerPrivate(this)) {}

QUuid OperationManager::moveFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return QUuid(); //p->operation(sources, OperationType::Move, destinationPath);
}

QUuid OperationManager::copyFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return QUuid(); //p->operation(sources, OperationType::Copy, destinationPath);
}

QUuid OperationManager::symlinkFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    return QUuid(); //p->operation(sources, OperationType::Symlink, destinationPath);
}

QUuid OperationManager::trashFiles(const QList<QUrl> &sources)
{
    return QUuid(); //p->operation(sources, OperationType::Trash, QString());
}

void OperationManager::showDialog()
{
    p->m_dialog->show();
}

bool OperationManager::dialogIsVisible() const
{
    return p->m_dialog->isVisible();
}

uint OperationManager::activeOperations() const
{
    return p->m_threads.count();
}

OperationManager *__hwshell_operationManager()
{
    if(g_global_op == nullptr)
        g_global_op = new OperationManager();

    return g_global_op;
}
