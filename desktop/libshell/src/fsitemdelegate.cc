#include "fsitemdelegate.h"
#include "fsitemdelegate_p.h"
#include "fileinfo.h"

#include <QSharedPointer>

#include <QPainter>
#include <QWidget>
#include <QApplication>
#include <QAbstractItemView>
#include <QTextLayout>
#include <QTextOption>
#include <QStyledItemDelegate>
#include <QSvgRenderer>

LSFSItemDelegatePrivate::LSFSItemDelegatePrivate(LSFSItemDelegate *parent)
    : d(parent)
    , m_symlinkIcon(QIcon(":/libshell/SymlinkOverlay"))
    , m_iconSize(QSize(48,48))
    , m_itemSize(QSize(96,96))
    , m_margins(QSize(1, 1))
    , m_shadowHidden(false)

{}

LSFSItemDelegate::LSFSItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
    , p(new LSFSItemDelegatePrivate(this)){}

LSFSItemDelegate::~LSFSItemDelegate()
{
    delete p;
}

void LSFSItemDelegate::setItemSize(QSize size) {
    p->m_itemSize = size;
}

QSize LSFSItemDelegate::itemSize() const {
    return p->m_itemSize;
}

void LSFSItemDelegate::setIconSize(QSize size) {
    p->m_iconSize = size;
}

QSize LSFSItemDelegate::iconSize() const {
    return p->m_iconSize;
}

bool LSFSItemDelegate::shadowHidden() const { return p->m_shadowHidden; }

void LSFSItemDelegate::setShadowHidden(bool hide) { p->m_shadowHidden = hide; }

void LSFSItemDelegate::setShadowColor(const QColor &color)
{
    p->m_shadowColor = color;
}

void LSFSItemDelegate::drawText(QPainter *painter, QStyleOptionViewItem &opt, QRectF &textRect) const
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

    // draw shadow for text if the item is not selected and a shadow color is set
    if(!(opt.state & QStyle::State_Selected) && p->m_shadowColor.isValid()) {
        QPen prevPen = painter->pen();
        painter->setPen(QPen(p->m_shadowColor));
        for(int i = 0; i < visibleLines; ++i) {
            QTextLine line = layout.lineAt(i);
            if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
                QPointF pos(boundRect.x() + line.position().x() + 1, boundRect.y() + line.y() + line.ascent() + 1);
                painter->drawText(pos, elidedText);
            }
            else {
                line.draw(painter, textRect.topLeft() + QPointF(1, 1));
            }
        }
        painter->setPen(prevPen);
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


QIcon::Mode LSFSItemDelegate::iconModeFromState(QStyle::State state)
{
    if(state & QStyle::State_Enabled)
        return (state & QStyle::State_Selected) ? QIcon::Selected : QIcon::Normal;

    return QIcon::Disabled;
}

