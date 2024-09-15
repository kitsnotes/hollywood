// Hollywood Stage
// (C) 2024 Originull Software
// Based on Qt's QTabBar
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LGPL-3.0-only

#define ANIMATION_DURATION 250

#include "stagetasklist.h"
#include <QStyle>
#include <QPainter>
#include <QEvent>
#include <QApplication>
#include <QToolButton>
#include <QStyleOption>
#include <QStylePainter>
#include <QWheelEvent>
#include <QAbstractItemDelegate>
#include <QAccessible>
#include <QDebug>
#include <QSinglePointEvent>
#include <QSize>
#include <QToolTip>
#include <QLayout>

#include <algorithm>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

typedef qint64 Fixed64;
static inline Fixed64 toFixed(int i) { return (Fixed64)i * 256; }
static inline int fRound(Fixed64 i) {
    return (i % 256 < 128) ? i / 256 : 1 + i / 256;
}

MovableTaskList::MovableTaskList(QWidget *parent)
    : QWidget(parent)
{
}

void MovableTaskList::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

void MovableTaskList::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.setOpacity(15);
    p.drawPixmap(0, 0, m_pixmap);
}

StageTaskList::StageTaskList(QWidget* parent)
    : QWidget(parent, { })
{
    setAttribute(Qt::WA_Hover, true);
    m_leftB = new QToolButton(this);
    m_leftB->setObjectName(u"ScrollLeftButton"_s);
    m_leftB->setAutoRepeat(true);
    connect(m_leftB, &QToolButton::clicked, this, &StageTaskList::scrollTabs);
    m_leftB->hide();
    m_rightB = new QToolButton(this);
    m_rightB->setObjectName(u"ScrollRightButton"_s);
    m_rightB->setAutoRepeat(true);
    connect(m_rightB, &QToolButton::clicked,
            this, &StageTaskList::scrollTabs);
    m_rightB->hide();
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplicationPrivate::keypadNavigationEnabled()) {
        m_leftB->setFocusPolicy(Qt::NoFocus);
        m_rightB->setFocusPolicy(Qt::NoFocus);
        setFocusPolicy(Qt::NoFocus);
    } else
#endif
        setFocusPolicy(Qt::TabFocus);

    m_leftB->setAccessibleName(StageTaskList::tr("Scroll Left"));
    m_rightB->setAccessibleName(StageTaskList::tr("Scroll Right"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, nullptr, this));
    m_useScrollButtons = !style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, nullptr, this);
    setExpanding(false);
}

StageTaskList::~StageTaskList()
{
}

void StageTaskList::initBasicStyleOption(QStyleOptionToolButton *option, int tabIndex) const
{
    const int totalTabs = m_tabList.size();

    if (!option || (tabIndex < 0 || tabIndex >= totalTabs))
        return;

    const StageTaskList::Tab &tab = *m_tabList.at(tabIndex);
    option->initFrom(this);
    option->subControls = QStyle::SC_ToolButton;
    option->activeSubControls = QStyle::SC_None;
    option->state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);
    option->rect = tabRect(tabIndex);
    option->arrowType = Qt::NoArrow;
    const bool isCurrent = tabIndex == m_currentIndex;

    if (tabIndex == m_pressedIndex)
    {
        option->state |= QStyle::State_Sunken;
        option->activeSubControls |= QStyle::SC_ToolButton;
    }

    if (isCurrent)
        option->state |= QStyle::State_On;

    if (isCurrent && hasFocus())
        option->state |= QStyle::State_HasFocus;

    if (!tab.enabled)
        option->state &= ~QStyle::State_Enabled;

    if(m_hoverIndex == tabIndex)
        option->state |= QStyle::State_Raised;

    if (isActiveWindow())
        option->state |= QStyle::State_Active;
    if (!m_dragInProgress && option->rect == m_hoverRect)
        option->state |= QStyle::State_MouseOver;
    option->text = tab.text;

    option->toolButtonStyle = Qt::ToolButtonTextBesideIcon;
    if (tab.textColor.isValid())
        option->palette.setColor(foregroundRole(), tab.textColor);
    option->icon = tab.icon;
    option->iconSize = iconSize();  // Will get the default value then.
}

void StageTaskList::initStyleOption(QStyleOptionToolButton *option, int tabIndex) const
{
    initBasicStyleOption(option, tabIndex);

    QRect textRect = style()->subElementRect(QStyle::SE_ToolButtonLayoutItem, option, this);
    option->text = fontMetrics().elidedText(option->text, m_elideMode, textRect.width(),
                                            Qt::TextShowMnemonic);
}

int StageTaskList::indexAtPos(const QPoint &p) const
{
    if (tabRect(m_currentIndex).contains(p))
        return m_currentIndex;
    for (int i = 0; i < m_tabList.size(); ++i)
        if (m_tabList.at(i)->enabled && tabRect(i).contains(p))
            return i;
    return -1;
}

