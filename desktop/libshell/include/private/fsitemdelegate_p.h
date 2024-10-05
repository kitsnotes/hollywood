// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include <QSize>
#include <QColor>
#include <QIcon>

class LSFSItemDelegate;
class LSFSItemDelegatePrivate
{
private:
    friend class LSFSItemDelegate;
    LSFSItemDelegate *d;
    LSFSItemDelegatePrivate(LSFSItemDelegate *parent);
    ~LSFSItemDelegatePrivate() = default;
    QIcon m_symlinkIcon;
    QSize m_iconSize;
    QSize m_itemSize;
    QSize m_margins;
    QColor m_shadowColor;
    bool m_shadowHidden;
    mutable bool m_hasEditor;
};
