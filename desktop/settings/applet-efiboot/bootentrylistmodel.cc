/* Hollywood Operating System
 * System Settings - System Startup Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-FileCopyrightText: 2002-2022  Maciej Szeptuch
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * Originated from github.com/Neverous/efibooteditor
 */

#include "bootentrylistmodel.h"
#include <QIcon>


BootEntryListModel::BootEntryListModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

auto BootEntryListModel::rowCount(const QModelIndex &parent) const -> int
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(entries.count());
}

auto BootEntryListModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if(!index.isValid() || !checkIndex(index))
        return {};

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return entries.at(index.row()).prettyName();
        break;
    case Qt::DecorationRole:
        if (index.column() == 0) {
            return entries.at(index.row()).icon();
        }
        break;
    case Qt::TextAlignmentRole:
        break;
    case Qt::UserRole+1:
        return entries.at(index.row()).statusEntry();
    case Qt::UserRole+2:
        return entries.at(index.row()).is_hollywood;
    case Qt::UserRole+3:
        return entries.at(index.row()).index;
    }

    return QVariant();
    QVariant data;
    data.setValue(&entries.at(index.row()));
    return data;
}

auto BootEntryListModel::changeData(const QModelIndex &index, const Change_fn &change_fn, int role) -> bool
{
    if(role != Qt::EditRole)
        return false;

    if(!index.isValid() || !checkIndex(index))
        return false;

    if(!change_fn(entries[index.row()]))
        return false;

    emit dataChanged(index, index, {role});
    return true;
}

void BootEntryListModel::setNextBootEntry(const QModelIndex &index, bool value)
{
    if(!index.isValid() || !checkIndex(index))
        return;

    if(value)
    {
        if(next_boot == index)
            return;

        if(next_boot.isValid() && checkIndex(next_boot))
            changeData(next_boot, [](BootEntry &entry)
                {
                entry.is_next_boot = false;
                return true; });

        next_boot = index;
        changeData(next_boot, [](BootEntry &entry)
            {
            entry.is_next_boot = true;
            return true; });

        return;
    }

    if(next_boot != index)
        return;

    changeData(next_boot, [](BootEntry &entry)
        {
        entry.is_next_boot = false;
        return true; });

    next_boot = QModelIndex{};
}

auto BootEntryListModel::flags(const QModelIndex &index) const -> Qt::ItemFlags
{
    auto flags = QAbstractListModel::flags(index);
    if(index.isValid() && checkIndex(index))
        return flags | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;

    return flags;
}

auto BootEntryListModel::insertRows(int row, int count, const QModelIndex &parent) -> bool
{
    beginInsertRows(parent, row, row + count - 1);
    for(int c = 0; c < count; ++c)
        entries.insert(static_cast<decltype(entries)::size_type>(row) + c, {});

    endInsertRows();
    return true;
}

auto BootEntryListModel::appendRow(const BootEntry &data, const QModelIndex &parent) -> bool
{
    int row = rowCount(parent);
    beginInsertRows(parent, row, row);
    entries.append(data);
    endInsertRows();
    return true;
}

auto BootEntryListModel::removeRows(int row, int count, const QModelIndex &parent) -> bool
{
    beginRemoveRows(parent, row, row + count - 1);
    for(int c = 0; c < count; ++c)
        entries.removeAt(row);

    endRemoveRows();
    return true;
}

auto BootEntryListModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) -> bool
{
    beginMoveRows(sourceParent, sourceRow, sourceRow + count, destinationParent, destinationChild);
    for(int c = 0; c < count; ++c)
        entries.move(sourceRow, static_cast<decltype(entries)::size_type>(destinationChild) + (sourceRow < destinationChild ? 0 : c));

    endMoveRows();
    return true;
}

void BootEntryListModel::clear()
{
    beginResetModel();
    entries.clear();
    endResetModel();
}
