// Hollywood Stage
// (C) 2024 Originull Software
// Based on Qt's QTabBar
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QWidget>
#include <QList>
#include <QVariantAnimation>
#include <QBasicTimer>
#include <QToolButton>
#include <QTabBar>
#include <QStyleOption>
#include <QLayoutItem>

class MovableTaskList : public QWidget
{
public:
    explicit MovableTaskList(QWidget *parent = nullptr);
    void setPixmap(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    QPixmap m_pixmap;
};

struct LayoutStruct
{
    inline void init(int stretchFactor = 0, int minSize = 0) {
        stretch = stretchFactor;
        minimumSize = sizeHint = minSize;
        maximumSize = QLAYOUTSIZE_MAX;
        expansive = false;
        empty = true;
        spacing = 0;
    }

    int smartSizeHint() {
        return (stretch > 0) ? minimumSize : sizeHint;
    }
    int effectiveSpacer(int uniformSpacer) const {
        Q_ASSERT(uniformSpacer >= 0 || spacing >= 0);
        return (uniformSpacer >= 0) ? uniformSpacer : spacing;
    }

    // parameters
    int stretch;
    int sizeHint;
    int maximumSize;
    int minimumSize;
    bool expansive;
    bool empty;
    int spacing;

    // temporary storage
    bool done;

    // result
    int pos;
    int size;
};

class StageTaskList : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
    Q_PROPERTY(Qt::TextElideMode elideMode READ elideMode WRITE setElideMode)
    Q_PROPERTY(bool usesScrollButtons READ usesScrollButtons WRITE setUsesScrollButtons)
    Q_PROPERTY(bool expanding READ expanding WRITE setExpanding)
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)
    Q_PROPERTY(bool changeCurrentOnDrag READ changeCurrentOnDrag WRITE setChangeCurrentOnDrag)
    Q_PROPERTY(bool vertical READ isVertical WRITE setVertical)

public:
    explicit StageTaskList(QWidget *parent = nullptr);
    ~StageTaskList();

    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon&icon, const QString &text);

    void removeTab(int index);
    void moveTab(int from, int to);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode mode);

    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;

    void setTabData(int index, const QVariant &data);
    QVariant tabData(int index) const;

    QRect tabRect(int index) const;
    int tabAt(const QPoint &pos) const;

    int currentIndex() const;
    int count() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QSize iconSize() const;
    void setIconSize(const QSize &size);

    bool usesScrollButtons() const;
    void setUsesScrollButtons(bool useButtons);

    bool expanding() const;
    void setExpanding(bool enabled);

    bool isMovable() const;
    void setMovable(bool movable);

    bool changeCurrentOnDrag() const;
    void setChangeCurrentOnDrag(bool change);

    bool isVertical() const;
    void setVertical(bool vertical);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

    QString accessibleTabName(int index) const;
    void setAccessibleTabName(int index, const QString &name);

    void moveTabAnim(int index, int offest);

public Q_SLOTS:
    void setCurrentIndex(int index);

Q_SIGNALS:
    void currentChanged(int index);
    void tabCloseRequested(int index);
    void tabMoved(int from, int to);
    void tabBarClicked(int index);
    void tabBarDoubleClicked(int index);

protected:
    virtual QSize tabSizeHint(int index) const;
    virtual QSize minimumTabSizeHint(int index) const;
    virtual void tabInserted(int index);
    virtual void tabRemoved(int index);
    virtual void tabLayoutChange();

    bool event(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *) override;
    void changeEvent(QEvent *) override;
    void timerEvent(QTimerEvent *event) override;
    void initStyleOption(QStyleOptionToolButton *option, int tabIndex) const;
private slots:
    void scrollTabs();
