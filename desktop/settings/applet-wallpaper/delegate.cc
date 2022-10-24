#include "delegate.h"

#include <QStyleOptionViewItem>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QWidget>
#include <QApplication>
#include <QAbstractItemView>
#include <QTextLayout>
#include <QTextOption>

ViewDelegate::ViewDelegate(QObject *parent)
    : QItemDelegate{parent}
{
}

void ViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(option.state == QStyle::State_Selected)
    {
        // draw our border
    }
}
