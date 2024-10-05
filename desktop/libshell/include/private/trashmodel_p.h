// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include "trashmodel.h"
#include <private/qabstractitemmodel_p.h>
#include <qabstractitemmodel.h>
//#include "fileinfogatherer_p.h"
#include <qpair.h>
#include <qdir.h>
#include <qicon.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qhash.h>
#include "trashnode.h"
#include "hwfileiconprovider.h"
#include "trashinfogatherer_p.h"

class ExtendedInformation;
class LSTrashModelPrivate;
class QFileIconProvider;

class LSTrashModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(LSTrashModel)

public:
    enum { NumColumns = 5 };

    LSTrashModelPrivate () :
        forceSort(true),
        sortColumn(0),
        sortOrder(Qt::AscendingOrder),
        readOnly(true),
        setRootPath(false),
        filters(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs),
        nameFilterDisables(true), // false on windows, true on mac and unix
        disableRecursiveSort(false)
    {
        delayedSortTimer.setSingleShot(true);

    }

    void init();
    inline bool isHiddenByFilter(LSTrashNode *indexNode, const QModelIndex &index) const
    {
        return (indexNode != &root && !index.isValid());
    }
    LSTrashNode *node(const QModelIndex &index) const;
    LSTrashNode *node(const QString &path, bool fetch = true) const;
    inline QModelIndex index(const QString &path, int column = 0) { return index(node(path), column); }
    QModelIndex index(const LSTrashNode *node, int column = 0) const;
    bool filtersAcceptsNode(const LSTrashNode *node) const;
    bool passNameFilters(const LSTrashNode *node) const;
    void removeNode(LSTrashNode *parentNode, const QString &name);
    LSTrashNode* addNode(LSTrashNode *parentNode, const QString &fileName, const QFileInfo &info);
    void addVisibleFiles(LSTrashNode *parentNode, const QStringList &newFiles);
    void removeVisibleFile(LSTrashNode *parentNode, int visibleLocation);
    void sortChildren(int column, const QModelIndex &parent);
    QString type(const QModelIndex &index) const;
    inline int translateVisibleLocation(LSTrashNode *parent, int row) const {
        if (sortOrder != Qt::AscendingOrder) {
            if (parent->m_dirtyChildrenIndex == -1)
                return parent->visibleChildren.count() - row - 1;

            if (row < parent->m_dirtyChildrenIndex)
                return parent->m_dirtyChildrenIndex - row - 1;
        }

        return row;
    }

    inline static QString myComputer() {
        // ### TODO We should query the system to find out what the string should be
        return LSTrashModel::tr("Computer");
    }

    inline void delayedSort() {
        if (!delayedSortTimer.isActive())
            delayedSortTimer.start(0);
    }

    static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
    {
        return QString::compare(s1, s2, Qt::CaseInsensitive) < 0;
    }

    static bool nodeCaseInsensitiveLessThan(const LSTrashNode &s1, const LSTrashNode &s2)
    {
        return QString::compare(s1.fileName, s2.fileName, Qt::CaseInsensitive) < 0;
    }

    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString displayName(const QModelIndex &index) const;
    QString filePath(const QModelIndex &index) const;
    QString size(const QModelIndex &index) const;
    QString deleteTime(const QModelIndex &index) const;
    static QString size(qint64 bytes);
    QString trashDirectory() const { return m_trashDirectory; }
    struct Fetching {
        QString dir;
        QString file;
        const LSTrashNode *node;
    };

    void _hwtrash_directoryChanged(const QString &directory, const QStringList &list);
    void _hwtrash_performDelayedSort();
    void _hwtrash_fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &);
    void _hwtrash_resolvedName(const QString &fileName, const QString &resolvedName);

    static int naturalCompare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs);
    QDir rootDir;
    LSTrashInfoGatherer fileInfoGatherer;
    QTimer delayedSortTimer;
    bool forceSort;
    int sortColumn;
    Qt::SortOrder sortOrder;
    bool readOnly;
    bool setRootPath;
    QDir::Filters filters;
    QHash<const LSTrashNode*, bool> bypassFilters;
    bool nameFilterDisables;
    //This flag is an optimization for the QFileDialog
    //It enable a sort which is not recursive, it means
    //we sort only what we see.
    bool disableRecursiveSort;

    QString m_trashDirectory;

#if QT_VERSION >= 0x060000
    QMultiHash<int,QByteArray> roleNames;
    std::vector<QRegularExpression> nameFiltersRegexps;
    void rebuildNameFilterRegexps();
#endif
    QStringList nameFilters;
    QHash<QString, QString> resolvedSymLinks;

    LSTrashNode root;
    //LSFSNode *m_root = nullptr;
    QBasicTimer fetchingTimer;
    QList<Fetching> toFetch;

};
Q_DECLARE_TYPEINFO(LSTrashModelPrivate::Fetching, Q_MOVABLE_TYPE);