void StageTaskList::layoutTabs()
{
    m_layoutDirty = false;
    int last, available;
    int maxExtent;
    int tabChainIndex = 0;
    int hiddenTabs = 0;

    // TODO: left/center alignment
    Qt::Alignment tabAlignment = Qt::AlignLeft;
    QList<LayoutStruct> tabChain(m_tabList.size() + 2);

    // We put an empty item at the front and back and set its expansive attribute
    // depending on tabAlignment and expanding.
    tabChain[tabChainIndex].init();
    tabChain[tabChainIndex].expansive = (!m_expanding)
                                        && (tabAlignment != Qt::AlignLeft)
                                        && (tabAlignment != Qt::AlignJustify);
    tabChain[tabChainIndex].empty = true;
    ++tabChainIndex;

    // We now go through our list of tabs and set the minimum size and the size hint
    // This will allow us to elide text if necessary. Since we don't set
    // a maximum size, tabs will EXPAND to fill up the empty space.
    // Since tab widget is rather *ahem* strict about keeping the geometry of the
    // tab bar to its absolute minimum, this won't bleed through, but will show up
    // if you use tab bar on its own (a.k.a. not a bug, but a feature).
    // Update: if expanding is false, we DO set a maximum size to prevent the tabs
    // being wider than necessary.
    if (!m_vertical) {
        int minx = 0;
        int x = 0;
        int maxHeight = 0;
        for (int i = 0; i < m_tabList.size(); ++i) {
            const auto tab = m_tabList.at(i);
            if (!tab->visible) {
                ++hiddenTabs;
                continue;
            }
            QSize sz = tabSizeHint(i);
            tab->maxRect = QRect(x, 0, sz.width(), sz.height());
            x += sz.width();
            maxHeight = qMax(maxHeight, sz.height());
            sz = minimumTabSizeHint(i);
            tab->minRect = QRect(minx, 0, sz.width(), sz.height());
            minx += sz.width();
            tabChain[tabChainIndex].init();
            tabChain[tabChainIndex].sizeHint = tab->maxRect.width();
            tabChain[tabChainIndex].minimumSize = sz.width();
            tabChain[tabChainIndex].empty = false;
            tabChain[tabChainIndex].expansive = true;

            if (!m_expanding)
                tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;
            ++tabChainIndex;
        }

        last = minx;
        available = size().width();
        maxExtent = maxHeight;
    } else {
        int miny = 0;
        int y = 0;
        int maxWidth = size().width();
        for (int i = 0; i < m_tabList.size(); ++i) {
            auto tab = m_tabList.at(i);
            if (!tab->visible) {
                ++hiddenTabs;
                continue;
            }
            QSize sz = tabSizeHint(i);
            tab->maxRect = QRect(0, y, sz.width(), sz.height());
            y += sz.height();
            maxWidth = qMax(maxWidth, sz.width());
            sz = minimumTabSizeHint(i);
            tab->minRect = QRect(0, miny, sz.width(), sz.height());
            miny += sz.height();
            tabChain[tabChainIndex].init();
            tabChain[tabChainIndex].sizeHint = tab->maxRect.height();
            tabChain[tabChainIndex].minimumSize = sz.height();
            tabChain[tabChainIndex].empty = false;
            tabChain[tabChainIndex].expansive = true;

            // we don't expand on vertical
            /*if (!m_expanding)
                tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;*/
            ++tabChainIndex;
        }

        last = miny;
        available = size().height();
        maxExtent = maxWidth;
    }

    // Mirror our front item.
    tabChain[tabChainIndex].init();
    tabChain[tabChainIndex].expansive = (!m_expanding)
                                        && (tabAlignment != Qt::AlignRight)
                                        && (tabAlignment != Qt::AlignJustify);
    tabChain[tabChainIndex].empty = true;
    Q_ASSERT(tabChainIndex == tabChain.size() - 1 - hiddenTabs); // add an assert just to make sure.

    // Do the calculation
    geomCalc(tabChain, 0, tabChain.size(), 0, qMax(available, last), 0);

    // Use the results
    hiddenTabs = 0;
    for (int i = 0; i < m_tabList.size(); ++i) {
        auto tab = m_tabList.at(i);
        if (!tab->visible) {
            tab->rect = QRect();
            ++hiddenTabs;
            continue;
        }
        const LayoutStruct &lstruct = tabChain.at(i + 1 - hiddenTabs);
        if (!m_vertical)
            tab->rect.setRect(lstruct.pos, 0, lstruct.size, maxExtent);
        else
            tab->rect.setRect(0, lstruct.pos, maxExtent, lstruct.size);
    }

    if (m_useScrollButtons && m_tabList.size() && last > available) {
        const QRect scrollRect = normalizedScrollRect(0);

        QStyleOption opt;
        opt.initFrom(this);
        QRect scrollButtonLeftRect = style()->subElementRect(QStyle::SE_TabBarScrollLeftButton, &opt, this);
        QRect scrollButtonRightRect = style()->subElementRect(QStyle::SE_TabBarScrollRightButton, &opt, this);
        int scrollButtonWidth = style()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, &opt, this);

        // Normally SE_TabBarScrollLeftButton should have the same width as PM_TabBarScrollButtonWidth.
        // But if that is not the case, we set the actual button width to PM_TabBarScrollButtonWidth, and
        // use the extra space from SE_TabBarScrollLeftButton as margins towards the tabs.
        if (m_vertical) {
            scrollButtonLeftRect.setHeight(scrollButtonWidth);
            scrollButtonRightRect.setY(scrollButtonRightRect.bottom() + 1 - scrollButtonWidth);
            scrollButtonRightRect.setHeight(scrollButtonWidth);
            m_leftB->setArrowType(Qt::UpArrow);
            m_rightB->setArrowType(Qt::DownArrow);
        } else if (layoutDirection() == Qt::RightToLeft) {
            scrollButtonRightRect.setWidth(scrollButtonWidth);
            scrollButtonLeftRect.setX(scrollButtonLeftRect.right() + 1 - scrollButtonWidth);
            scrollButtonLeftRect.setWidth(scrollButtonWidth);
            m_leftB->setArrowType(Qt::RightArrow);
            m_rightB->setArrowType(Qt::LeftArrow);
        } else {
            scrollButtonLeftRect.setWidth(scrollButtonWidth);
            scrollButtonRightRect.setX(scrollButtonRightRect.right() + 1 - scrollButtonWidth);
            scrollButtonRightRect.setWidth(scrollButtonWidth);
            m_leftB->setArrowType(Qt::LeftArrow);
            m_rightB->setArrowType(Qt::RightArrow);
        }

        m_leftB->setGeometry(scrollButtonLeftRect);
        m_leftB->setEnabled(false);
        m_leftB->show();

        m_rightB->setGeometry(scrollButtonRightRect);
        m_rightB->setEnabled(last + scrollRect.left() > scrollRect.x() + scrollRect.width());
        m_rightB->show();
    } else {
        m_rightB->hide();
        m_leftB->hide();
    }

    layoutWidgets();
    tabLayoutChange();
}

QRect StageTaskList::normalizedScrollRect(int index)
{
    // "Normalized scroll rect" means return the free space on the tab bar
    // that doesn't overlap with scroll buttons or tear indicators, and
    // always return the rect as horizontal Qt::LeftToRight, even if the
    // tab bar itself is in a different orientation.

    // If scrollbuttons are not visible, then there's no tear either, and
    // the entire widget is the scroll rect.
    if (m_leftB->isHidden())
        return m_vertical ? rect().transposed() : rect();

    QStyleOptionToolButton opt;
    initStyleOption(&opt, m_currentIndex);
    opt.rect = rect();

    QRect scrollButtonLeftRect = style()->subElementRect(QStyle::SE_TabBarScrollLeftButton, &opt, this);
    QRect scrollButtonRightRect = style()->subElementRect(QStyle::SE_TabBarScrollRightButton, &opt, this);
    QRect tearLeftRect = style()->subElementRect(QStyle::SE_TabBarTearIndicatorLeft, &opt, this);
    QRect tearRightRect = style()->subElementRect(QStyle::SE_TabBarTearIndicatorRight, &opt, this);

    if (m_vertical) {
        int topEdge, bottomEdge;
        bool leftButtonIsOnTop = scrollButtonLeftRect.y() < height() / 2;
        bool rightButtonIsOnTop = scrollButtonRightRect.y() < height() / 2;

        if (leftButtonIsOnTop && rightButtonIsOnTop) {
            topEdge = scrollButtonRightRect.bottom() + 1;
            bottomEdge = height();
        } else if (!leftButtonIsOnTop && !rightButtonIsOnTop) {
            topEdge = 0;
            bottomEdge = scrollButtonLeftRect.top();
        } else {
            topEdge = scrollButtonLeftRect.bottom() + 1;
            bottomEdge = scrollButtonRightRect.top();
        }

        bool tearTopVisible = index != 0 && topEdge != -m_scrollOffset;
        bool tearBottomVisible = index != m_tabList.size() - 1 && bottomEdge != m_tabList.constLast()->rect.bottom() + 1 - m_scrollOffset;
        if (tearTopVisible && !tearLeftRect.isNull())
            topEdge = tearLeftRect.bottom() + 1;
        if (tearBottomVisible && !tearRightRect.isNull())
            bottomEdge = tearRightRect.top();

        return QRect(topEdge, 0, bottomEdge - topEdge, height());
    } else {
        if (layoutDirection() == Qt::RightToLeft) {
            scrollButtonLeftRect = QStyle::visualRect(Qt::RightToLeft, rect(), scrollButtonLeftRect);
            scrollButtonRightRect = QStyle::visualRect(Qt::RightToLeft, rect(), scrollButtonRightRect);
            tearLeftRect = QStyle::visualRect(Qt::RightToLeft, rect(), tearLeftRect);
            tearRightRect = QStyle::visualRect(Qt::RightToLeft, rect(), tearRightRect);
        }

        int leftEdge, rightEdge;
        bool leftButtonIsOnLeftSide = scrollButtonLeftRect.x() < width() / 2;
        bool rightButtonIsOnLeftSide = scrollButtonRightRect.x() < width() / 2;

        if (leftButtonIsOnLeftSide && rightButtonIsOnLeftSide) {
            leftEdge = scrollButtonRightRect.right() + 1;
            rightEdge = width();
        } else if (!leftButtonIsOnLeftSide && !rightButtonIsOnLeftSide) {
            leftEdge = 0;
            rightEdge = scrollButtonLeftRect.left();
        } else {
            leftEdge = scrollButtonLeftRect.right() + 1;
            rightEdge = scrollButtonRightRect.left();
        }

        bool tearLeftVisible = index != 0 && leftEdge != -m_scrollOffset;
        bool tearRightVisible = index != m_tabList.size() - 1 && rightEdge != m_tabList.constLast()->rect.right() + 1 - m_scrollOffset;
        if (tearLeftVisible && !tearLeftRect.isNull())
            leftEdge = tearLeftRect.right() + 1;
        if (tearRightVisible && !tearRightRect.isNull())
            rightEdge = tearRightRect.left();

        return QRect(leftEdge, 0, rightEdge - leftEdge, height());
    }
}

int StageTaskList::hoveredTabIndex() const
{
    if (m_dragInProgress)
        return m_currentIndex;
    if (m_hoverIndex >= 0)
        return m_hoverIndex;
    return -1;
}

