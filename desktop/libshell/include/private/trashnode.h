// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#ifndef TRASHNODE_H
#define TRASHNODE_H

#include <sys/xattr.h>

#include <QMimeDatabase>
#include <QSharedPointer>
#include <QFile>
#include <QFileIconProvider>

class LSTrashNode
{
public:
    explicit LSTrashNode(const QString &filename = QString(), LSTrashNode *p = nullptr);
    ~LSTrashNode();

    QString fileName;

    qint64 size() const;
    QString type() const;
    QDateTime deleteTime() const;

    bool isDir() const;
    QFileInfo fileInfo() const;
    bool isFile() const;
    bool caseSensitive() const;
    QIcon icon() const;
    QString originalPath() const;
    void populate(QFileInfo &info);

    bool operator <(const LSTrashNode &node) const;
    bool operator >(const QString &name) const;
    bool operator <(const QString &name) const;
    bool operator !=(const QFileInfo &fileInfo) const;
    bool operator ==(const QString &name) const;
    bool operator ==(const QFileInfo &fileInfo) const;

    int visibleLocation(const QString &childName);
    void updateIcon(QFileIconProvider *iconProvider, const QString &path);
    void retranslateStrings(QFileIconProvider *iconProvider, const QString &path);

    QFileInfo m_info;
    QHash<QString, LSTrashNode *> children;
    QList<QString> visibleChildren;
    LSTrashNode *m_parent;
    bool m_visible = false;
    bool m_popChildren = false;
    int  m_dirtyChildrenIndex = -1;

    QDateTime m_deleteTime;
    QString m_origPath;
};

#endif // TRASHNODE_H
