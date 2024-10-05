#pragma once

#include <QStyledItemDelegate>
#include <QObject>

class LSFSItemDelegatePrivate;
class LSFSItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    explicit LSFSItemDelegate(QObject* parent);
    ~LSFSItemDelegate();
    void setItemSize(QSize size);
    QSize itemSize() const;
    void setIconSize(QSize size);
    QSize iconSize() const;
    bool shadowHidden() const;
    void setShadowHidden(bool hide);
    void setShadowColor(const QColor &color);
private:
    void drawText(QPainter* painter, QStyleOptionViewItem& opt, QRectF& textRect) const;
    static QIcon::Mode iconModeFromState(QStyle::State state);
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    LSFSItemDelegatePrivate *p;
};
