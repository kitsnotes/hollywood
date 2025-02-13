// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qpair.h>
#include <QtCore/qdir.h>
#include <QtGui/qicon.h>
#include <QtCore/qdiriterator.h>

class ExtendedInformation;
class HWFileIconProvider;
class LSFSModelPrivate;
class QFileIconProvider;
class LSFSModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool resolveSymlinks READ resolveSymlinks WRITE setResolveSymlinks)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
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
        FileNameRole = Qt::UserRole + 2,
        FilePermissions = Qt::UserRole + 3,
        FileHidden = Qt::UserRole + 4,
        FileSymlink = Qt::UserRole + 5,
        FileAbsolutePath = Qt::UserRole + 6,
        FileInfoRole = Qt::UserRole + 99
    };

    enum Column
    {
        Name,
        Size,
        Kind,
        ModificationDate,
        Owner,
        Group,
        Comment // Keep comment LAST!
    };

    enum Option
    {
        DontWatchForChanges         = 0x00000001,
        DontResolveSymlinks         = 0x00000002,
        DontUseCustomDirectoryIcons = 0x00000004
    };
    Q_ENUM(Option)
    Q_DECLARE_FLAGS(Options, Option)


    explicit LSFSModel(QObject *parent = Q_NULLPTR);
    ~LSFSModel();

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
    QModelIndex setRootPath(const QString &path);
    QString rootPath() const;
    QDir rootDirectory() const;

    HWFileIconProvider *iconProvider() const;

    void setFilter(QDir::Filters filters);
    QDir::Filters filter() const;

    void setResolveSymlinks(bool enable);
    bool resolveSymlinks() const;

    void setReadOnly(bool enable);
    bool isReadOnly() const;

    void setNameFilterDisables(bool enable);
    bool nameFilterDisables() const;

    void setNameFilters(const QStringList &filters);
    QStringList nameFilters() const;

    QString filePath(const QModelIndex &index) const;
    bool isDir(const QModelIndex &index) const;
    bool isDesktop(const QModelIndex &index) const;
    QString desktopFileForIndex(const QModelIndex &index) const;
    qint64 size(const QModelIndex &index) const;
    QString type(const QModelIndex &index) const;
    QDateTime lastModified(const QModelIndex &index) const;

    QModelIndex mkdir(const QModelIndex &parent, const QString &name);
    bool rmdir(const QModelIndex &index);
    inline QString fileName(const QModelIndex &index) const;
    inline QIcon fileIcon(const QModelIndex &index) const;
    QFile::Permissions permissions(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    bool remove(const QModelIndex &index);
    bool isHidden(const QModelIndex &index) const;
    bool isSymlink(const QModelIndex &index) const;
    QString absolutePath(const QModelIndex &index) const;

protected:
    LSFSModel(LSFSModelPrivate &, QObject *parent = Q_NULLPTR);
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(LSFSModel)
    Q_DISABLE_COPY(LSFSModel)

    Q_PRIVATE_SLOT(d_func(), void _q_directoryChanged(const QString &directory, const QStringList &list))
    Q_PRIVATE_SLOT(d_func(), void _q_performDelayedSort())
    Q_PRIVATE_SLOT(d_func(), void _q_fileSystemChanged(const QString &path, const QVector<QPair<QString, QFileInfo> > &))
    Q_PRIVATE_SLOT(d_func(), void _q_resolvedName(const QString &fileName, const QString &resolvedName))

    friend class QFileDialogPrivate;
};

inline QString LSFSModel::fileName(const QModelIndex &aindex) const
{ return aindex.data(Qt::DisplayRole).toString(); }
inline QIcon LSFSModel::fileIcon(const QModelIndex &aindex) const
{ return qvariant_cast<QIcon>(aindex.data(Qt::DecorationRole)); }

