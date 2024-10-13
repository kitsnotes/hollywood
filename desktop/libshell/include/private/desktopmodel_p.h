// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once
#include <QAbstractListModel>
#include <QFile>
#include <QDir>
#include <QFileIconProvider>
#include <QBasicTimer>
#include <QTimer>
#include <QCollator>
#include <QVector>
#include <QHash>
#include <QMimeData>
#include "fsnode.h"

class LSUDiskDevice;
class LSFSThread;
class LSDesktopModel;
class LSDesktopModelPrivate
{
private:
    friend class LSDesktopModel;
    LSDesktopModelPrivate(LSDesktopModel *parent);
    ~LSDesktopModelPrivate() = default;
    LSDesktopModel *d;
    bool hasFile(const QString &file);
    int rowForFile(const QString &file);
    void initialPopulation();
    LSUDiskDevice* deviceForPath(const QString &path);
private:
    QDir m_rootDir;
    LSFSNode m_root;
    QFileSystemWatcher *m_fileInfoGatherer;
    bool m_readOnly = true;
    QList<LSUDiskDevice*> m_devices;
    QList<LSFSNode*> m_files;
};
