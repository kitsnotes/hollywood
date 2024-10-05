// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpair.h>
#include <QtCore/qdir.h>
#include <QtGui/qicon.h>
#include <QtCore/qdiriterator.h>

class ExtendedInformation;
class HWFileIconProvider;
class LSTrashModelPrivate;
class QFileIconProvider;
class LSTrashModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool nameFilterDisables READ nameFilterDisables WRITE setNameFilterDisables)

Q_SIGNALS:
    void rootPathChanged(const QString &newPath);
    void fileRenamed(const QString &path, const QString &oldName, const QString &newName);
    void directoryLoaded(const QString &path);
    void sortingChanged();
public:
    enum Roles {
        FileIconRole = Qt::DecorationRole,
        FilePathRole = Qt::UserRole + 1,
        FileNameRole = Qt::UserRole + 2
    };

    enum Column
    {
        Name,
        Size,
        Kind,
        DeletionDate,
        OriginalPath
    };

    enum Option
    {
        DontWatchForChanges         = 0x00000001,
        DontResolveSymlinks         = 0x00000002,
        DontUseCustomDirectoryIcons = 0x00000004
    };
    Q_ENUM(Option)
    Q_DECLARE_FLAGS(Options, Option)


    explicit LSTrashModel(QObject *parent = Q_NULLPTR);
    ~LSTrashModel();
    void init();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex index(const QString &path, int column = 0) const;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const Q_DECL_OVERRIDE;

    using QObject::parent;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    bool canFetchMore(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    void fetchMore(const QModelIndex &parent) Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant myComputer(int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;
    int sortColumn() const;
    Qt::SortOrder sortOrder() const;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    void setOption(Option option, bool);
    bool testOption(Option option) const;
    void setOptions(Options options);
    Options options() const;

    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) Q_DECL_OVERRIDE;
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;

    // LSFSModel specific API

    HWFileIconProvider *iconProvider() const;

    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;

    void setNameFilterDisables(bool enable);
    bool nameFilterDisables() const;

    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;

    QString filePath(const QModelIndex &index) const;
    bool isDir(const QModelIndex &index) const;
    qint64 size(const QModelIndex &index) const;
    QString type(const QModelIndex &index) const;
    QDateTime deleteTime(const QModelIndex &index) const;
    QString originalPath(const QModelIndex &index) const;
    QString rootPath() const;

    inline QString fileName(const QModelIndex &index) const;
    inline QIcon fileIcon(const QModelIndex &index) const;

    static QString xdgTrashDir();
protected:
    LSTrashModel(LSTrashModelPrivate &, QObject *parent = Q_NULLPTR);
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    QModelIndex setRootPath();

    Q_DECLARE_PRIVATE(LSTrashModel)
    Q_DISABLE_COPY(LSTrashModel)

    Q_PRIVATE_SLOT(d_func(), void _hwtrash_directoryChanged(const QString &directory, const QStringList &list))
    Q_PRIVATE_SLOT(d_func(), void _hwtrash_performDelayedSort())
    Q_PRIVATE_SLOT(d_func(), void _hwtrash_fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &))
    Q_PRIVATE_SLOT(d_func(), void _hwtrash_resolvedName(const QString &fileName, const QString &resolvedName))

};
