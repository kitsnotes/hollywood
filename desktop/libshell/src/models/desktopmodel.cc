// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "desktopmodel.h"
#include "desktopmodel_p.h"
#include "fsnode.h"
#include "fileinfo.h"
#include "disks.h"
#include "commonfunctions.h"

#include <QFileSystemWatcher>

LSDesktopModelPrivate::LSDesktopModelPrivate(LSDesktopModel *parent)
    : d(parent)
    , m_fileInfoGatherer(new QFileSystemWatcher(parent))
{
    initialPopulation();
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
        {
            auto offset = m_devices.count();
            auto idx = m_files.indexOf(f);
            return offset+idx;
        }
    }
    return -1;
}

void LSDesktopModelPrivate::initialPopulation()
{
    // loop and find our root partition - we want that first
    for(auto dev : LSCommonFunctions::instance()->udiskManager()->devices)
    {
        if(dev == nullptr)
            continue;

        if(dev->mountpoint() == "/")
        {
            m_devices.append(dev);
            break;
        }
    }

    for(auto dev2 : LSCommonFunctions::instance()->udiskManager()->devices)
    {
        if(dev2 == nullptr)
            continue;

        // already handled root
        if(dev2->mountpoint() == "/")
            continue;

        // do we have an ignore hint?
        if(dev2->hintIgnore())
            continue;

        if(dev2->name().toUpper() == "HWRECOVERY")
            continue;

        if(dev2->name().toUpper() == "HWBOOT")
            continue;

        // is this an optical drive? we always track them
        if(dev2->isOptical())
        {
            m_devices.append(dev2);
            continue;
        }

        QStringList pathparts = dev2->path().split("/");
        // if type == block_devices, dev will be the /dev path (loopX, sdaX etc)
        // if type == drive, it will be the drive name if just a drive, partition name if mounted
        // or volume name on optical/etc
        auto dev = pathparts.takeLast();
        // type will be block_devices or drives
        auto type = pathparts.takeLast();

        // loop devices need to be *mounted* before we care
        if(type == "block_devices" && dev.startsWith("loop"))
        {
            if(!dev2->filesystem().isEmpty())
                m_devices.append(dev2);

            continue;
        }

        if(!dev2->filesystem().isEmpty())
            m_devices.append(dev2);
    }
}

void LSDesktopModel::mediaChanged(QString path, bool media)
{
    auto dev = p->deviceForPath(path);
    if(dev)
    {
        auto row = p->m_devices.indexOf(dev);
        emit dataChanged(index(row, 0), index(row, 0));
    }
}

void LSDesktopModel::mountpointChanged(QString path, QString mountpoint)
{
    auto dev = p->deviceForPath(path);
    if(dev)
    {
        // this is a device we already have in our sorted local model
        auto row = p->m_devices.indexOf(dev);
        if(dev->isLoopDevice() && mountpoint.isEmpty())
        {
            // unmount a loop device - remove the entry
            emit beginRemoveRows(QModelIndex(), row, row);
            p->m_devices.removeOne(dev);
            emit endRemoveRows();
        }
        else
            emit dataChanged(index(row, 0), index(row, 0));
    }
    else
    {
        // a loop device needs to be added
        auto newdev = LSCommonFunctions::instance()->udiskManager()->devices[path];
        if(newdev)
        {
            if(newdev->isLoopDevice())
            {
                emit beginInsertRows(QModelIndex(), rowCount(), rowCount());
                p->m_devices.append(newdev);
                emit endInsertRows();
            }
        }
    }
}

LSUDiskDevice *LSDesktopModelPrivate::deviceForPath(const QString &path)
{
    for(auto dev : m_devices)
    {
        if(dev->path() == path)
            return dev;
    }

    return nullptr;
}

void LSDesktopModel::foundNewDevice(QString path)
{
    // see if it's a device we already track
    auto dev = p->deviceForPath(path);
    if(dev != nullptr)
        return; // don't take action if it exists

    auto newdev = LSCommonFunctions::instance()->udiskManager()->devices[path];
    if(newdev)
    {
        if(newdev->isLoopDevice())
            return; // handled in mountpointChanged - we don't want empties

        if(newdev->hasMedia() && !newdev->filesystem().isEmpty())
        {
            emit beginInsertRows(QModelIndex(), rowCount(), rowCount());
            p->m_devices.append(newdev);
            emit endInsertRows();
        }
    }
}

