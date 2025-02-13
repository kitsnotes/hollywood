// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QAbstractListModel>
#include <QFileInfo>
#include "libshell_int.h"

class LSUDiskDevice;
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
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QIcon icon(const QModelIndex &index) const;
    QString name(const QModelIndex &index) const;
    QString description(const QModelIndex &index) const;
    QFileInfo fileInfo(const QModelIndex &index) const;
    QUrl url(const QModelIndex &index) const;
    bool isTrash(const QModelIndex &index) const;
    bool isDesktop(const QModelIndex &index) const;
    bool isHidden(const QModelIndex &index) const;
    bool isSymlink(const QModelIndex &index) const;
    bool isDevice(const QModelIndex &index) const;
    LSDesktopEntry* desktopFileForIndex(const QModelIndex &index);
    LSUDiskDevice*  deviceForIndex(const QModelIndex &index);
    Qt::DropActions supportedDropActions() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;
private slots:
    void refreshDesktopFolder();
    void mediaChanged(QString path, bool media);
    void mountpointChanged(QString path, QString mountpoint);
    void foundNewDevice(QString path);
    void removedDevice(QString path);
private:
    friend class LSDesktopModelPrivate;
    LSDesktopModelPrivate *p;
};
