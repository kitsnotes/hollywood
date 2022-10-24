#include "desktopmodel.h"
#include "fsnode.h"
#include "fileinfo.h"

LSDesktopModel::LSDesktopModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug() << "creating desktopmodel";
    /* connect(&m_fileInfoGatherer, &LSFSThread::newListOfFiles, this, &LSDesktopModel::directoryChanged);
    connect(&m_fileInfoGatherer, &LSFSThread::updates, this, &LSDesktopModel::fileSystemChanged);
    connect(&m_fileInfoGatherer, &LSFSThread::nameResolved, this, &LSDesktopModel::resolvedName);
    //connect(&m_fileInfoGatherer, &LSFSThread::directoryLoaded, this, &LSDesktopModel::directoryLoaded); */

    /*m_fileInfoGatherer.watchPaths(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation));
    m_fileInfoGatherer.setWatching(true);*/
}

int LSDesktopModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_files.count();
}

int LSDesktopModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant LSDesktopModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
            case 0: return name(index);
            case 1: return description(index);
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return icon(index);
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    }

    return QVariant();
}

QIcon LSDesktopModel::icon(const QModelIndex &index) const
{
    return m_files.at(index.row())->icon();
}

QString LSDesktopModel::name(const QModelIndex &index) const
{
    if(m_files.at(index.row())->isDesktopFile())
        return m_files.at(index.row())->desktopFileNameEntry();

    return name(index);
}

QString LSDesktopModel::description(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QString("");
}


void LSDesktopModel::directoryChanged(const QString &directory, const QStringList &list)
{
    qDebug() << "LSDesktopModel::directoryChanged:" << directory << "files:" << list.join(',');
}

void LSDesktopModel::fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &items)
{
    qDebug() << "LSDesktopModel::fileSystemChanged:" << path;
    for (const auto &i : items)
    {
        QString fileName = i.first;
        Q_ASSERT(!fileName.isEmpty());
        //LSExtendedFileInfo info = m_fileInfoGatherer.getInfo(i.second);
        //LSFSNode *node = new LSFSNode(i.first, &m_root);
        //node->populate(info);
        //m_files.append(node);
    }
}

void LSDesktopModel::resolvedName(const QString &file, const QString &resolved)
{
    Q_UNUSED(file);
    Q_UNUSED(resolved);
}