void LSDesktopModel::removedDevice(QString path)
{
    auto dev = p->deviceForPath(path);
    if(dev)
    {
        auto row = p->m_devices.indexOf(dev);
        emit beginRemoveRows(QModelIndex(), row, row);
        p->m_devices.removeOne(dev);
        emit endRemoveRows();
    }
}

LSDesktopModel::LSDesktopModel(QObject *parent)
    : QAbstractListModel(parent)
    , p(new LSDesktopModelPrivate(this))
{
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::mediaChanged,
            this, &LSDesktopModel::mediaChanged);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::mountpointChanged,
            this, &LSDesktopModel::mountpointChanged);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::foundNewDevice,
            this, &LSDesktopModel::foundNewDevice);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::removedDevice,
            this, &LSDesktopModel::removedDevice);

    p->m_rootDir = QDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    p->m_fileInfoGatherer->addPath(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    refreshDesktopFolder();
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::directoryChanged, this, &LSDesktopModel::refreshDesktopFolder);
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::fileChanged, this, &LSDesktopModel::refreshDesktopFolder);

}

int LSDesktopModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return p->m_devices.count()+p->m_files.count()+1;
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

Qt::ItemFlags LSDesktopModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
}

QIcon LSDesktopModel::icon(const QModelIndex &index) const
{
    if(index.row() > p->m_devices.count()+p->m_files.count()-1)
        return QIcon::fromTheme("user-trash");

    auto row = index.row();
    if(row < p->m_devices.count())
        return p->m_devices.at(row)->icon();

    auto offset = p->m_devices.count();

    return p->m_files.at(row-offset)->icon();
}

QString LSDesktopModel::name(const QModelIndex &index) const
{
    if(index.row() > p->m_devices.count()+p->m_files.count()-1)
        return tr("Trash");

    auto row = index.row();
    if(row < p->m_devices.count())
        return p->m_devices.at(row)->name();

    auto offset = p->m_devices.count();
    if(p->m_files.at(row-offset)->isDesktopFile())
        return p->m_files.at(row-offset)->desktopFileNameEntry();

    return p->m_files.at(row-offset)->fileName;
}

bool LSDesktopModel::isHidden(const QModelIndex &index) const
{
    if(index.row() > p->m_devices.count()+p->m_files.count()-1)
        return false;

    auto row = index.row();
    if(row < p->m_devices.count())
        return false;

    auto offset = p->m_devices.count();
    if(p->m_files.at(row-offset))
        return p->m_files.at(row-offset)->isHidden();

    return false;
}

bool LSDesktopModel::isSymlink(const QModelIndex &index) const
{
    if(index.row() > p->m_devices.count()+p->m_files.count()-1)
        return false;

    auto row = index.row();
    if(row < p->m_devices.count())
        return false;

    auto offset = p->m_devices.count();

    if(p->m_files.at(row-offset))
        return p->m_files.at(row-offset)->isSymLink();

    return false;
}

bool LSDesktopModel::isDevice(const QModelIndex &index) const
{
    if(index.row() >= p->m_devices.count())
        return false;

    return true;
}

QString LSDesktopModel::description(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QString("");
}

QFileInfo LSDesktopModel::fileInfo(const QModelIndex &index) const
{
    if(index.row() > p->m_devices.count()+p->m_files.count()-1)
        return QFileInfo();

    auto row = index.row();
    if(row < p->m_devices.count())
        return QFileInfo();

    auto offset = p->m_devices.count();
    return p->m_files.at(row-offset)->fileInfo();
}

QUrl LSDesktopModel::url(const QModelIndex &index) const
{
    if(isTrash(index))
        return QUrl("trash://");

    auto row = index.row();
    if(row < p->m_devices.count())
    {
        const auto dev = p->m_devices.at(row);
        if(dev)
        {
            if(!dev->mountpoint().isEmpty())
                return QUrl::fromLocalFile(dev->mountpoint());
        }

        return QUrl();
    }
    else
    {
        auto offset = p->m_devices.count();
        return QUrl::fromLocalFile(p->m_files.at(row-offset)->fileInfo().absoluteFilePath());
    }
}

