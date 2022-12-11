#ifndef DESKTOPMODEL_P_H
#define DESKTOPMODEL_P_H

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
private:
    QDir m_rootDir;
    LSFSNode m_root;
    QFileSystemWatcher *m_fileInfoGatherer;
    bool m_readOnly = true;
    QList<LSFSNode*> m_files;
};

#endif // DESKTOPMODEL_P_H