QSize LSFSItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);
    // no further processing if the size is specified by the data model
    if(value.isValid())
        return qvariant_cast<QSize>(value);


    if(option.decorationPosition == QStyleOptionViewItem::Top ||
            option.decorationPosition == QStyleOptionViewItem::Bottom) {
        // we handle vertical layout just by returning our item size
        return p->m_itemSize;
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

void LSFSItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    // get file info for the item
    auto file = index.data(Qt::UserRole + 4).value<QSharedPointer<LSExtendedFileInfo>>();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // distinguish the hidden items visually by making their texts italic
    bool shadowIcon(false);
    if(file && file->isHidden()) {
        QFont f(opt.font);
        f.setItalic(true);
        opt.font = f;
        shadowIcon = p->m_shadowHidden;
    }

    bool isSymlink = file && file->isSymLink();
    if(file && file->isSymLink()) {
        QFont f(opt.font);
        f.setItalic(true);
        opt.font = f;
    }
    // TODO: fix this
    bool isCut = false;
    //bool isCut = index.data(FolderModel::FileIsCutRole).toBool();

    // vertical layout (icon mode, thumbnail mode)
    if(option.decorationPosition == QStyleOptionViewItem::Top ||
            option.decorationPosition == QStyleOptionViewItem::Bottom) {
        painter->save();
        painter->setClipRect(option.rect);

        opt.decorationAlignment = Qt::AlignHCenter | Qt::AlignTop;
        opt.displayAlignment = Qt::AlignTop | Qt::AlignHCenter;

        // draw the icon
        QIcon::Mode iconMode = shadowIcon ? QIcon::Disabled
                                          // in the icon and thumbnail modes, we select text, not icon
                                          : iconModeFromState(opt.state & ~QStyle::State_Selected);
        QPoint iconPos(opt.rect.x() + (opt.rect.width() - option.decorationSize.width()) / 2, opt.rect.y() + p->m_margins.height());
        QRect iconRect(iconPos, option.decorationSize);
        if(isCut) {
            painter->save();
            painter->setOpacity(0.45);
        }
        opt.icon.paint(painter, iconRect, Qt::AlignCenter, iconMode);
        if(isCut) {
            painter->restore();
        }

        // draw some emblems for the item if needed
        if(isSymlink) {
            // draw the emblem for symlinks
            auto sml = iconRect;
            sml.setSize(option.decorationSize / 3);
            sml.adjust(0, iconRect.height() - sml.height(), 0, iconRect.height() - sml.height());
            //sml.setTop(iconRect.bottom() - sml.height());
            p->m_symlinkIcon.paint(painter, sml);
        }

        /* if(file && file->canUnmount()) {
            // emblem for mounted mountable files
            mountedIcon_.paint(painter, iconRect.translated(option.decorationSize.width() / 2, 0), Qt::AlignCenter, iconMode);
        } */

        // draw other emblems if there's any
        /* if(!emblems.empty()) {
            // FIXME: we only support one emblem now
            QIcon emblem = emblems.front()->qicon();
            emblem.paint(painter, iconRect.translated(option.decorationSize.width() / 2, option.decorationSize.height() / 2), Qt::AlignCenter, iconMode);
        }*/

        // Draw select/deselect icons outside the main icon but near its top left corner,
        // with its 1/3 size and only if the icon size isn't smaller than 48 px
        // (otherwise, the user could not click on them easily).
        const QAbstractItemView* iv = qobject_cast<const QAbstractItemView*>(opt.widget);
        if(iv != nullptr
           // only for the extended and multiple selection modes
           && (iv->selectionMode() == QAbstractItemView::ExtendedSelection
               || iv->selectionMode() == QAbstractItemView::MultiSelection)
           && option.decorationSize.width() >= 48 && (opt.state & QStyle::State_MouseOver)) {
            int s = option.decorationSize.width() / 3;
            bool cursorOnSelectionCorner = false;
            iconPos = QPoint(qMax(opt.rect.x(), iconPos.x() - s),
                             qMax(opt.rect.y(), iconPos.y() - s));
            QPoint curPos = iv->viewport()->mapFromGlobal(QCursor::pos());
            if(curPos.x() >= iconPos.x() && curPos.x() <= iconPos.x() + s
               && curPos.y() >= iconPos.y() && curPos.y() <= iconPos.y() + s) {
                cursorOnSelectionCorner = true;
            }
            if(!cursorOnSelectionCorner) { // make it translucent when not under the cursor
                painter->save();
                painter->setOpacity(0.6);
            }
            iconRect = QRect(iconPos, QSize(s, s));
            /* if(opt.state & QStyle::State_Selected) {
                removeIcon_.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal);
            }
            else {
                addIcon_.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal);
            }*/
            if(!cursorOnSelectionCorner) {
                painter->restore();
            }
        }

        // draw the text
        QSize drawAreaSize = p->m_itemSize - 2 * p->m_margins;
        // The text rect dimensions should be exactly as they were in sizeHint()
        QRectF textRect(opt.rect.x() + (opt.rect.width() - drawAreaSize.width()) / 2,
                        opt.rect.y() + p->m_margins.height() + option.decorationSize.height(),
                        drawAreaSize.width(),
                        drawAreaSize.height() - option.decorationSize.height());
        drawText(painter, opt, textRect);
        painter->restore();
    }
    else
    {
        // horizontal layout (list view)

        // Let the style engine do the painting but take care of shadowed and cut icons.
        // NOTE: The shadowing can also be done directly.
        // WARNING: QStyledItemDelegate shouldn't be used for painting because it resets the icon.
        // FIXME: For better text alignment, here we could have increased the icon width
        // when it's smaller than the requested size.

        QIcon::Mode iconMode = shadowIcon ? QIcon::Disabled : iconModeFromState(opt.state);

        if(!opt.icon.isNull()) {
            if(shadowIcon) {
                QPixmap pixmap = opt.icon.pixmap(option.decorationSize, iconMode);
                opt.icon = QIcon(pixmap);
            }

            if(isCut) {
                QPixmap pixmap = opt.icon.pixmap(option.decorationSize, iconMode);
                QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
                img.fill(Qt::transparent);
                QPainter p(&img);
                p.setOpacity(0.45);
                p.drawPixmap(0, 0, pixmap);
                p.end();
                opt.icon = QIcon(QPixmap::fromImage(img));
            }
        }

        const QWidget* widget = opt.widget;
        QStyle* style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

        // restore the original icon to not make it translucent more than once
        if(isCut && !opt.icon.isNull())
            opt.icon = option.icon;

        // draw some emblems for the item if needed
        QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget);
        iconRect.setSize(option.decorationSize / 2);
        if(isSymlink)
            p->m_symlinkIcon.paint(painter, iconRect, Qt::AlignCenter, iconMode);
/*
        if(file && file->canUnmount()) {
            mountedIcon_.paint(painter, iconRect.translated(option.decorationSize.width() / 2, 0), Qt::AlignCenter, iconMode);
        }*/
        /* if(!emblems.empty()) {
            // FIXME: we only support one emblem now
            QIcon emblem = emblems.front()->qicon();
            emblem.paint(painter, iconRect.translated(option.decorationSize.width() / 2, option.decorationSize.height() / 2), Qt::AlignCenter, iconMode);
        } */
    }
}

