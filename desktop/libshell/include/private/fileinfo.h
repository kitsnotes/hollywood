#pragma once

#include <QFileInfo>
#include <QIcon>
#include <QString>
#include <QtCore/private/qfilesystemengine_p.h>

class LSDesktopEntry;
class LSExtendedFileInfo
{
public:
    enum Type {
        Dir,
        File,
        System
    };

    LSExtendedFileInfo();
    LSExtendedFileInfo(const QFileInfo &info);
    bool operator ==(const LSExtendedFileInfo &fileInfo) const;
    inline bool isDir() { return type() == Dir; }
    inline bool isFile() { return type() == File; }
    inline bool isSystem() { return type() == System; }
    bool isCaseSensitive() const {
        return QFileSystemEngine::isCaseSensitive();
    }
    Type type() const;
    QFile::Permissions permissions() const;
    bool isSymLink() const;
    bool isHidden() const;
    QFileInfo fileInfo() const;
    QDateTime lastModified() const;
    qint64 size() const;
    QString owner() const;
    QString group() const;
    QString displayType;
    QIcon icon;
private:
    QFileInfo m_info;
};
