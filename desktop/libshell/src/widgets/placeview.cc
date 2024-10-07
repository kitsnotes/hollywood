// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "placeview.h"

LSPlaceView::LSPlaceView(QWidget *parent)
    :QListView(parent)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setUniformItemSizes(true);
    setIconSize(QSize(24, 24));

    setFrameShape(QFrame::NoFrame);
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    setPalette(p);
    viewport()->setAutoFillBackground(false);
    setMinimumHeight(100);
}

void LSPlaceView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    resizeDesired();
}

void LSPlaceView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    resizeDesired();
    QListView::dataChanged(topLeft, bottomRight, roles);
}

void LSPlaceView::resizeDesired()
{
    int rows = model()->rowCount();
    int size = rows * (iconSize().height()+4);
    int gap = rows * spacing();
    int dheight = rows+size+gap;
    qDebug() << "desired height" << dheight;
    setMinimumHeight(dheight);
    setMaximumHeight(dheight);
}
