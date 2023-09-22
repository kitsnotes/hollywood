#ifndef SETTINGSSIDEBARDELEGATE_H
#define SETTINGSSIDEBARDELEGATE_H

#include <QStyledItemDelegate>
class SettingsSidebarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    explicit SettingsSidebarDelegate(QObject* parent);
    ~SettingsSidebarDelegate() = default;
    void setItemSize(QSize size) { m_itemSize = size; }
    QSize itemSize() const { return m_itemSize;}
    void setIconSize(QSize size) { m_iconSize = size;}
    QSize iconSize() const { return m_iconSize; }
private:
    void drawText(QPainter* painter, QStyleOptionViewItem& opt, QRectF& textRect) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QIcon::Mode iconModeFromState(QStyle::State state);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
    QSize m_iconSize;
    QSize m_itemSize;
};

#endif // SETTINGSSIDEBARDELEGATE_H
