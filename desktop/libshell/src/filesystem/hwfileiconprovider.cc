#include "hwfileiconprovider.h"
#include <QMimeDatabase>
#include <QMimeType>
#include "directories.h"

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
    if(mtname == "inode-directory")
    {
        // see if we are a freedesktop directory
        LSDirectories d;
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Desktop))
            return QIcon::fromTheme("user-desktop");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Documents))
            return QIcon::fromTheme("folder-documents");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Downloads))
            return QIcon::fromTheme("folder-downloads");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Pictures))
            return QIcon::fromTheme("folder-pictures");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Music))
            return QIcon::fromTheme("folder-music");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Videos))
            return QIcon::fromTheme("folder-videos");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::Templates))
            return QIcon::fromTheme("folder-templates");
        if(info.canonicalFilePath() == d.userDir(LSDirectories::PublicShare))
            return QIcon::fromTheme("folder-publicshare");

        if(info.canonicalFilePath() == qgetenv("HOME"))
            return QIcon::fromTheme("user-home");
    }
    return QIcon::fromTheme(mtname);
}

