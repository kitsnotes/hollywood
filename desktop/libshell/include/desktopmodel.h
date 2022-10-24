#ifndef LSDESKTOPMODEL_H
#define LSDESKTOPMODEL_H

#include <QAbstractListModel>
#include <QFile>
#include <QDir>
#include <QFileIconProvider>
#include <QBasicTimer>
#include <QTimer>
#include <QCollator>
#include <QVector>
#include <QHash>
#include <QMimeData>

#include "fsnode.h"

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
    QDir m_rootDir;
    LSFSNode m_root;
    //LSFSThread m_fileInfoGatherer;
    bool m_readOnly = true;
    QList<LSFSNode*> m_files;
};

#endif // LSDESKTOPMODEL_H
