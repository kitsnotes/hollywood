// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "opmanager_p.h"
#include "opmanager.h"
#include "fileoperation.h"

#include "progresswidget.h"

#include <QListView>
#include <QListWidget>

OperationManagerDialog::OperationManagerDialog()
    : QDialog(nullptr)
    , m_layout(new QVBoxLayout(this))
    , m_status(new QStatusBar(this))
{
    setWindowTitle(tr("Operation Progress"));
    setMinimumSize(QSize(450,350));
    m_layout->addWidget(m_status);
    m_layout->setContentsMargins(0,0,0,0);
    setLayout(m_layout);
}

void OperationManagerDialog::addWidget(LSOpProgressWidget *widget)
{
    m_layout->insertWidget(m_layout->count()-1, widget);
    m_widgets.append(widget);
}

void OperationManagerDialog::removeWidget(LSOpProgressWidget *widget)
{
    m_layout->removeWidget(widget);
    m_widgets.removeOne(widget);
    if(m_widgets.count() == 0)
        hide();
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

OperationManager::OperationManager()
    : p(new OperationManagerPrivate(this)) {}

QUuid OperationManager::moveFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    auto op = new FileOperation(this);
    auto widget = new LSOpProgressWidget(op, p->m_dialog);

    connect(op, &FileOperation::done, [this,op,widget](bool error){
        if(error)
        {
            //widget->setError(op->error(op->););
            op->deleteLater();
        }
        else
        {
            // operation success - lets clean up
            p->m_dialog->removeWidget(widget);
            widget->deleteLater();
            op->deleteLater();
        }
    });
    p->m_dialog->addWidget(widget);
    p->m_dialog->show();
    if(sources.count() == 1)
    {
        op->move(sources.first(), destinationPath);
    }
    else
    {
        op->moveFiles(sources, destinationPath);
    }
    return QUuid(); //p->operation(sources, OperationType::Move, destinationPath);
}

QUuid OperationManager::copyFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    auto op = new FileOperation(this);
    auto widget = new LSOpProgressWidget(op, p->m_dialog);

    connect(op, &FileOperation::done, [this,op,widget](bool error){
        if(error)
        {
            //widget->setError(op->error(op->););
            op->deleteLater();
        }
        else
        {
            // operation success - lets clean up
            p->m_dialog->removeWidget(widget);
            widget->deleteLater();
            op->deleteLater();
            // make an undo stack entry
        }
    });
    p->m_dialog->addWidget(widget);
    p->m_dialog->show();
    if(sources.count() == 1)
    {
        op->copy(sources.first(), destinationPath);
    }
    else
    {
        op->copyFiles(sources, destinationPath);
    }
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
    return 0;
}