void StageTaskList::makeVisible(int index)
{
    if (!validIndex(index))
        return;

    const QRect tabRect = m_tabList.at(index)->rect;
    const int oldScrollOffset = m_scrollOffset;
    const bool horiz = !m_vertical;
    const int available = horiz ? width() : height();
    const int tabStart = horiz ? tabRect.left() : tabRect.top();
    const int tabEnd = horiz ? tabRect.right() : tabRect.bottom();
    const int lastTabEnd = horiz ? m_tabList.constLast()->rect.right() : m_tabList.constLast()->rect.bottom();
    const QRect scrollRect = normalizedScrollRect(index);
    const QRect entireScrollRect = normalizedScrollRect(0); // ignore tears
    const int scrolledTabBarStart = qMax(1, scrollRect.left() + m_scrollOffset);
    const int scrolledTabBarEnd = qMin(lastTabEnd - 1, scrollRect.right() + m_scrollOffset);

    if (available >= lastTabEnd) {
        // the entire tabbar fits, reset scroll
        m_scrollOffset = 0;
    } else if (tabStart < scrolledTabBarStart) {
        // Tab is outside on the left, so scroll left.
        m_scrollOffset = tabStart - scrollRect.left();
    } else if (tabEnd > scrolledTabBarEnd) {
        // Tab is outside on the right, so scroll right.
        m_scrollOffset = qMax(0, tabEnd - scrollRect.right());
    } else if (m_scrollOffset + entireScrollRect.width() > lastTabEnd + 1) {
        // fill any free space on the right without overshooting
        m_scrollOffset = qMax(0, lastTabEnd - entireScrollRect.width() + 1);
    }

    m_leftB->setEnabled(m_scrollOffset > -scrollRect.left());
    m_rightB->setEnabled(m_scrollOffset < lastTabEnd - scrollRect.right());

    if (oldScrollOffset != m_scrollOffset) {
        update();
        layoutWidgets();
    }
}

void StageTaskList::moveTabAnim(int index, int offest)
{
    if(validIndex(index))
    {
        m_tabList.at(index)->dragOffset = offest;
        layoutTab(index); // Make buttons follow tab
        update();
    }
}

void StageTaskList::killSwitchTabTimer()
{
    m_switchTabTimer.stop();
    m_switchTabCurrentIndex = -1;
}

void StageTaskList::layoutTab(int index)
{
    Q_ASSERT(index >= 0);

    const Tab *tab = m_tabList.at(index);
    if (!(tab->leftWidget || tab->rightWidget))
        return;

    QStyleOptionToolButton opt;
    initStyleOption(&opt, index);
    if (tab->leftWidget) {
        QRect rect = style()->subElementRect(QStyle::SE_TabBarTabLeftButton, &opt, this);
        QPoint p = rect.topLeft();
        if ((index == m_pressedIndex) || m_paintWithOffsets) {
            if (m_vertical)
                p.setY(p.y() + tab->dragOffset);
            else
                p.setX(p.x() + tab->dragOffset);
        }
        tab->leftWidget->move(p);
    }
    if (tab->rightWidget) {
        QRect rect = style()->subElementRect(QStyle::SE_TabBarTabRightButton, &opt, this);
        QPoint p = rect.topLeft();
        if ((index == m_pressedIndex) || m_paintWithOffsets) {
            if (m_vertical)
                p.setY(p.y() + tab->dragOffset);
            else
                p.setX(p.x() + tab->dragOffset);
        }
        tab->rightWidget->move(p);
    }
}

void StageTaskList::layoutWidgets(int start)
{
    for (int i = start; i < count(); ++i) {
        layoutTab(i);
    }
}

void StageTaskList::scrollTabs()
{
    const bool horizontal = !m_vertical;
    const QRect scrollRect = normalizedScrollRect().translated(m_scrollOffset, 0);

    int i = -1;

    if (sender() == m_leftB) {
        for (i = m_tabList.size() - 1; i >= 0; --i) {
            int start = horizontal ? m_tabList.at(i)->rect.left() : m_tabList.at(i)->rect.top();
            if (start < scrollRect.left()) {
                makeVisible(i);
                return;
            }
        }
    } else if (sender() == m_rightB) {
        for (i = 0; i < m_tabList.size(); ++i) {
            const auto tabRect = m_tabList.at(i)->rect;
            int start = horizontal ? tabRect.left() : tabRect.top();
            int end = horizontal ? tabRect.right() : tabRect.bottom();
            if (end > scrollRect.right() && start > m_scrollOffset) {
                makeVisible(i);
                return;
            }
        }
    }
}

void StageTaskList::refresh()
{
    // be safe in case a subclass is also handling move with the tabs
    if (m_pressedIndex != -1
        && m_movable
        && m_mouseButtons == Qt::NoButton) {
        moveTabFinished(m_pressedIndex);
        if (!validIndex(m_pressedIndex))
            m_pressedIndex = -1;
    }

    if (!isVisible()) {
        m_layoutDirty = true;
    } else {
        layoutTabs();
        makeVisible(m_currentIndex);
        update();
        updateGeometry();
    }
}

int StageTaskList::addTab(const QString &text)
{
    return insertTab(-1, text);
}

int StageTaskList::addTab(const QIcon& icon, const QString &text)
{
    return insertTab(-1, icon, text);
}

int StageTaskList::insertTab(int index, const QString &text)
{
    return insertTab(index, QIcon(), text);
}

int StageTaskList::insertTab(int index, const QIcon& icon, const QString &text)
{
    if (!validIndex(index)) {
        index = m_tabList.size();
        m_tabList.append(new StageTaskList::Tab(icon, text));
    } else {
        m_tabList.insert(index, new StageTaskList::Tab(icon, text));
    }
    m_firstVisible = qMax(qMin(index, m_firstVisible), 0);
    refresh();
    if (m_tabList.size() == 1)
        setCurrentIndex(index);
    else if (index <= m_currentIndex)
        ++m_currentIndex;

    if (index <= m_lastVisible)
        ++m_lastVisible;
    else
        m_lastVisible = index;

    for (const auto tab : std::as_const(m_tabList)) {
        if (tab->lastTab >= index)
            ++tab->lastTab;
    }

    if (tabAt(m_mousePosition) == index) {
        m_hoverIndex = index;
        m_hoverRect = tabRect(index);
    }

    tabInserted(index);
    return index;
}

void StageTaskList::removeTab(int index)
{
    if (validIndex(index)) {
        auto removedTab = m_tabList.at(index);
        if (m_dragInProgress)
            moveTabFinished(m_pressedIndex);

        if (removedTab->leftWidget) {
            removedTab->leftWidget->hide();
            removedTab->leftWidget->deleteLater();
            removedTab->leftWidget = nullptr;
        }
        if (removedTab->rightWidget) {
            removedTab->rightWidget->hide();
            removedTab->rightWidget->deleteLater();
            removedTab->rightWidget = nullptr;
        }

        m_tabList.removeAt(index);
        delete removedTab;
        for (auto tab : std::as_const(m_tabList)) {
            if (tab->lastTab == index)
                tab->lastTab = -1;
            if (tab->lastTab > index)
                --tab->lastTab;
        }

        calculateFirstLastVisible(index, false, true);

        if (index == m_currentIndex) {
            // The current tab is going away, in order to make sure
            // we emit that "current has changed", we need to reset this
            // around.
            m_currentIndex = -1;
            emit currentChanged(-1);
        } else if (index < m_currentIndex) {
            setCurrentIndex(m_currentIndex - 1);
        }
        refresh();
        if (m_hoverRect.isValid()) {
            update(m_hoverRect);
            m_hoverIndex = tabAt(m_mousePosition);
            if (validIndex(m_hoverIndex)) {
                m_hoverRect = tabRect(m_hoverIndex);
                update(m_hoverRect);
            } else {
                m_hoverRect = QRect();
            }
        }
        tabRemoved(index);
    }
}

void StageTaskList::setTabData(int index, const QVariant & data)
{
    if (StageTaskList::Tab *tab = at(index))
        tab->data = data;
}

QVariant StageTaskList::tabData(int index) const
{
    if (const StageTaskList::Tab *tab = at(index))
        return tab->data;
    return QVariant();
}

QRect StageTaskList::tabRect(int index) const
{
    if (const StageTaskList::Tab *tab = at(index)) {
        if (m_layoutDirty)
            const_cast<StageTaskList*>(this)->layoutTabs();
        if (!tab->visible)
            return QRect();
        QRect r = tab->rect;
        if (m_vertical)
            r.translate(0, -m_scrollOffset);
        else
            r.translate(-m_scrollOffset, 0);
        if (!m_vertical)
            r = QStyle::visualRect(layoutDirection(), rect(), r);

        return r;
    }
    return QRect();
}

