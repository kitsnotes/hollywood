#ifndef LSFSITEMDELEGATE_H
#define LSFSITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class LSFSItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    explicit LSFSItemDelegate(QObject* parent);
    inline void setItemSize(QSize size) {
        m_itemSize = size;
    }

    inline QSize itemSize() const {
        return m_itemSize;
    }

    inline void setIconSize(QSize size) {
        m_iconSize = size;
    }

    inline QSize iconSize() const {
        return m_iconSize;
    }
    inline bool shadowHidden() const { return m_shadowHidden; }
    inline void setShadowHidden(bool hide) { m_shadowHidden = hide; }
    void setShadowColor(const QColor &color);
private:
    void drawText(QPainter* painter, QStyleOptionViewItem& opt, QRectF& textRect) const;
    static QIcon::Mode iconModeFromState(QStyle::State state);
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QIcon m_symlinkIcon;
    QSize m_iconSize;
    QSize m_itemSize;
    QSize m_margins;
    QColor m_shadowColor;
    bool m_shadowHidden;
    mutable bool m_hasEditor;
};

#endif // LSFSITEMDELEGATE_H
