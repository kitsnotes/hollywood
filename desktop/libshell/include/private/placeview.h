// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QListView>

class LSPlaceView : public QListView
{
    Q_OBJECT
public:
    explicit LSPlaceView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model) override;
};
