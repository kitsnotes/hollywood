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
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);

    setFrameShape(QFrame::NoFrame);
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    setPalette(p);
    viewport()->setAutoFillBackground(false);
    setMinimumWidth(450);
}

void LSPlaceView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    updateGeometry();
}
