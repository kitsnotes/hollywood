#ifndef LSDESKTOPMODEL_H
#define LSDESKTOPMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>
#include "libshell_int.h"
class LSDesktopEntry;
class LSDesktopModelPrivate;
class LIBSHELL_EXPORT LSDesktopModel : public QAbstractListModel
{
public:
    enum Roles {
        FileIconRole = Qt::DecorationRole,
        FilePathRole = Qt::UserRole + 1,
        FileNameRole = Qt::UserRole + 2,
        FilePermissions = Qt::UserRole + 3,
        FileOwner = Qt::UserRole + 4,
        FileGroup = Qt::UserRole + 5
    };
    explicit LSDesktopModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString description(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    bool isTrash(const QModelIndex &index) const;
    bool isDesktop(const QModelIndex &index) const;
    bool isHidden(const QModelIndex &index) const;
    bool isSymlink(const QModelIndex &index) const;
    LSDesktopEntry *desktopFileForIndex(const QModelIndex &index);
private slots:
    void refreshDesktopFolder();
private:
    LSDesktopModelPrivate *p;
};

#endif // LSDESKTOPMODEL_H