int StageTaskList::tabAt(const QPoint &position) const
{
    if (validIndex(m_currentIndex)
        && tabRect(m_currentIndex).contains(position)) {
        return m_currentIndex;
    }
    const int max = m_tabList.size();
    for (int i = 0; i < max; ++i) {
        if (tabRect(i).contains(position)) {
            return i;
        }
    }
    return -1;
}

int StageTaskList::currentIndex() const
{
    if (validIndex(m_currentIndex))
        return m_currentIndex;
    return -1;
}

void StageTaskList::setCurrentIndex(int index)
{
    if (m_dragInProgress && m_pressedIndex != -1)
        return;
    if (m_currentIndex == index)
        return;

    int oldIndex = m_currentIndex;

    if(index == -1)
    {
        m_currentIndex = -1;
        update();
        emit currentChanged(index);
        return;
    }
    if (auto tab = at(index)) {
        m_currentIndex = index;
        // If the size hint depends on whether the tab is selected (for instance a style
        // sheet rule that sets a bold font on the 'selected' tab) then we need to
        // re-layout the entire tab bar. To minimize the cost, do that only if the
        // size hint changes for the tab that becomes the current tab (the old current tab
        // will most certainly do the same). QTBUG-6905
        if (tabRect(index).size() != tabSizeHint(index))
            layoutTabs();
        update();
        if (!isVisible())
            m_layoutDirty = true;
        else
            makeVisible(index);
        if (validIndex(oldIndex)) {
            tab->lastTab = oldIndex;
            layoutTab(oldIndex);
        }
        layoutTab(index);
        if (QAccessible::isActive()) {
            if (hasFocus()) {
                QAccessibleEvent focusEvent(this, QAccessible::Focus);
                focusEvent.setChild(index);
                QAccessible::updateAccessibility(&focusEvent);
            }
            QAccessibleEvent selectionEvent(this, QAccessible::Selection);
            selectionEvent.setChild(index);
            QAccessible::updateAccessibility(&selectionEvent);
        }
        emit currentChanged(index);
    }
}

QSize StageTaskList::iconSize() const
{
    if (m_iconSize.isValid())
        return m_iconSize;
    int iconExtent = style()->pixelMetric(QStyle::PM_TabBarIconSize, nullptr, this);
    return QSize(iconExtent, iconExtent);

}

void StageTaskList::setIconSize(const QSize &size)
{
    m_iconSize = size;
    m_layoutDirty = true;
    update();
    updateGeometry();
}

int StageTaskList::count() const
{
    return m_tabList.size();
}

QSize StageTaskList::sizeHint() const
{
    if (m_layoutDirty)
        const_cast<StageTaskList*>(this)->layoutTabs();
    QRect r;
    for (const auto tab : m_tabList) {
        if (tab->visible)
            r = r.united(tab->maxRect);
    }
    return r.size();
}

QSize StageTaskList::minimumSizeHint() const
{
    if (m_layoutDirty)
        const_cast<StageTaskList*>(this)->layoutTabs();
    if (!m_useScrollButtons) {
        QRect r;
        for (const auto tab : m_tabList) {
            if (tab->visible)
                r = r.united(tab->minRect);
        }
        return r.size();
    }
    if (m_vertical)
        return QSize(sizeHint().width(), m_rightB->sizeHint().height() * 2 + 75);
    else
        return QSize(m_rightB->sizeHint().width() * 2 + 75, sizeHint().height());
}

// Compute the most-elided possible text, for minimumSizeHint
static QString computeElidedText(Qt::TextElideMode mode, const QString &text)
{
    if (text.size() <= 3)
        return text;

    // TODO: fix
    //static const auto Ellipses = "..."_L1;
    QString ret;
    switch (mode) {
    /*case Qt::ElideRight:
        ret = QStringView{text}.left(2) + Ellipses;
        break;
    case Qt::ElideMiddle:
        ret = QStringView{text}.left(1) + Ellipses + QStringView{text}.right(1);
        break;
    case Qt::ElideLeft:
        ret = Ellipses + QStringView{text}.right(2);
        break;*/
    case Qt::ElideNone:
    default:
        ret = text;
        break;
    }
    return ret;
}

QSize StageTaskList::minimumTabSizeHint(int index) const
{
    StageTaskList::Tab *tab = m_tabList.at(index);
    QString oldText = tab->text;
    tab->text = computeElidedText(m_elideMode, oldText);
    tab->measuringMinimum = true;
    QSize size = tabSizeHint(index);
    tab->text = oldText;
    tab->measuringMinimum = false;
    return size;
}

QSize StageTaskList::tabSizeHint(int index) const
{
    //Note: this must match with the computations in QCommonStylePrivate::tabLayout
    if (const StageTaskList::Tab *tab = at(index)) {
        QStyleOptionToolButton opt;
        initBasicStyleOption(&opt, index);
        opt.text = tab->text;
        QSize iconSize = tab->icon.isNull() ? QSize(0, 0) : opt.iconSize;
        int hframe = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, &opt, this);
        int vframe = 0; //style()->pixelMetric(QStyle::PM_TabBarTabVSpace, &opt, this);
        const QFontMetrics fm = fontMetrics();

        int maxWidgetHeight = 0;
        int maxWidgetWidth = 0;

        int padding = 0;

        if (!opt.icon.isNull())
            padding += 4;

        QHash<QString, QSize>::iterator it = m_textSizes.find(tab->text);
        if (it == m_textSizes.end())
            it = m_textSizes.insert(tab->text, fm.size(Qt::TextShowMnemonic, tab->text));
        const int textWidth = it.value().width();
        QSize csz;
        if (m_vertical) {
            csz = QSize( qMax(maxWidgetWidth, qMax(fm.height(), iconSize.height())) + vframe,
                        qMax(maxWidgetHeight, qMax(fm.height(), iconSize.height())) + vframe);
        } else {
            csz = QSize(textWidth + iconSize.width() + hframe + padding,
                        qMax(maxWidgetHeight, qMax(fm.height(), iconSize.height())) + vframe);
        }

        QSize retSize = style()->sizeFromContents(QStyle::CT_ToolButton, &opt, csz, this);
        return retSize;
    }
    return QSize();
}

void StageTaskList::tabInserted(int index)
{
    Q_UNUSED(index);
}

void StageTaskList::tabRemoved(int index)
{
    Q_UNUSED(index);
}

void StageTaskList::tabLayoutChange()
{
}

void StageTaskList::showEvent(QShowEvent *)
{
    if (m_layoutDirty)
        refresh();
    if (!validIndex(m_currentIndex))
        setCurrentIndex(0);
    else
        makeVisible(m_currentIndex);

}

void StageTaskList::hideEvent(QHideEvent *)
{

}

bool StageTaskList::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverMove:
    case QEvent::HoverEnter: {
        QHoverEvent *he = static_cast<QHoverEvent *>(event);
        m_mousePosition = he->position().toPoint();

        if (!m_hoverRect.contains(m_mousePosition)) {
            if (m_hoverRect.isValid())
                update(m_hoverRect);
            m_hoverIndex = tabAt(m_mousePosition);
            if (validIndex(m_hoverIndex)) {
                m_hoverRect = tabRect(m_hoverIndex);
                update(m_hoverRect);
            } else {
                m_hoverRect = QRect();
            }
        }
        return true;
    }
    case QEvent::HoverLeave: {
        m_mousePosition = {-1, -1};
        if (m_hoverRect.isValid())
            update(m_hoverRect);
        m_hoverIndex = -1;
        m_hoverRect = QRect();
        m_accumulatedAngleDelta = QPoint();
        return true;
    }
    case QEvent::ToolTip:
        if (const StageTaskList::Tab *tab = at(tabAt(static_cast<QHelpEvent*>(event)->pos()))) {
            if (!tab->toolTip.isEmpty()) {
                // TODO: fix this
                //QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tab->toolTip, this);
                return true;
            }
        }
        break;
    case QEvent::Shortcut: {
        QShortcutEvent *se = static_cast<QShortcutEvent *>(event);
        for (int i = 0; i < m_tabList.size(); ++i) {
            const StageTaskList::Tab *tab = m_tabList.at(i);
            if (tab->shortcutId == se->shortcutId()) {
                setCurrentIndex(i);
                return true;
            }
        }
    }
    break;
    case QEvent::Move:

        break;
    case QEvent::DragEnter:
        if (m_changeCurrentOnDrag)
            event->accept();
        break;
    case QEvent::DragMove:
        if (m_changeCurrentOnDrag) {
            const int tabIndex = tabAt(static_cast<QDragMoveEvent *>(event)->position().toPoint());
            if (m_switchTabCurrentIndex != tabIndex) {
                m_switchTabCurrentIndex = tabIndex;
                m_switchTabTimer.start(
                    style()->styleHint(QStyle::SH_TabBar_ChangeCurrentDelay) * 1ms, this);
            }
            event->ignore();
        }
        break;
    case QEvent::DragLeave:
    case QEvent::Drop:
        killSwitchTabTimer();
        event->ignore();
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        m_mousePosition = static_cast<QMouseEvent *>(event)->position().toPoint();
        m_mouseButtons = static_cast<QMouseEvent *>(event)->buttons();
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void StageTaskList::resizeEvent(QResizeEvent *)
{
    if (m_layoutDirty)
        updateGeometry();

    // when resizing, we want to keep the scroll offset as much as possible
    layoutTabs();

    makeVisible(m_currentIndex);
}

