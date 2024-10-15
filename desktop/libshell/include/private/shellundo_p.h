// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QUndoCommand>
#include <QUrl>

class QFileSystemWatcher;
class UndoNewFolder : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    UndoNewFolder(const QUrl &target, const QString &newFolderName);
    ~UndoNewFolder();
    void undo() override;
    void redo() override;
private slots:
    void nowObsolete();
private:
    QUrl m_target;
    QString m_newName;
    QFileSystemWatcher *m_watch;
};
