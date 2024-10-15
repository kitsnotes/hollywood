// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QObject>
#include <QUuid>
#include <QDialog>

#include "fileoperation.h"
#include "progresswidget.h"

class QListView;
class QStatusBar;
class OperationManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OperationManagerDialog();
    void addWidget(LSOpProgressWidget *widget);
    void removeWidget(LSOpProgressWidget *widget);
private:
    QList<LSOpProgressWidget*> m_widgets;
    QVBoxLayout *m_layout = nullptr;
    QStatusBar *m_status = nullptr;
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
        FileOperation *worker = nullptr;
        LSOpProgressWidget *widget = nullptr;
        QUuid uuid;
    };

    //QUuid operation(const QList<QUrl> &sources, const OperationManager::OperationType type, const QUrl &destinationPath);
private slots:

private:
    OperationManager *d;
    QMap<QUuid, operation_t> m_threads;
    OperationManagerDialog *m_dialog;
};
