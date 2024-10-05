// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include "filesystemmodel.h"

#include <private/qabstractitemmodel_p.h>
#include <qabstractitemmodel.h>
#include "fileinfogatherer_p.h"
#include <qpair.h>
#include <qdir.h>
#include <qicon.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qhash.h>
#include "fsnode.h"

class ExtendedInformation;
class LSFSModelPrivate;
class QFileIconProvider;

class LSFSModelPrivate : public QAbstractItemModelPrivate
{
    Q_DECLARE_PUBLIC(LSFSModel)

public:
    enum { NumColumns = 7 };

    LSFSModelPrivate () :
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
    inline bool isHiddenByFilter(LSFSNode *indexNode, const QModelIndex &index) const
    {
       return (indexNode != &root && !index.isValid());
    }
    LSFSNode *node(const QModelIndex &index) const;
    LSFSNode *node(const QString &path, bool fetch = true) const;
    inline QModelIndex index(const QString &path, int column = 0) { return index(node(path), column); }
    QModelIndex index(const LSFSNode *node, int column = 0) const;
    bool filtersAcceptsNode(const LSFSNode *node) const;
    bool passNameFilters(const LSFSNode *node) const;
    void removeNode(LSFSNode *parentNode, const QString &name);
    LSFSNode* addNode(LSFSNode *parentNode, const QString &fileName, const QFileInfo &info);
    void addVisibleFiles(LSFSNode *parentNode, const QStringList &newFiles);
    void removeVisibleFile(LSFSNode *parentNode, int visibleLocation);
    void sortChildren(int column, const QModelIndex &parent);

    inline int translateVisibleLocation(LSFSNode *parent, int row) const {
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
        return LSFSModel::tr("Computer");
    }

    inline void delayedSort() {
        if (!delayedSortTimer.isActive())
            delayedSortTimer.start(0);
    }

    static bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
    {
       return QString::compare(s1, s2, Qt::CaseInsensitive) < 0;
    }

    static bool nodeCaseInsensitiveLessThan(const LSFSNode &s1, const LSFSNode &s2)
    {
       return QString::compare(s1.fileName, s2.fileName, Qt::CaseInsensitive) < 0;
    }

    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString displayName(const QModelIndex &index) const;
    QString filePath(const QModelIndex &index) const;
    QString size(const QModelIndex &index) const;
    static QString size(qint64 bytes);
    QString type(const QModelIndex &index) const;
    QString time(const QModelIndex &index) const;
    QString owner(const QModelIndex &index) const;
    QString group(const QModelIndex &index) const;
    QString comment(const QModelIndex &index) const;

    struct Fetching {
        QString dir;
        QString file;
        const LSFSNode *node;
    };

    void _q_directoryChanged(const QString &directory, const QStringList &list);
    void _q_performDelayedSort();
    void _q_fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &);
    void _q_resolvedName(const QString &fileName, const QString &resolvedName);

    static int naturalCompare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs);
    QDir rootDir;
    LSFileInfoGatherer fileInfoGatherer;
    QTimer delayedSortTimer;
    bool forceSort;
    int sortColumn;
    Qt::SortOrder sortOrder;
    bool readOnly;
    bool setRootPath;
    QDir::Filters filters;
    QHash<const LSFSNode*, bool> bypassFilters;
    bool nameFilterDisables;
    //This flag is an optimization for the QFileDialog
    //It enable a sort which is not recursive, it means
    //we sort only what we see.
    bool disableRecursiveSort;


#if QT_VERSION >= 0x060000
    QMultiHash<int,QByteArray> roleNames;
    std::vector<QRegularExpression> nameFiltersRegexps;
    void rebuildNameFilterRegexps();
#endif
    QStringList nameFilters;
    QHash<QString, QString> resolvedSymLinks;

    LSFSNode root;
    QBasicTimer fetchingTimer;
    QList<Fetching> toFetch;

};
Q_DECLARE_TYPEINFO(LSFSModelPrivate::Fetching, Q_MOVABLE_TYPE);
