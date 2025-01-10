// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "opmanager_p.h"
#include "opmanager.h"
#include "fileoperation.h"
#include "hwfileiconprovider.h"
#include "progresswidget.h"
#include "commonfunctions.h"
#include "mimeapps.h"
#include "desktopentry.h"

#include <QListView>
#include <QListWidget>

OperationManagerDialog::OperationManagerDialog()
    : QDialog(nullptr)
    , m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("Operation Progress"));
    setMinimumSize(QSize(450,90));
    setMaximumWidth(950);
    m_layout->addSpacerItem(new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::MinimumExpanding));
    m_layout->setContentsMargins(0,0,0,0);
    setLayout(m_layout);
}

void OperationManagerDialog::addWidget(LSOpProgressWidget *widget)
{
    m_layout->insertWidget(m_layout->count()-1, widget);
    m_widgets.append(widget);
    uint myheight = 0;
    for(auto w : m_widgets)
        myheight += w->size().height()+10;
    resize(size().width(), myheight);
    setMinimumHeight(myheight);
    setMaximumHeight(myheight+1);
    if(isVisible())
        activateWindow();
}

void OperationManagerDialog::removeWidget(LSOpProgressWidget *widget)
{
    m_layout->removeWidget(widget);
    m_widgets.removeOne(widget);
    if(m_widgets.count() == 0)
        hide();
    uint myheight = 0;
    for(auto w : m_widgets)
        myheight += w->size().height()+10;
    resize(size().width(), myheight);
    setMinimumHeight(myheight);
    setMaximumHeight(myheight+1);
}

OperationManagerPrivate::OperationManagerPrivate(OperationManager *parent)
    : d(parent)
    , m_dialog(new OperationManagerDialog) {}

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
    auto path = destinationPath.toLocalFile().split('/').last();
    auto fi = QFileInfo(sources.first().toLocalFile());
    HWFileIconProvider pr;
    widget->setIcon(pr.icon(fi));
    QString target;
    if(sources.count() > 1)
        target = tr("Moving %1 files to \"%2\"").arg(QString::number(sources.count()).arg(path));
    else
    {
        auto srcpath = sources.first().toLocalFile().split('/').last();
        target = tr("Moving \"%1\" to \"%2\"").arg(srcpath).arg(path);
    }
    widget->setOperationTitle(target);
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
    p->m_dialog->activateWindow();
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
    auto path = destinationPath.toLocalFile().split('/').last();
    auto fi = QFileInfo(sources.first().toLocalFile());
    HWFileIconProvider pr;
    widget->setIcon(pr.icon(fi));
    QString target;
    if(sources.count() > 1)
        target = tr("Copying %1 files to \"%2\"").arg(QString::number(sources.count()).arg(path));
    else
    {
        auto srcpath = sources.first().toLocalFile().split('/').last();
        target = tr("Copying \"%1\" to \"%2\"").arg(srcpath).arg(path);
    }
    widget->setOperationTitle(target);
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
    p->m_dialog->activateWindow();
    if(sources.count() == 1)
    {
        op->copy(sources.first(), destinationPath);
    }
    else
    {
        op->copyFiles(sources, destinationPath);
    }
    return QUuid();
}

QUuid OperationManager::symlinkFiles(const QList<QUrl> &sources, const QUrl &destinationPath)
{
    Q_UNUSED(sources)
    Q_UNUSED(destinationPath)
    return QUuid();
}

QUuid OperationManager::trashFiles(const QList<QUrl> &sources)
{
    // this function handles more than 'trash'
    // if an application is sent over, it should be prompted and uninstalled
    // if a device is sent over, it should be unmounted/ejected

    QList<QUrl> filteredSources;
    QList<QUrl> applications;
    //QList<QUrl> mounts;
    for(auto u : sources)
    {
        bool removed = false;
        if(u.fileName().endsWith("desktop"))
        {
            // check to see if this is a managed application
            auto fileName = u.toLocalFile();
            QFileInfo fi(u.toLocalFile());
            if(fi.isSymbolicLink())
                fileName = fi.readSymLink();

            auto desktop = LSCommonFunctions::instance()->mimeApps()->findDesktopForFile(fileName);
            if(desktop && desktop->isValid())
            {
                if(desktop->checkForApk())
                {
                    applications.append(u);
                    removed = true;
                }
            }
        }

        if(!removed)
            filteredSources.append(u);
    }

    if(filteredSources.count() > 0)
    {
        auto op = new FileOperation(this);
        auto widget = new LSOpProgressWidget(op, p->m_dialog);
        auto fi = QFileInfo(sources.first().toLocalFile());
        HWFileIconProvider pr;
        widget->setIcon(pr.icon(fi));
        QString target;
        if(sources.count() > 1)
            target = tr("Trashing %1 files").arg(QString::number(filteredSources.count()));
        else
        {
            auto srcpath = sources.first().toLocalFile().split('/').last();
            target = tr("Trashing \"%1\"").arg(srcpath);
        }
        widget->setOperationTitle(target);
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
        p->m_dialog->activateWindow();
        op->trash(filteredSources);
    }

    // process any unmount requests

    // Process our uninstallations
    LSCommonFunctions::instance()->questionAndStartUninstall(applications);
    return QUuid();
}

void OperationManager::showDialog()
{
    p->m_dialog->show();
    p->m_dialog->activateWindow();
}

bool OperationManager::dialogIsVisible() const
{
    return p->m_dialog->isVisible();
}

uint OperationManager::activeOperations() const
{
    return 0;
}