bool LSDesktopModel::isTrash(const QModelIndex &index) const
{
    // trash is *always* the last item in the index
    if(index.row() == p->m_files.count()+p->m_devices.count())
        return true;

    return false;
}

bool LSDesktopModel::isDesktop(const QModelIndex &index) const
{
    // return false on trash
    if(index.row() >= p->m_devices.count()+p->m_files.count())
        return false;

    auto row = index.row();
    if(row < p->m_devices.count())
        return false;

    auto offset = p->m_devices.count();

    if(p->m_files.at(row-offset)->isDesktopFile())
        return true;

    return false;
}

LSDesktopEntry *LSDesktopModel::desktopFileForIndex(const QModelIndex &index)
{
    // return nullptr on trash
    if(index.row() >= p->m_devices.count()+p->m_files.count())
        return nullptr;

    auto row = index.row();
    if(row < p->m_devices.count())
        return nullptr;

    auto offset = p->m_devices.count();

    if(p->m_files.at(row-offset)->isDesktopFile())
        return p->m_files.at(row-offset)->m_desktop;

    return nullptr;
}

LSUDiskDevice *LSDesktopModel::deviceForIndex(const QModelIndex &index)
{
    if(index.row() >= p->m_devices.count())
        return nullptr;

    auto row = index.row();
    auto dev = p->m_devices[row];
    if(dev)
        return dev;

    return nullptr;
}

QMimeData *LSDesktopModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;
    QList<QModelIndex>::const_iterator it = indexes.begin();
    /*for (; it != indexes.end(); ++it)
        if ((*it).column() == 0)
            urls << QUrl::fromLocalFile(filePath(*it));*/
    QMimeData *data = new QMimeData();
    data->setUrls(urls);
    return data;
}


bool LSDesktopModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    // see if data is something we can handle
    // right now we only handle urls of certain schemes
    if(!data->hasUrls())
        return false;

    bool has_file = false;
    bool has_app = false;
    bool has_virt = false;
    bool has_net = false;
    bool has_trash = false;

    data->urls();
    if(!has_file && !has_app && !has_virt && !has_net && !has_trash)
        return false; // no types we handle

    // find the item that we are targeting
    auto target_item = index(row, column);
    if(target_item.isValid())
    {
        if(isTrash(target_item))
        {
            // see if we can trash items in the selection

            return true;
        }
        else if(isDesktop(target_item))
        {
            // see if we can execute the selection with the specified desktop
            return false;
        }
        else if(isDevice(target_item))
        {
            // user dropped on a device - see if it's writable and proceed
            return false;
        }
    }
    else
    {
        // this should be 'empty desktop' space - so paste into ~/Desktop
        if(action == Qt::CopyAction)
        return true;
    }
}

void LSDesktopModel::refreshDesktopFolder()
{
    auto olditems = p->m_files;
    QList<LSFSNode*> newFiles;
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    auto items = dir.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
    auto offset = p->m_devices.count();
    for (const auto &i : items)
    {
        QString fileName = i.fileName();
        Q_ASSERT(!fileName.isEmpty());
        int origrow = p->rowForFile(fileName)-offset;
        if(!p->hasFile(fileName))
        {
            LSExtendedFileInfo info(i);
            LSFSNode *node = new LSFSNode(fileName, &p->m_root);
            node->populate(info);
            int row = offset+p->m_files.count()-1;
            beginInsertRows(QModelIndex(), row,row);
            newFiles.append(node);
            endInsertRows();
        }
        else
        {
            newFiles.append(olditems[origrow]);
            emit dataChanged(index(origrow, 0), index(origrow, columnCount()-1));
        }
    }
    for(auto &mp : olditems)
    {
        if(!p->m_files.contains(mp))
        {
            int row = p->rowForFile(mp->fileName);
            qDebug() << "removing row" << row;
            beginRemoveRows(QModelIndex(), row, row);
            delete mp;
            endRemoveRows();
        }
    }
    p->m_files = newFiles;
}