void StageTaskList::paintEvent(QPaintEvent *)
{
    QStyleOptionToolButton optTabBase;

    QStylePainter p(this);
    int selected = -1;
    QStyleOptionToolButton cutTabLeft;
    QStyleOptionToolButton cutTabRight;
    selected = m_currentIndex;
    if (m_dragInProgress)
        selected = m_pressedIndex;

    const QRect scrollRect = normalizedScrollRect();

    // the buttons might be semi-transparent or not fill their rect, but we don't
    // want the tab underneath to shine through, so clip the button area; QTBUG-50866
    if (m_leftB->isVisible() || m_rightB->isVisible()) {
        QStyleOption opt;
        opt.initFrom(this);
        QRegion buttonRegion;
        if (m_leftB->isVisible())
            buttonRegion |= style()->subElementRect(QStyle::SE_TabBarScrollLeftButton, &opt, this);
        if (m_rightB->isVisible())
            buttonRegion |= style()->subElementRect(QStyle::SE_TabBarScrollRightButton, &opt, this);
        if (!buttonRegion.isEmpty())
            p.setClipRegion(QRegion(rect()) - buttonRegion);
    }

    for (int i = 0; i < m_tabList.size(); ++i) {
        const auto tab = m_tabList.at(i);
        if (!tab->visible)
            continue;
        QStyleOptionToolButton tabOption;
        initStyleOption(&tabOption, i);
        if (m_paintWithOffsets && tab->dragOffset != 0) {
            if (m_vertical) {
                tabOption.rect.moveTop(tabOption.rect.y() + tab->dragOffset);
            } else {
                tabOption.rect.moveLeft(tabOption.rect.x() + tab->dragOffset);
            }
        }
        if (!(tabOption.state & QStyle::State_Enabled)) {
            tabOption.palette.setCurrentColorGroup(QPalette::Disabled);
        }

        tabOption.text = tab->text;

        // If this tab is partially obscured, make a note of it so that we can pass the information
        // along when we draw the tear.
        QRect tabRect = tab->rect;
        int tabStart = m_vertical ? tabRect.top() : tabRect.left();
        int tabEnd = m_vertical ? tabRect.bottom() : tabRect.right();
        if (tabStart < scrollRect.left() + m_scrollOffset) {
            cutTabLeft = tabOption;
        } else if (tabEnd > scrollRect.right() + m_scrollOffset) {
            cutTabRight = tabOption;
        }

        // Don't bother drawing a tab if the entire tab is outside of the visible tab bar.
        if ((!m_vertical && (tabOption.rect.right() < 0 || tabOption.rect.left() > width()))
            || (m_vertical && (tabOption.rect.bottom() < 0 || tabOption.rect.top() > height())))
            continue;

        if (i == selected)
            continue;

        tabOption.state |= QStyle::State_Active;
        p.drawComplexControl(QStyle::CC_ToolButton, tabOption);
    }

    // Draw the selected tab last to get it "on top"
    if (selected >= 0) {
        QStyleOptionToolButton tabOption;
        const auto tab = m_tabList.at(selected);
        initStyleOption(&tabOption, selected);

        if (m_paintWithOffsets && tab->dragOffset != 0) {
            // if the drag offset is != 0, a move is in progress (drag or animation)
            // => set the tab position to Moving to preserve the rect
            //tabOption.position = QStyleOptionToolButton::TabPosition::Moving;

            if (m_vertical)
                tabOption.rect.moveTop(tabOption.rect.y() + tab->dragOffset);
            else
                tabOption.rect.moveLeft(tabOption.rect.x() + tab->dragOffset);
        }

        // Calculate the rect of a moving tab
        const int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, nullptr, this);
        const QRect &movingRect = m_vertical
                                      ? tabOption.rect.adjusted(0, -taboverlap, 0, taboverlap)
                                      : tabOption.rect.adjusted(-taboverlap, 0, taboverlap, 0);


        // If a drag is in process, set the moving tab's geometry here
        // (in an animation, it is already set)
        if (m_dragInProgress)
            m_movingTab->setGeometry(movingRect);

        p.drawComplexControl(QStyle::CC_ToolButton, tabOption);
    }
}

void StageTaskList::calculateFirstLastVisible(int index, bool visible, bool remove)
{
    if (visible) {
        m_firstVisible = qMin(index, m_firstVisible);
        m_lastVisible  = qMax(index, m_lastVisible);
    } else {
        if (remove || (index == m_firstVisible)) {
            m_firstVisible = -1;
            for (int i = 0; i < m_tabList.size(); ++i) {
                if (m_tabList.at(i)->visible) {
                    m_firstVisible = i;
                    break;
                }
            }
        }
        if (remove || (index == m_lastVisible)) {
            m_lastVisible = -1;
            for (int i = m_tabList.size() - 1; i >= 0; --i) {
                if (m_tabList.at(i)->visible) {
                    m_lastVisible = i;
                    break;
                }
            }
        }
    }
}

int StageTaskList::selectNewCurrentIndexFrom(int fromIndex)
{
    int newindex = -1;
    for (int i = fromIndex; i < m_tabList.size(); ++i) {
        if (at(i)->visible && at(i)->enabled) {
            newindex = i;
            break;
        }
    }
    if (newindex < 0) {
        for (int i = fromIndex-1; i > -1; --i) {
            if (at(i)->visible && at(i)->enabled) {
                newindex = i;
                break;
            }
        }
    }

    return newindex;
}

int StageTaskList::calculateNewPosition(int from, int to, int index) const
{
    if (index == from)
        return to;

    int start = qMin(from, to);
    int end = qMax(from, to);
    if (index >= start && index <= end)
        index += (from < to) ? -1 : 1;
    return index;
}

