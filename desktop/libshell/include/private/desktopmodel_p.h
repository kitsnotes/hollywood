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

class LSDesktopModel;
class LSDesktopModelPrivate
{
private:
    friend class LSDesktopModel;
    LSDesktopModelPrivate(LSDesktopModel *parent);
    ~LSDesktopModelPrivate() = default;
    LSDesktopModel *d;
private:
    QDir m_rootDir;
    LSFSNode m_root;
    //LSFSThread m_fileInfoGatherer;
    bool m_readOnly = true;
    QList<LSFSNode*> m_files;
};

#endif // DESKTOPMODEL_P_H
