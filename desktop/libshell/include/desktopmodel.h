#ifndef LSDESKTOPMODEL_H
#define LSDESKTOPMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>

class LSDesktopModelPrivate;
class LSDesktopModel : public QAbstractListModel
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
private slots:
    void directoryChanged(const QString &directory, const QStringList &list);
    void fileSystemChanged(const QString &path, const QVector<QPair<QString,QFileInfo> >&items);
    void resolvedName(const QString &file, const QString &resolved);
private:
    LSDesktopModelPrivate *p;
};

#endif // LSDESKTOPMODEL_H