void StageTaskList::moveTab(int from, int to)
{
    if (from == to
        || !validIndex(from)
        || !validIndex(to))
        return;

    auto &fromTab = *m_tabList.at(from);
    auto &toTab = *m_tabList.at(to);

    bool vertical = m_vertical;
    int oldPressedPosition = 0;
    if (m_pressedIndex != -1) {
        // Record the position of the pressed tab before reordering the tabs.
        oldPressedPosition = vertical ? m_tabList.at(m_pressedIndex)->rect.y()
                                      : m_tabList.at(m_pressedIndex)->rect.x();
    }

    // Update the locations of the tabs first
    int start = qMin(from, to);
    int end = qMax(from, to);
    int width = vertical ? fromTab.rect.height() : fromTab.rect.width();
    if (from < to)
        width *= -1;
    bool rtl = isRightToLeft();
    for (int i = start; i <= end; ++i) {
        if (i == from)
            continue;
        auto &tab = *m_tabList.at(i);
        if (vertical)
            tab.rect.moveTop(tab.rect.y() + width);
        else
            tab.rect.moveLeft(tab.rect.x() + width);
        int direction = -1;
        if (rtl && !vertical)
            direction *= -1;
        if (tab.dragOffset != 0)
            tab.dragOffset += (direction * width);
    }

    if (vertical) {
        if (from < to)
            fromTab.rect.moveTop(toTab.rect.bottom() + 1);
        else
            fromTab.rect.moveTop(toTab.rect.top() - width);
    } else {
        if (from < to)
            fromTab.rect.moveLeft(toTab.rect.right() + 1);
        else
            fromTab.rect.moveLeft(toTab.rect.left() - width);
    }

    // Move the actual data structures
    m_tabList.move(from, to);


    // update lastTab locations
    for (const auto tab : std::as_const(m_tabList))
        tab->lastTab = calculateNewPosition(from, to, tab->lastTab);

    // update external variables
    int previousIndex = m_currentIndex;
    m_currentIndex = calculateNewPosition(from, to, m_currentIndex);

    // If we are in the middle of a drag update the dragStartPosition
    if (m_pressedIndex != -1) {
        m_pressedIndex = calculateNewPosition(from, to, m_pressedIndex);
        const auto pressedTab = m_tabList.at(m_pressedIndex);
        int newPressedPosition = vertical ? pressedTab->rect.top() : pressedTab->rect.left();
        int diff = oldPressedPosition - newPressedPosition;
        if (isRightToLeft() && !vertical)
            diff *= -1;
        if (vertical)
            m_dragStartPosition.setY(m_dragStartPosition.y() - diff);
        else
            m_dragStartPosition.setX(m_dragStartPosition.x() - diff);
    }

    layoutWidgets(start);
    update();
    emit tabMoved(from, to);
    if (previousIndex != m_currentIndex)
        emit currentChanged(m_currentIndex);
    emit tabLayoutChange();
}

void StageTaskList::slide(int from, int to)
{
    if (from == to
        || !validIndex(from)
        || !validIndex(to))
        return;
    int preLocation = m_vertical ? tabRect(from).y() : tabRect(from).x();
    setUpdatesEnabled(false);
    moveTab(from, to);
    setUpdatesEnabled(true);
    int postLocation = m_vertical ? tabRect(to).y() : tabRect(to).x();
    int length = postLocation - preLocation;
    m_tabList.at(to)->dragOffset -= length;
    m_tabList.at(to)->startAnimation(this, ANIMATION_DURATION);
}

void StageTaskList::mousePressEvent(QMouseEvent *event)
{
    const QPoint pos = event->position().toPoint();
    const bool isEventInCornerButtons = (!m_leftB->isHidden() && m_leftB->geometry().contains(pos))
                                        || (!m_rightB->isHidden() && m_rightB->geometry().contains(pos));
    if (!isEventInCornerButtons) {
        const int index = indexAtPos(pos);
        emit tabBarClicked(index);
    }

    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }
    // Be safe!
    if (m_pressedIndex != -1 && m_movable)
        moveTabFinished(m_pressedIndex);

    m_pressedIndex = indexAtPos(event->position().toPoint());

    if (validIndex(m_pressedIndex)) {

        /*if (event->type() == QEvent::MouseButtonPress)
            setCurrentIndex(m_pressedIndex);
        else*/
            repaint(tabRect(m_pressedIndex));
        if (m_movable) {
            m_dragStartPosition = event->position().toPoint();
        }
    }
}

void StageTaskList::mouseMoveEvent(QMouseEvent *event)
{
    if (m_movable) {
        // Be safe!
        if (m_pressedIndex != -1
            && event->buttons() == Qt::NoButton)
            moveTabFinished(m_pressedIndex);

        // Start drag
        if (!m_dragInProgress && m_pressedIndex != -1) {
            if ((event->position().toPoint() - m_dragStartPosition).manhattanLength() > QApplication::startDragDistance()) {
                m_dragInProgress = true;
                setAttribute(Qt::WA_Hover, false);
                setupMovableTab();
            }
        }

        if (event->buttons() == Qt::LeftButton && m_dragInProgress && validIndex(m_pressedIndex))
        {
            int dragDistance;
            if (m_vertical) {
                dragDistance = (event->position().toPoint().y() - m_dragStartPosition.y());
            } else {
                dragDistance = (event->position().toPoint().x() - m_dragStartPosition.x());
            }
            m_tabList.at(m_pressedIndex)->dragOffset = dragDistance;

            QRect startingRect = tabRect(m_pressedIndex);
            if (m_vertical)
                startingRect.moveTop(startingRect.y() + dragDistance);
            else
                startingRect.moveLeft(startingRect.x() + dragDistance);

            int overIndex;
            if (dragDistance < 0)
                overIndex = tabAt(startingRect.topLeft());
            else
                overIndex = tabAt(startingRect.topRight());

            if (overIndex != m_pressedIndex && overIndex != -1) {
                int offset = 1;
                if (isRightToLeft() && !m_vertical)
                    offset *= -1;
                if (dragDistance < 0) {
                    dragDistance *= -1;
                    offset *= -1;
                }
                for (int i = m_pressedIndex;
                     offset > 0 ? i < overIndex : i > overIndex;
                     i += offset) {
                    QRect overIndexRect = tabRect(overIndex);
                    int needsToBeOver = (m_vertical ? overIndexRect.height() : overIndexRect.width()) / 2;
                    if (dragDistance > needsToBeOver)
                        slide(i + offset, m_pressedIndex);
                }
            }
            // Buttons needs to follow the dragged tab
            if (m_pressedIndex != -1)
                layoutTab(m_pressedIndex);

            update();
        }
    }

    if (event->buttons() != Qt::LeftButton) {
        event->ignore();
        return;
    }
}

void StageTaskList::setupMovableTab()
{
    if (!m_movingTab)
        m_movingTab = new MovableTaskList(this);

    int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, nullptr ,this);
    QRect grabRect = tabRect(m_pressedIndex);
    if (m_vertical)
        grabRect.adjust(0, -taboverlap, 0, taboverlap);
    else
        grabRect.adjust(-taboverlap, 0, taboverlap, 0);

    QPixmap grabImage(grabRect.size() * devicePixelRatio());
    grabImage.setDevicePixelRatio(devicePixelRatio());
    grabImage.fill(Qt::transparent);
    QStylePainter p(&grabImage, this);

    QStyleOptionToolButton opt;
    initStyleOption(&opt, m_pressedIndex);
    p.drawComplexControl(QStyle::CC_ToolButton, opt);


    /*QStyleOptionToolButton tab;
    initStyleOption(&tab, m_pressedIndex);
    tab.position = QStyleOptionToolButton::Moving;
    if (m_vertical)
        tab.rect.moveTopLeft(QPoint(0, taboverlap));
    else
        tab.rect.moveTopLeft(QPoint(taboverlap, 0));
    p.drawControl(QStyle::CE_TabBarTab, tab);*/
    p.end();


    m_movingTab->setPixmap(grabImage);
    m_movingTab->setGeometry(grabRect);
    m_movingTab->raise();

    // Re-arrange widget order to avoid overlaps
    if (m_leftB)
        m_leftB->raise();
    if (m_rightB)
        m_rightB->raise();
    m_movingTab->setVisible(true);
}

void StageTaskList::moveTabFinished(int index)
{
    bool cleanup = (m_pressedIndex == index) || (m_pressedIndex == -1) || !validIndex(index);
    bool allAnimationsFinished = true;
    for (const auto tab : std::as_const(m_tabList)) {
        if (tab->animation && tab->animation->state() == QAbstractAnimation::Running) {
            allAnimationsFinished = false;
            break;
        }
    }
    if (allAnimationsFinished && cleanup) {
        if (m_movingTab)
            m_movingTab->setVisible(false); // We might not get a mouse release
        for (auto tab : std::as_const(m_tabList)) {
            tab->dragOffset = 0;
        }
        if (m_pressedIndex != -1 && m_movable) {
            m_pressedIndex = -1;
            m_dragInProgress = false;
            m_dragStartPosition = QPoint();
        }
        layoutWidgets();
    } else {
        if (!validIndex(index))
            return;
        m_tabList.at(index)->dragOffset = 0;
    }
    setAttribute(Qt::WA_Hover, true);
    update();
}

