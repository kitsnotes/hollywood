// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#ifndef COMMONFUNCTIONS_P_H
#define COMMONFUNCTIONS_P_H

#include <QObject>

class QUndoStack;
class OperationManager;
class LSCommonFunctions;
class LSCommonFunctionsPrivate : public QObject
{
    Q_OBJECT
private:
    friend class LSCommonFunctions;
    LSCommonFunctions *d;
    LSCommonFunctionsPrivate(LSCommonFunctions *parent);
    ~LSCommonFunctionsPrivate() = default;
    QString nextNewFolderName(const QUrl &path) const;
public slots:
    void clipboardDataChanged();
private:
    QUndoStack *m_undo = nullptr;
    OperationManager *m_opmgr = nullptr;
};

#endif // COMMONFUNCTIONS_P_H