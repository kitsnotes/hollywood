#include "desktopmodel.h"
#include "desktopmodel_p.h"
#include "fsnode.h"
#include "fileinfo.h"
#include <QFileSystemWatcher>

LSDesktopModelPrivate::LSDesktopModelPrivate(LSDesktopModel *parent)
    : d(parent)
    , m_fileInfoGatherer(new QFileSystemWatcher(parent))
{

}

bool LSDesktopModelPrivate::hasFile(const QString &file)
{
    for(auto f : m_files)
    {
        if(f->fileName == file)
            return true;
    }
    return false;
}

int LSDesktopModelPrivate::rowForFile(const QString &file)
{
    for(auto f : m_files)
    {
        if(f->fileName == file)
            return m_files.indexOf(f);
    }
    return -1;
}

LSDesktopModel::LSDesktopModel(QObject *parent)
    : QAbstractListModel(parent)
    , p(new LSDesktopModelPrivate(this))
{
    p->m_rootDir = QDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    p->m_fileInfoGatherer->addPath(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    refreshDesktopFolder();
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::directoryChanged, this, &LSDesktopModel::refreshDesktopFolder);
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::fileChanged, this, &LSDesktopModel::refreshDesktopFolder);
}

int LSDesktopModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return p->m_files.count()+1;
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
            case 0: return name(index);
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return icon(index);
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    case Qt::UserRole+4:
        return (bool)isHidden(index);
    case Qt::UserRole+5:
        return (bool)isSymlink(index);
// TODO: hack. fix?
#if QT_VERSION >= 0x060000
    case Qt::UserRole+99:
            if(index.row() > p->m_files.count()-1)
                return QVariant();
            return QVariant::fromValue(p->m_files.at(index.row())->info);
#endif
    }

    return QVariant();
}

QIcon LSDesktopModel::icon(const QModelIndex &index) const
{
    if(index.row() > p->m_files.count()-1)
        return QIcon::fromTheme("user-trash");

    return p->m_files.at(index.row())->icon();
}

QString LSDesktopModel::name(const QModelIndex &index) const
{
    if(index.row() > p->m_files.count()-1)
        return tr("Trash");

    if(p->m_files.at(index.row())->isDesktopFile())
        return p->m_files.at(index.row())->desktopFileNameEntry();

    return p->m_files.at(index.row())->fileName;
}

bool LSDesktopModel::isHidden(const QModelIndex &index) const
{
    if(index.row() > p->m_files.count()-1)
        return false;

    if(p->m_files.at(index.row()))
        return p->m_files.at(index.row())->isHidden();

    return false;
}

bool LSDesktopModel::isSymlink(const QModelIndex &index) const
{
    if(index.row() > p->m_files.count()-1)
        return false;

    if(p->m_files.at(index.row()))
        return p->m_files.at(index.row())->isSymLink();

    return false;
}

QString LSDesktopModel::description(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QString("");
}

QFileInfo LSDesktopModel::fileInfo(const QModelIndex &index) const
{
    return p->m_files.at(index.row())->fileInfo();
}

bool LSDesktopModel::isTrash(const QModelIndex &index) const
{
    if(index.row() == p->m_files.count())
        return true;

    return false;
}

bool LSDesktopModel::isDesktop(const QModelIndex &index) const
{
    if(p->m_files.at(index.row())->isDesktopFile())
        return true;

    return false;
}

LSDesktopEntry *LSDesktopModel::desktopFileForIndex(const QModelIndex &index)
{
    if(p->m_files.at(index.row())->isDesktopFile())
        return p->m_files.at(index.row())->m_desktop;

    return nullptr;
}

void LSDesktopModel::refreshDesktopFolder()
{
    auto olditems = p->m_files;
    QList<LSFSNode*> newFiles;
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    auto items = dir.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
    for (const auto &i : items)
    {
        QString fileName = i.fileName();
        qDebug() << fileName;
        Q_ASSERT(!fileName.isEmpty());
        int row = p->rowForFile(fileName);
        if(!p->hasFile(fileName))
        {
            LSExtendedFileInfo info(i);
            LSFSNode *node = new LSFSNode(fileName, &p->m_root);
            node->populate(info);
            int row = p->m_files.count()-1;
            beginInsertRows(QModelIndex(), row,row);
            newFiles.append(node);
            endInsertRows();
        }
        else
        {
            newFiles.append(olditems[row]);
            emit dataChanged(index(row, 0), index(row, columnCount()-1));
        }
    }
    for(auto &mp : olditems)
    {
        if(!p->m_files.contains(mp))
        {
            int row = p->rowForFile(mp->fileName);
            beginRemoveRows({}, row, row);
            delete mp;
            endRemoveRows();
        }
    }
    p->m_files = newFiles;
}