void StageTaskList::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    if (m_movable && m_dragInProgress && validIndex(m_pressedIndex)) {
        int length = m_tabList.at(m_pressedIndex)->dragOffset;
        int width = m_vertical
                        ? tabRect(m_pressedIndex).height()
                        : tabRect(m_pressedIndex).width();
        int duration = qMin(ANIMATION_DURATION,
                            (qAbs(length) * ANIMATION_DURATION) / width);
        m_tabList.at(m_pressedIndex)->startAnimation(this, duration);
        m_dragInProgress = false;
        m_movingTab->setVisible(false);
        m_dragStartPosition = QPoint();
    }

    setAttribute(Qt::WA_Hover, true);

    // mouse release event might happen outside the tab, so keep the pressed index
    int oldPressedIndex = m_pressedIndex;
    int i = indexAtPos(event->position().toPoint()) == m_pressedIndex ? m_pressedIndex : -1;
    m_pressedIndex = -1;
    const bool selectOnRelease = false;
    if (selectOnRelease)
        setCurrentIndex(i);
    if (validIndex(oldPressedIndex))
        update(tabRect(oldPressedIndex));
}

void StageTaskList::mouseDoubleClickEvent(QMouseEvent *event)
{
    const QPoint pos = event->position().toPoint();
    const bool isEventInCornerButtons = (!m_leftB->isHidden() && m_leftB->geometry().contains(pos))
                                        || (!m_rightB->isHidden() && m_rightB->geometry().contains(pos));
    if (!isEventInCornerButtons)
        emit tabBarDoubleClicked(tabAt(pos));

    mousePressEvent(event);
}

void StageTaskList::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Left && event->key() != Qt::Key_Right) {
        event->ignore();
        return;
    }
    int offset = event->key() == (isRightToLeft() ? Qt::Key_Right : Qt::Key_Left) ? -1 : 1;
    setCurrentNextEnabledIndex(offset);
}

void StageTaskList::wheelEvent(QWheelEvent *event)
{
    if (style()->styleHint(QStyle::SH_TabBar_AllowWheelScrolling)) {
        const bool wheelVertical = qAbs(event->angleDelta().y()) > qAbs(event->angleDelta().x());
        const bool tabsVertical = m_vertical;
        if (event->device()->capabilities().testFlag(QInputDevice::Capability::PixelScroll)) {
            // For wheels/touch pads with pixel precision, scroll the tab bar if
            // it has the right orientation.
            int delta = 0;
            if (tabsVertical == wheelVertical)
                delta = wheelVertical ? event->pixelDelta().y() : event->pixelDelta().x();
            if (layoutDirection() == Qt::RightToLeft)
                delta = -delta;
            if (delta && validIndex(m_lastVisible)) {
                const int oldScrollOffset = m_scrollOffset;
                const QRect lastTabRect = m_tabList.at(m_lastVisible)->rect;
                const QRect scrollRect = normalizedScrollRect(m_lastVisible);
                int scrollRectExtent = scrollRect.right();
                if (!m_leftB->isVisible())
                    scrollRectExtent += tabsVertical ? m_leftB->height() : m_leftB->width();
                if (!m_rightB->isVisible())
                    scrollRectExtent += tabsVertical ? m_rightB->height() : m_rightB->width();

                const int maxScrollOffset = qMax((tabsVertical ?
                                                      lastTabRect.bottom() :
                                                      lastTabRect.right()) - scrollRectExtent, 0);
                m_scrollOffset = qBound(0, m_scrollOffset - delta, maxScrollOffset);
                m_leftB->setEnabled(m_scrollOffset > -scrollRect.left());
                m_rightB->setEnabled(maxScrollOffset > m_scrollOffset);
                if (oldScrollOffset != m_scrollOffset) {
                    event->accept();
                    update();
                    return;
                }
            }
        } else {
            m_accumulatedAngleDelta += event->angleDelta();
            const int xSteps = m_accumulatedAngleDelta.x() / QWheelEvent::DefaultDeltasPerStep;
            const int ySteps = m_accumulatedAngleDelta.y() / QWheelEvent::DefaultDeltasPerStep;
            int offset = 0;
            if (xSteps > 0 || ySteps > 0) {
                offset = -1;
                m_accumulatedAngleDelta = QPoint();
            } else if (xSteps < 0 || ySteps < 0) {
                offset = 1;
                m_accumulatedAngleDelta = QPoint();
            }
            const int oldCurrentIndex = m_currentIndex;
            setCurrentNextEnabledIndex(offset);
            if (oldCurrentIndex != m_currentIndex) {
                event->accept();
                return;
            }
        }
        QWidget::wheelEvent(event);
    }
}

void StageTaskList::setCurrentNextEnabledIndex(int offset)
{
    for (int index = m_currentIndex + offset; validIndex(index); index += offset) {
        if (m_tabList.at(index)->enabled && m_tabList.at(index)->visible) {
            setCurrentIndex(index);
            break;
        }
    }
}

void StageTaskList::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::StyleChange:
        if (!m_elideModeSetByUser)
            m_elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, nullptr, this));
        if (!m_useScrollButtonsSetByUser)
            m_useScrollButtons = !style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, nullptr, this);
        Q_FALLTHROUGH();
    case QEvent::FontChange:
        m_textSizes.clear();
        refresh();
        break;
    default:
        break;
    }

    QWidget::changeEvent(event);
}

void StageTaskList::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_switchTabTimer.timerId()) {
        m_switchTabTimer.stop();
        setCurrentIndex(m_switchTabCurrentIndex);
        m_switchTabCurrentIndex = -1;
    }
    QWidget::timerEvent(event);
}

Qt::TextElideMode StageTaskList::elideMode() const
{
    return m_elideMode;
}

void StageTaskList::setElideMode(Qt::TextElideMode mode)
{
    m_elideMode = mode;
    m_elideModeSetByUser = true;
    m_textSizes.clear();
    refresh();
}

bool StageTaskList::usesScrollButtons() const
{
    return m_useScrollButtons;
}

void StageTaskList::setUsesScrollButtons(bool useButtons)
{
    m_useScrollButtonsSetByUser = true;
    if (m_useScrollButtons == useButtons)
        return;
    m_useScrollButtons = useButtons;
    refresh();
}

bool StageTaskList::expanding() const
{
    return m_expanding;
}

void StageTaskList::setExpanding(bool enabled)
{
    if (m_expanding == enabled)
        return;
    m_expanding = enabled;
    layoutTabs();
}

bool StageTaskList::isMovable() const
{
    return m_movable;
}

void StageTaskList::setMovable(bool movable)
{
    m_movable = movable;
}

bool StageTaskList::changeCurrentOnDrag() const
{
    return m_changeCurrentOnDrag;
}

void StageTaskList::setChangeCurrentOnDrag(bool change)
{
    m_changeCurrentOnDrag = change;
    if (!change)
        killSwitchTabTimer();
}

bool StageTaskList::isVertical() const
{
    return m_vertical;
}

void StageTaskList::setVertical(bool vertical)
{
    m_vertical = vertical;
    refresh();
}

QIcon StageTaskList::tabIcon(int index) const
{
    if (const StageTaskList::Tab *tab = at(index))
        return tab->icon;
    return QIcon();
}

void StageTaskList::setTabIcon(int index, const QIcon & icon)
{
    if (StageTaskList::Tab *tab = at(index)) {
        bool simpleIconChange =
            (!icon.isNull() && !tab->icon.isNull());
        tab->icon = icon;
        if (simpleIconChange)
            update(tabRect(index));
        else
            refresh();
    }
}

QString StageTaskList::tabText(int index) const
{
    if (const StageTaskList::Tab *tab = at(index))
        return tab->text;
    return QString();
}

void StageTaskList::setTabText(int index, const QString &text)
{
    if (StageTaskList::Tab *tab = at(index)) {
        m_textSizes.remove(tab->text);
        tab->text = text;
        //tab->toolTip = text;
        /*releaseShortcut(tab->shortcutId);
        tab->shortcutId = grabShortcut(QKeySequence::mnemonic(text));
        setShortcutEnabled(tab->shortcutId, tab->enabled);*/
        refresh();
    }
}

void StageTaskList::setAccessibleTabName(int index, const QString &name)
{
    if (StageTaskList::Tab *tab = at(index)) {
        tab->accessibleName = name;
        QAccessibleEvent event(this, QAccessible::NameChanged);
        event.setChild(index);
        QAccessible::updateAccessibility(&event);
    }
}

QString StageTaskList::accessibleTabName(int index) const
{
    if (const StageTaskList::Tab *tab = at(index))
        return tab->accessibleName;
    return QString();
}

