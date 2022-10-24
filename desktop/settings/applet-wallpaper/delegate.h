#ifndef VIEWDELEGATE_H
#define VIEWDELEGATE_H

#include <QItemDelegate>
#include <QObject>

class ViewDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ViewDelegate(QObject *parent = nullptr);
    inline void setItemSize(QSize size) {
        m_itemSize = size;
    }

    inline QSize itemSize() const {
        return m_itemSize;
    }

private:
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

#endif // VIEWDELEGATE_H
