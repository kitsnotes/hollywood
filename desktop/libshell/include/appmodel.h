// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QAbstractItemModel>

class AppModelPrivate;
class ApplicationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ApplicationModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::DropActions supportedDropActions() const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

private:
    AppModelPrivate *p;
};
