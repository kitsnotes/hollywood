// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "shellundo_p.h"

#include <QDir>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QApplication>
#include <QDebug>

UndoNewFolder::UndoNewFolder(const QUrl &target, const QString &newFolderName)
    : QObject()
    , QUndoCommand(QApplication::tr("New Folder"))
    , m_target(target)
    , m_newName(newFolderName)
    , m_watch(new QFileSystemWatcher())
{
    connect(m_watch, &QFileSystemWatcher::directoryChanged,
            this, &UndoNewFolder::nowObsolete);
    m_watch->addPath(m_target.toLocalFile());
}

UndoNewFolder::~UndoNewFolder()
{
    m_watch->removePath(m_target.toLocalFile());
    m_watch->deleteLater();
}

void UndoNewFolder::undo()
{
    qDebug() << "removing dir" << m_target.toLocalFile() << m_newName;
    QDir dir(m_target.toLocalFile());
    dir.rmdir(m_newName);
}

void UndoNewFolder::redo()
{
    QDir dir(m_target.toLocalFile());
    dir.mkdir(m_newName);
}

void UndoNewFolder::nowObsolete()
{
    // something happened in this folder, we can't undo anymore
    setObsolete(true);
}
