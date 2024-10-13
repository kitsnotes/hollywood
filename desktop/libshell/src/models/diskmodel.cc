#include "lsdiskmodel.h"
#include "commonfunctions.h"
#include "disks.h"

#include <QStringList>

LSDiskModel::LSDiskModel(QObject *parent)
    : QAbstractListModel(parent)
{
    initialPopulation();
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::mediaChanged,
            this, &LSDiskModel::mediaChanged);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::mountpointChanged,
            this, &LSDiskModel::mountpointChanged);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::foundNewDevice,
            this, &LSDiskModel::foundNewDevice);
    connect(LSCommonFunctions::instance()->udiskManager(), &LSUDisks::removedDevice,
            this, &LSDiskModel::removedDevice);

    /*for(auto dev : LSCommonFunctions::instance()->udiskManager()->devices)
    {
        if(dev == nullptr)
            continue;

        qDebug() << dev->name() << dev->mountpoint() << dev->path() << dev->filesystem() << dev->removable();
    }*/
}

QVariant LSDiskModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    // FIXME: Implement me!

    return QVariant();
}

int LSDiskModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_devices.count();
}

QVariant LSDiskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    LSUDiskDevice *d = m_devices.at(index.row());
    if(!d)
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        return driveDisplayName(d);
    case Qt::DecorationRole:
        return driveIcon(d);
    default:
        return QVariant();
    }

    return QVariant();
}

LSUDiskDevice *LSDiskModel::device(const QModelIndex &index)
{
    if(!index.isValid())
        return nullptr;

    LSUDiskDevice *d = m_devices.at(index.row());
    if(!d)
        return nullptr;

    return d;
}

LSUDiskDevice *LSDiskModel::deviceForPath(const QString &path)
{
    for(auto dev : m_devices)
    {
        if(dev->path() == path)
            return dev;
    }

    return nullptr;
}

LSUDiskDevice *LSDiskModel::deviceForMount(const QString &mount)
{
    for(auto dev : m_devices)
    {
        if(dev->mountpoint() == mount)
            return dev;
    }

    return nullptr;
}

QModelIndex LSDiskModel::indexOf(LSUDiskDevice *dev) const
{
    if(m_devices.contains(dev))
    {
        auto row = m_devices.indexOf(dev);
        return index(row,0);
    }
    return QModelIndex();
}

void LSDiskModel::mediaChanged(QString path, bool media)
{
    Q_UNUSED(media);
    //qDebug() << "LSDiskModel::mediaChanged" << path << media;
    auto dev = deviceForPath(path);
    if(dev)
    {
        auto row = m_devices.indexOf(dev);
        emit dataChanged(index(row, 0), index(row, 0));
    }
}

void LSDiskModel::mountpointChanged(QString path, QString mountpoint)
{
    auto dev = deviceForPath(path);
    if(dev)
    {
        // this is a device we already have in our sorted local model
        auto row = m_devices.indexOf(dev);
        if(dev->isLoopDevice() && mountpoint.isEmpty())
        {
            // unmount a loop device - remove the entry
            emit beginRemoveRows(QModelIndex(), row, row);
            m_devices.removeOne(dev);
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
                m_devices.append(newdev);
                emit endInsertRows();
            }
        }
    }
}

void LSDiskModel::foundNewDevice(QString path)
{
    // see if it's a device we already track
    auto dev = deviceForPath(path);
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
            m_devices.append(newdev);
            emit endInsertRows();
        }
    }
}

void LSDiskModel::removedDevice(QString path)
{
    auto dev = deviceForPath(path);
    if(dev)
    {
        auto row = m_devices.indexOf(dev);
        emit beginRemoveRows(QModelIndex(), row, row);
        m_devices.removeOne(dev);
        emit endRemoveRows();
    }
}

void LSDiskModel::initialPopulation()
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

QString LSDiskModel::driveDisplayName(const LSUDiskDevice *dev) const
{
    if(dev == nullptr)
        return QString();

    return dev->name();
}

QIcon LSDiskModel::driveIcon(const LSUDiskDevice *dev) const
{
    if(dev == nullptr)
        return QIcon();

    return dev->icon();
}
