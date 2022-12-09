#ifndef LSINODEITEM_H
#define LSINODEITEM_H

#include "libshell_int.h"
#include <sys/xattr.h>

#include <QMimeDatabase>
#include <QSharedPointer>

class LSDesktopEntry;
class LSExtendedFileInfo;
class LSFSNode
{
public:
    explicit LSFSNode(const QString &filename = QString(), LSFSNode *p = nullptr);
    ~LSFSNode();

    QString fileName;

    qint64 size() const;
    QString type() const;
    QDateTime lastModified() const;
    QString owner() const;
    QString group() const;
    QString desktopFileNameEntry() const;
    QString xattrComment() const;
    QFile::Permissions permissions() const;
    inline bool isReadable() const { return ((permissions() & QFile::ReadUser) != 0); }
    inline bool isWritable() const { return ((permissions() & QFile::WriteUser) != 0); }
    inline bool isExecutable() const { return ((permissions() & QFile::ExeUser) != 0); }
    bool isDir() const;
    QFileInfo fileInfo() const;
    bool isFile() const;
    bool isSystem() const;
    bool isHidden() const;
    bool isSymLink() const;
    bool caseSensitive() const;
    bool isDesktopFile() const;

    QIcon icon() const;

    bool operator <(const LSFSNode &node) const;
    bool operator >(const QString &name) const;
    bool operator <(const QString &name) const;
    bool operator !=(const LSExtendedFileInfo &fileInfo) const;
    bool operator ==(const QString &name) const;
    bool operator ==(const LSExtendedFileInfo &fileInfo) const;

    inline bool hasInformation() const { return info.data() != nullptr; }

    void populate(const LSExtendedFileInfo &fileInfo);

    int visibleLocation(const QString &childName);
    void updateIcon(QFileIconProvider *iconProvider, const QString &path);
    void retranslateStrings(QFileIconProvider *iconProvider, const QString &path);

    QHash<QString, LSFSNode *> children;
    QList<QString> visibleChildren;
    QSharedPointer<LSExtendedFileInfo> info;
    LSDesktopEntry *m_desktop = nullptr;
    LSFSNode *m_parent;
    bool m_visible = false;
    bool m_popChildren = false;
    int  m_dirtyChildrenIndex = -1;

};

#endif // LSINODEITEM_H
