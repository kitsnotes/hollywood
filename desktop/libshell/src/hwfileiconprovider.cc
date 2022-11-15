#include "hwfileiconprovider.h"
#include <QMimeDatabase>
#include <QMimeType>

HWFileIconProvider::HWFileIconProvider()
    : QFileIconProvider() { }

QIcon HWFileIconProvider::icon(IconType type) const
{
    switch(type)
    {
    case QFileIconProvider::Computer:
        return QIcon::fromTheme("computer");
    case QFileIconProvider::Desktop:
        return QIcon::fromTheme("user-desktop");
    case QFileIconProvider::Trashcan:
        return QIcon::fromTheme("user-trash");
    case QFileIconProvider::Network:
        return QIcon::fromTheme("network-workgroup");
    case QFileIconProvider::Drive:
        return QIcon::fromTheme("folder");
    case QFileIconProvider::Folder:
        return QIcon::fromTheme("folder");
    case QFileIconProvider::File:
    default:
        return QIcon::fromTheme("text-x-generic");
    }

    return QIcon();
}

QIcon HWFileIconProvider::icon(const QFileInfo &info) const
{

    if((options() & DontUseCustomDirectoryIcons) && info.isDir())
        return QIcon::fromTheme(QLatin1String("inode-directory"));

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(info);
    auto mtname = type.name();
    mtname.replace('/', '-');
    return QIcon::fromTheme(mtname);
}