private:
    QRect m_hoverRect;
    QPoint m_dragStartPosition;
    QPoint m_mousePosition = {-1, -1};
    QPoint m_accumulatedAngleDelta;
    QToolButton* m_rightB = nullptr; // right or bottom
    QToolButton* m_leftB = nullptr; // left or top

    QSize m_iconSize;
    MovableTaskList *m_movingTab = nullptr;
    int m_hoverIndex = -1;
    int m_switchTabCurrentIndex = -1;
    QBasicTimer m_switchTabTimer;
    Qt::TextElideMode m_elideMode = Qt::ElideNone;
    Qt::MouseButtons m_mouseButtons = Qt::NoButton;

    int m_currentIndex = -1;
    int m_pressedIndex = -1;
    int m_firstVisible = 0;
    int m_lastVisible = -1;
    int m_scrollOffset = 0;

    bool m_layoutDirty = true;
    bool m_elideModeSetByUser = false;
    bool m_useScrollButtons = false;
    bool m_useScrollButtonsSetByUser : 1;
    bool m_expanding : 1;
    bool m_paintWithOffsets = true;
    bool m_movable = true;
    bool m_dragInProgress = false;
    bool m_changeCurrentOnDrag = true;

    bool m_vertical = false;
struct Tab {
        inline Tab(const QIcon &ico, const QString &txt)
        : text(txt), icon(ico), enabled(true), visible(true), measuringMinimum(false)
            {}
        Q_DISABLE_COPY_MOVE(Tab);

        QString text;
        QString toolTip;
        QString accessibleName;
        QIcon icon;
        QRect rect;
        QRect minRect;
        QRect maxRect;

        QColor textColor;
        QVariant data;
        QWidget *leftWidget = nullptr;
        QWidget *rightWidget = nullptr;
        int shortcutId = 0;
        int lastTab = -1;
        int dragOffset = 0;
        uint enabled : 1;
        uint visible : 1;
        uint measuringMinimum : 1;

        struct TabBarAnimation : public QVariantAnimation {
            TabBarAnimation(Tab *t, StageTaskList *_tl) : tab(t), tl(_tl)
            { setEasingCurve(QEasingCurve::InOutQuad); }

            void updateCurrentValue(const QVariant &current) override;

            void updateState(State newState, State) override;
        private:
            //these are needed for the callbacks
            Tab *tab;
            StageTaskList *tl;
        };
        std::unique_ptr<TabBarAnimation> animation;

        void startAnimation(StageTaskList *tl, int duration) {
            if (!tl->isAnimated()) {
                tl->moveTabFinished(tl->m_tabList.indexOf(this));
                return;
            }
            if (!animation)
                animation = std::make_unique<TabBarAnimation>(this, tl);
            animation->setStartValue(dragOffset);
            animation->setEndValue(0);
            animation->setDuration(duration);
            animation->start();
        }
    };


    QList<Tab*> m_tabList;
    mutable QHash<QString, QSize> m_textSizes;

    void calculateFirstLastVisible(int index, bool visible, bool remove);
    int selectNewCurrentIndexFrom(int currentIndex);
    int calculateNewPosition(int from, int to, int index) const;
    void slide(int from, int to);

    inline Tab *at(int index) { return m_tabList.value(index, nullptr); }
    inline const Tab *at(int index) const { return m_tabList.value(index, nullptr); }

    int indexAtPos(const QPoint &p) const;

    inline bool isAnimated() const { return true; }
    inline bool validIndex(int index) const { return index >= 0 && index < m_tabList.size(); }
    void setCurrentNextEnabledIndex(int offset);

    void moveTabFinished(int index);

    void refresh();
    void layoutTabs();
    void layoutWidgets(int start = 0);
    void layoutTab(int index);
    void setupMovableTab();
    QRect normalizedScrollRect(int index = -1);
    int hoveredTabIndex() const;
    void initBasicStyleOption(QStyleOptionToolButton *option, int tabIndex) const;
    void makeVisible(int index);

    void killSwitchTabTimer();
    void geomCalc(QVector<LayoutStruct> &chain, int start, int count,
                  int pos, int space, int spacer);
};
