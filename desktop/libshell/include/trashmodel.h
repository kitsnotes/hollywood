#ifndef LSDESKTOPMODEL_H
#define LSDESKTOPMODEL_H

#include <QAbstractListModel>

#include <QFileInfo>
#include "libshell_int.h"

class LSTrashModelItem;
class LSTrashModelPrivate;
class LIBSHELL_EXPORT LSTrashModel : public QAbstractListModel
{
public:
    enum Roles {
        FileIconRole = Qt::DecorationRole,
        FilePathRole = Qt::UserRole + 1,
        FileNameRole = Qt::UserRole + 2,
    };
    explicit LSTrashModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString description(const QModelIndex &index) const;
private slots:
    void refreshTrashFolder();
private:
    LSTrashModelPrivate *p;
};

#endif // LSDESKTOPMODEL_H