void StageTaskList::Tab::TabBarAnimation::updateCurrentValue(const QVariant &current)
{
    tl->moveTabAnim(tl->m_tabList.indexOf(tab), current.toInt());
}

void StageTaskList::Tab::TabBarAnimation::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State)
{
    if (newState == Stopped) tl->moveTabFinished(tl->m_tabList.indexOf(tab));
}

void StageTaskList::geomCalc(QVector<LayoutStruct> &chain, int start, int count,
               int pos, int space, int spacer)
{
    int cHint = 0;
    int cMin = 0;
    int sumStretch = 0;
    int sumSpacing = 0;
    int expandingCount = 0;
    bool allEmptyNonstretch = true;
    int pendingSpacing = -1;
    int spacerCount = 0;
    int i;
    for (i = start; i < start + count; i++) {
        LayoutStruct *data = &chain[i];
        data->done = false;
        cHint += data->smartSizeHint();
        cMin += data->minimumSize;
        sumStretch += data->stretch;
        if (!data->empty) {
            /*
                Using pendingSpacing, we ensure that the spacing for the last
                (non-empty) item is ignored.
            */
            if (pendingSpacing >= 0) {
                sumSpacing += pendingSpacing;
                ++spacerCount;
            }
            pendingSpacing = data->effectiveSpacer(
                spacer);
        }
        if (data->expansive)
            expandingCount++;
        allEmptyNonstretch = allEmptyNonstretch && data->empty && !data->expansive && data->stretch <= 0;
    }
    int extraspace = 0;
    if (space < cMin + sumSpacing) {
        /*
          Less space than minimumSize; take from the biggest first
        */
        int minSize = cMin + sumSpacing;
        // shrink the spacers proportionally
        if (spacer >= 0) {
            spacer = minSize > 0 ? spacer * space / minSize : 0;
            sumSpacing = spacer * spacerCount;
        }
        QVarLengthArray<int, 32> minimumSizes;
        minimumSizes.reserve(
            count);
        for (i = start; i < start + count; i++)
            minimumSizes << chain.at(i).minimumSize;
        std::sort(minimumSizes.begin(),
                  minimumSizes.end());
        int space_left = space - sumSpacing;
        int sum = 0;
        int idx = 0;
        int space_used=0;
        int current = 0;
        while (idx < count && space_used < space_left) {
            current = minimumSizes.at(idx);
            space_used = sum + current * (count - idx);
            sum += current;
            ++idx;
        }
        --idx;
        int deficit = space_used - space_left;
        int items = count - idx;
        /*
         * If we truncate all items to "current", we would get "deficit" too many pixels. Therefore, we have to remove
         * deficit/items from each item bigger than maxval. The actual value to remove is deficitPerItem + remainder/items
         * "rest" is the accumulated error from using integer arithmetic.
        */
        int deficitPerItem = deficit/items;
        int remainder = deficit % items;
        int maxval = current - deficitPerItem;
        int rest = 0;
        for (i = start; i < start + count; i++) {
            int maxv = maxval;
            rest += remainder;
            if (rest >= items) {
                maxv--;
                rest-=items;
            }
            LayoutStruct *data = &chain[i];
            data->size = qMin(data->minimumSize,
                              maxv);
            data->done = true;
        }
    } else if (space < cHint + sumSpacing) {
        /*
          Less space than smartSizeHint(), but more than minimumSize.
          Currently take space equally from each, as in Qt 2.x.
          Commented-out lines will give more space to stretchier
          items.
        */
        int n = count;
        int space_left = space - sumSpacing;
        int overdraft = cHint - space_left;
        // first give to the fixed ones:
        for (i = start; i < start + count; i++) {
            LayoutStruct *data = &chain[i];
            if (!data->done
                && data->minimumSize >= data->smartSizeHint()) {
                data->size = data->smartSizeHint();
                data->done = true;
                space_left -= data->smartSizeHint();
                // sumStretch -= data->stretch;
                n--;
            }
        }
        bool finished = n == 0;
        while (!finished) {
            finished = true;
            Fixed64 fp_over = toFixed(
                overdraft);
            Fixed64 fp_w = 0;
            for (i = start; i < start+count; i++) {
                LayoutStruct *data = &chain[i];
                if (data->done)
                    continue;
                // if (sumStretch <= 0)
                fp_w += fp_over / n;
                // else
                //    fp_w += (fp_over * data->stretch) / sumStretch;
                int w = fRound(
                    fp_w);
                data->size = data->smartSizeHint() - w;
                fp_w -= toFixed(
                    w); // give the difference to the next
                if (data->size < data->minimumSize) {
                    data->done = true;
                    data->size = data->minimumSize;
                    finished = false;
                    overdraft -= data->smartSizeHint() - data->minimumSize;
                    // sumStretch -= data->stretch;
                    n--;
                    break;
                }
            }
        }
    } else { // extra space
        int n = count;
        int space_left = space - sumSpacing;
        // first give to the fixed ones, and handle non-expansiveness
        for (i = start; i < start + count; i++) {
            LayoutStruct *data = &chain[i];
            if (!data->done
                && (data->maximumSize <= data->smartSizeHint()
                    || (!allEmptyNonstretch && data->empty &&
                        !data->expansive && data->stretch == 0))) {
                data->size = data->smartSizeHint();
                data->done = true;
                space_left -= data->size;
                sumStretch -= data->stretch;
                if (data->expansive)
                    expandingCount--;
                n--;
            }
        }
        extraspace = space_left;
        /*
          Do a trial distribution and calculate how much it is off.
          If there are more deficit pixels than surplus pixels, give
          the minimum size items what they need, and repeat.
          Otherwise give to the maximum size items, and repeat.
          Paul Olav Tvete has a wonderful mathematical proof of the
          correctness of this principle, but unfortunately this
          comment is too small to contain it.
        */
        int surplus, deficit;
        do {
            surplus = deficit = 0;
            Fixed64 fp_space = toFixed(
                space_left);
            Fixed64 fp_w = 0;
            for (i = start; i < start + count; i++) {
                LayoutStruct *data = &chain[i];
                if (data->done)
                    continue;
                extraspace = 0;
                if (sumStretch > 0) {
                    fp_w += (fp_space * data->stretch) / sumStretch;
                } else if (expandingCount > 0) {
                    fp_w += (fp_space * (data->expansive ? 1 : 0)) / expandingCount;
                } else {
                    fp_w += fp_space * 1 / n;
                }
                int w = fRound(
                    fp_w);
                data->size = w;
                fp_w -= toFixed(
                    w); // give the difference to the next
                if (w < data->smartSizeHint()) {
                    deficit +=  data->smartSizeHint() - w;
                } else if (w > data->maximumSize) {
                    surplus += w - data->maximumSize;
                }
            }
            if (deficit > 0 && surplus <= deficit) {
                // give to the ones that have too little
                for (i = start; i < start+count; i++) {
                    LayoutStruct *data = &chain[i];
                    if (!data->done && data->size < data->smartSizeHint()) {
                        data->size = data->smartSizeHint();
                        data->done = true;
                        space_left -= data->smartSizeHint();
                        sumStretch -= data->stretch;
                        if (data->expansive)
                            expandingCount--;
                        n--;
                    }
                }
            }
            if (surplus > 0 && surplus >= deficit) {
                // take from the ones that have too much
                for (i = start; i < start + count; i++) {
                    LayoutStruct *data = &chain[i];
                    if (!data->done && data->size > data->maximumSize) {
                        data->size = data->maximumSize;
                        data->done = true;
                        space_left -= data->maximumSize;
                        sumStretch -= data->stretch;
                        if (data->expansive)
                            expandingCount--;
                        n--;
                    }
                }
            }
        } while (n > 0 && surplus != deficit);
        if (n == 0)
            extraspace = space_left;
    }
    /*
      As a last resort, we distribute the unwanted space equally
      among the spacers (counting the start and end of the chain). We
      could, but don't, attempt a sub-pixel allocation of the extra
      space.
    */
    int extra = extraspace / (spacerCount + 2);
    int p = pos + extra;
    for (i = start; i < start+count; i++) {
        LayoutStruct *data = &chain[i];
        data->pos = p;
        p += data->size;
        if (!data->empty)
            p += data->effectiveSpacer(
                     spacer) + extra;
    }

}
