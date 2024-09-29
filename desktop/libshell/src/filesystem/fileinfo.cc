#include "fileinfo.h"
#include "desktopentry.h"
#include <QDateTime>
#include <QMimeDatabase>
LSExtendedFileInfo::LSExtendedFileInfo() {}

LSExtendedFileInfo::LSExtendedFileInfo(const QFileInfo &info)
{
    m_info = info;
    QMimeDatabase db;
    displayType = db.mimeTypeForFile(info.fileName()).comment();
}

bool LSExtendedFileInfo::operator ==(const LSExtendedFileInfo &fileInfo) const
{
    bool is_equal = m_info == fileInfo.m_info
            && displayType == fileInfo.displayType
            && permissions() == fileInfo.permissions()
            && lastModified() == fileInfo.lastModified();

    return is_equal;
}

LSExtendedFileInfo::Type LSExtendedFileInfo::type() const
{
    if (m_info.isDir()) {
        return LSExtendedFileInfo::Dir;
    }
    if (m_info.isFile()) {
        return LSExtendedFileInfo::File;
    }
    if (!m_info.exists() && m_info.isSymLink()) {
        return LSExtendedFileInfo::System;
    }
    return LSExtendedFileInfo::System;
}

QFileDevice::Permissions LSExtendedFileInfo::permissions() const
{
    return m_info.permissions();
}

bool LSExtendedFileInfo::isSymLink() const
{
    return m_info.isSymbolicLink();
}

bool LSExtendedFileInfo::isHidden() const
{
    return m_info.isHidden();
}

QFileInfo LSExtendedFileInfo::fileInfo() const
{
    return m_info;
}

QDateTime LSExtendedFileInfo::lastModified() const
{
    return m_info.lastModified();
}

qint64 LSExtendedFileInfo::size() const
{
    qint64 size = -1;
    if (type() == LSExtendedFileInfo::Dir)
        size = 0;
    if (type() == LSExtendedFileInfo::File)
        size = m_info.size();
    if (!m_info.exists() && !m_info.isSymLink())
        size = -1;
    return size;
}

QString LSExtendedFileInfo::owner() const
{
    return m_info.owner();
}

QString LSExtendedFileInfo::group() const
{
    return m_info.group();
}
