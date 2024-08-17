// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "placeview.h"

LSPlaceView::LSPlaceView(QWidget *parent)
    :QTreeView(parent)
{
    setHeaderHidden(true);
    setAnimated(true);
    setRootIsDecorated(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setIndentation(3);
    setIconSize(QSize(24, 24));

    setFrameShape(QFrame::NoFrame);
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, p.color(QPalette::WindowText));
    setPalette(p);
    viewport()->setAutoFillBackground(false);
    setMinimumHeight(100);
}
