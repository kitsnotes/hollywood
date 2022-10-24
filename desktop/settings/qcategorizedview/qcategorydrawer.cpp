/**
  * (c)LGPL2+
  * This file is part of the KDE project
  * Copyright (C) 2007, 2009 Rafael Fernández López <ereslibre@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include "qcategorydrawer.h"

#include <QPainter>
#include <QStyleOption>
#include <QApplication>

#include "qcategorizedview.h"
#include "qcategorizedsortfilterproxymodel.h"

#define HORIZONTAL_HINT 3

class QCategoryDrawer::Private
{
public:
    Private()
        : leftMargin(0)
        , rightMargin(0)
    {
    }

    ~Private()
    {
    }

    int leftMargin;
    int rightMargin;
};

QCategoryDrawer::QCategoryDrawer()
    : d(new Private)
{
    setLeftMargin(2);
    setRightMargin(2);
}

QCategoryDrawer::~QCategoryDrawer()
{
    delete d;
}

void QCategoryDrawer::drawCategory(const QModelIndex &index,
                                   int /*sortRole*/,
                                   const QStyleOption &option,
                                   QPainter *painter) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    const QString category = index.model()->data(index, QCategorizedSortFilterProxyModel::CategoryDisplayRole).toString();
    const QRect optRect = option.rect;
    QFont font(QApplication::font());
    font.setBold(true);
    const QFontMetrics fontMetrics = QFontMetrics(font);

    //BEGIN: text
    {
        QRect textRect(option.rect);
        textRect.setTop(textRect.top() + 7);
        textRect.setLeft(textRect.left() + 7);
        textRect.setHeight(fontMetrics.height());
        textRect.setRight(textRect.right() - 7);

        painter->save();
        painter->setFont(font);
        QColor penColor(option.palette.text().color());
        penColor.setAlphaF(0.6);
        painter->setPen(penColor);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, category);
        painter->restore();
    }
    //END: text
}

int QCategoryDrawer::categoryHeight(const QModelIndex &index, const QStyleOption &option) const
{
    Q_UNUSED(index);
    Q_UNUSED(option)

    QFont font(QApplication::font());
    font.setBold(true);
    QFontMetrics fontMetrics(font);

    const int height = fontMetrics.height() + 1 /* 1 pixel-width gradient */
                                            + 11 /* top and bottom separation */;
    return height;
}

int QCategoryDrawer::leftMargin() const
{
    return d->leftMargin;
}

void QCategoryDrawer::setLeftMargin(int leftMargin)
{
    d->leftMargin = leftMargin;
}

int QCategoryDrawer::rightMargin() const
{
    return d->rightMargin;
}

void QCategoryDrawer::setRightMargin(int rightMargin)
{
    d->rightMargin = rightMargin;
}

QCategoryDrawer &QCategoryDrawer::operator=(const QCategoryDrawer &cd)
{
    d->leftMargin = cd.d->leftMargin;
    d->rightMargin = cd.d->rightMargin;
    return *this;
}

QCategoryDrawerV2::QCategoryDrawerV2(QObject *parent)
    : QObject(parent)
    , QCategoryDrawer()
{
}

QCategoryDrawerV2::~QCategoryDrawerV2()
{
}

void QCategoryDrawerV2::mouseButtonPressed(const QModelIndex&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV2::mouseButtonReleased(const QModelIndex&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV2::mouseButtonMoved(const QModelIndex&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV2::mouseButtonDoubleClicked(const QModelIndex&, QMouseEvent *event)
{
    event->ignore();
}

class QCategoryDrawerV3::Private
{
public:
    Private(QCategorizedView *view)
        : view(view)
    {
    }

    ~Private()
    {
    }

    QCategorizedView *view;
};

QCategoryDrawerV3::QCategoryDrawerV3(QCategorizedView *view)
    : QCategoryDrawerV2(view)
    , d(new Private(view))
{
}

QCategoryDrawerV3::~QCategoryDrawerV3()
{
    delete d;
}

QCategorizedView *QCategoryDrawerV3::view() const
{
    return d->view;
}

void QCategoryDrawerV3::mouseButtonPressed(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV3::mouseButtonReleased(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV3::mouseMoved(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV3::mouseButtonDoubleClicked(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->ignore();
}

void QCategoryDrawerV3::mouseLeft(const QModelIndex&, const QRect&)
{
}

#include "moc_qcategorydrawer.cpp"
