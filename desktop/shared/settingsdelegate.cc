#include "settingsdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QTextLine>

SettingsSidebarDelegate::SettingsSidebarDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
    , m_iconSize(QSize(32,32))
    , m_itemSize(QSize(64,64))
//    , m_margins(QSize(1, 1))
{}

void SettingsSidebarDelegate::drawText(QPainter *painter, QStyleOptionViewItem &opt, QRectF &textRect) const
{
    QTextLayout layout(opt.text, opt.font);
    QTextOption textOption;
    textOption.setAlignment(opt.displayAlignment);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    // FIXME:     textOption.setTextDirection(opt.direction); ?
    if(opt.text.isRightToLeft())
        textOption.setTextDirection(Qt::RightToLeft);
    else
        textOption.setTextDirection(Qt::LeftToRight);

    layout.setTextOption(textOption);
    qreal height = 0;
    qreal width = 0;
    int visibleLines = 0;
    layout.beginLayout();
    QString elidedText;
    textRect.adjust(2, 2, -2, -2); // a 2-px margin is considered at FolderView::updateGridSize()
    for(;;) {
        QTextLine line = layout.createLine();
        if(!line.isValid()) {
            break;
        }
        line.setLineWidth(textRect.width());
        height += opt.fontMetrics.leading();
        line.setPosition(QPointF(0, height));
        if((height + line.height() + textRect.y()) > textRect.bottom()) {
            // if part of this line falls outside the textRect, ignore it and quit.
            QTextLine lastLine = layout.lineAt(visibleLines - 1);
            elidedText = opt.text.mid(lastLine.textStart());
            elidedText = opt.fontMetrics.elidedText(elidedText, opt.textElideMode, textRect.width());
            if(visibleLines == 1) { // this is the only visible line
                width = textRect.width();
            }
            break;
        }
        height += line.height();
        width = qMax(width, line.naturalTextWidth());
        ++ visibleLines;
    }
    layout.endLayout();

    width = qMax(width, static_cast<qreal>(opt.fontMetrics.horizontalAdvance(elidedText)));

    // draw background for selected item
    QRectF boundRect = layout.boundingRect();
    //qDebug() << "bound rect: " << boundRect << "width: " << width;
    boundRect.setWidth(width);
    boundRect.setHeight(height);
    boundRect.moveTo(textRect.x() + (textRect.width() - width) / 2, textRect.y());

    QRectF selRect = boundRect.adjusted(-2, -2, 2, 2);

    if(!painter) { // no painter, calculate the bounding rect only
        textRect = selRect;
        return;
    }

    // Respect the active and inactive palettes (some styles can use different colors for them).
    // Also, take into account a probable disabled palette.
    QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
                                  ? (opt.state & QStyle::State_Active)
                                        ? QPalette::Active
                                        : QPalette::Inactive
                                  : QPalette::Disabled;
    if(opt.state & QStyle::State_Selected) {
        if(!opt.widget) {
            painter->fillRect(selRect, opt.palette.highlight());
        }
        painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
    }
    else {
        painter->setPen(opt.palette.color(cg, QPalette::Text));
    }

    if(opt.state & QStyle::State_Selected || opt.state & QStyle::State_MouseOver) {
        if(const QWidget* widget = opt.widget) {  // let the style engine do it
            QStyle* style = widget->style() ? widget->style() : qApp->style();
            QStyleOptionViewItem o(opt);
            o.text = QString();
            o.rect = selRect.toAlignedRect().intersected(opt.rect); // due to clipping and rounding, we might lose 1px
            o.showDecorationSelected = true;
            style->drawPrimitive(QStyle::PE_PanelItemViewItem, &o, painter, widget);
        }
    }

    // draw text
    for(int i = 0; i < visibleLines; ++i) {
        QTextLine line = layout.lineAt(i);
        if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
            QPointF pos(boundRect.x() + line.position().x(), boundRect.y() + line.y() + line.ascent());
            painter->drawText(pos, elidedText);
        }
        else {
            line.draw(painter, textRect.topLeft());
        }
    }

    if(opt.state & QStyle::State_HasFocus) {
        // draw focus rect
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(opt);
        o.rect = selRect.toRect(); // subElementRect(SE_ItemViewItemFocusRect, vopt, widget);
        o.state |= QStyle::State_KeyboardFocusChange;
        o.state |= QStyle::State_Item;
        QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
                                      ? QPalette::Normal : QPalette::Disabled;
        o.backgroundColor = opt.palette.color(cg, (opt.state & QStyle::State_Selected)
                                                      ? QPalette::Highlight : QPalette::Window);
        if(const QWidget* widget = opt.widget) {
            QStyle* style = widget->style() ? widget->style() : qApp->style();
            style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
        }
    }
}

QSize SettingsSidebarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    // no further processing if the size is specified by the data model
    if(value.isValid())
        return qvariant_cast<QSize>(value);


    if(option.decorationPosition == QStyleOptionViewItem::Top ||
        option.decorationPosition == QStyleOptionViewItem::Bottom) {
        // we handle vertical layout just by returning our item size
        return m_itemSize;
    }

    // The default size hint of the horizontal layout isn't reliable
    // because Qt calculates the row size based on the real icon size,
    // which may not be equal to the requested icon size on various occasions.
    // So, we do as in QStyledItemDelegate::sizeHint() but use the requested size.
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.decorationSize = option.decorationSize; // requested by the view
    const QWidget* widget = option.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), widget);
}

void SettingsSidebarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // vertical layout (icon mode, thumbnail mode)
    if(option.decorationPosition == QStyleOptionViewItem::Top ||
        option.decorationPosition == QStyleOptionViewItem::Bottom) {
        painter->save();
        painter->setClipRect(option.rect);

        opt.decorationAlignment = Qt::AlignHCenter | Qt::AlignTop;
        opt.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

        // draw the icon
        QIcon::Mode iconMode = QIcon::Normal; //iconModeFromState(opt.state & ~QStyle::State_Selected);
        QPoint iconPos(opt.rect.x() + (opt.rect.width() - option.decorationSize.width()) / 2, opt.rect.y()
                                                                                                  /*+ p->m_margins.height()*/);
        QRect iconRect(iconPos, option.decorationSize);
        opt.icon.paint(painter, iconRect, Qt::AlignCenter, iconMode);

        // draw the text
        QSize drawAreaSize = itemSize(); //p->m_itemSize - 2 * p->m_margins;
        // The text rect dimensions should be exactly as they were in sizeHint()
        QRectF textRect(opt.rect.x() + (opt.rect.width() - drawAreaSize.width()) / 2,
                        opt.rect.y() /*+ p->m_margins.height()*/ + option.decorationSize.height(),
                        drawAreaSize.width(),
                        drawAreaSize.height() - option.decorationSize.height());
        drawText(painter, opt, textRect);
        painter->restore();
    }
    else
    {
        // horizontal layout (list view)

        const QWidget* widget = opt.widget;
        QStyle* style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

        // draw some emblems for the item if needed
        QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
        iconRect.setSize(option.decorationSize / 2);
/*        if(symlink)
            p->m_symlinkIcon.paint(painter, iconRect, Qt::AlignCenter, iconMode);*/

    }
}

QIcon::Mode SettingsSidebarDelegate::iconModeFromState(QStyle::State state)
{
    if(state & QStyle::State_Enabled)
        return (state & QStyle::State_Selected) ? QIcon::Selected : QIcon::Normal;

    return QIcon::Disabled;
}
