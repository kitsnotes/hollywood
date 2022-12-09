/*
 * Copyright (C) 2016  Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "locationbar.h"
#include "private/locationbar_p.h"
#include "private/locationbarbtn.h"
#include <QToolButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QDebug>


LSLocationBarPrivate::LSLocationBarPrivate(LSLocationBar *parent)
    : d(parent)
    , m_tempPathEdit(nullptr)
    , m_toggledBtn(nullptr)
{

}


LSLocationBar::LSLocationBar(QWidget* parent)
   : QWidget(parent)
   , p(new LSLocationBarPrivate(this))
{

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);
    bool rtl(layoutDirection() == Qt::RightToLeft);

    // the arrow button used to scroll to start of the path
    p->m_scrollToStart = new QToolButton(this);
    p->m_scrollToStart->setArrowType(rtl ? Qt::RightArrow : Qt::LeftArrow);
    p->m_scrollToStart->setAutoRepeat(true);
    p->m_scrollToStart->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    connect(p->m_scrollToStart, &QToolButton::clicked, this, &LSLocationBar::onScrollButtonClicked);
    topLayout->addWidget(p->m_scrollToStart);

    // there might be too many buttons when the path is long, so make it scrollable.
    p->m_scrollArea = new QScrollArea(this);
    p->m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    p->m_scrollArea->setFrameShape(QFrame::NoFrame);
    p->m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->m_scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    p->m_scrollArea->verticalScrollBar()->setDisabled(true);
    connect(p->m_scrollArea->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &LSLocationBar::setArrowEnabledState);
    topLayout->addWidget(p->m_scrollArea, 1); // stretch factor=1, make it expandable

    // the arrow button used to scroll to end of the path
    p->m_scrollToEnd = new QToolButton(this);
    p->m_scrollToEnd->setArrowType(rtl ? Qt::LeftArrow : Qt::RightArrow);
    p->m_scrollToEnd->setAutoRepeat(true);
    p->m_scrollToEnd->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    connect(p->m_scrollToEnd, &QToolButton::clicked, this, &LSLocationBar::onScrollButtonClicked);
    topLayout->addWidget(p->m_scrollToEnd);

    // container widget of the path buttons
    p->m_buttonsWidget = new QWidget(this);
    p->m_buttonsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    p->m_buttonsLayout = new QHBoxLayout(p->m_buttonsWidget);
    p->m_buttonsLayout->setContentsMargins(0, 0, 0, 0);
    p->m_buttonsLayout->setSpacing(0);
    p->m_buttonsLayout->setSizeConstraint(QLayout::SetFixedSize); // required when added to scroll area according to QScrollArea doc.
    p->m_scrollArea->setWidget(p->m_buttonsWidget); // make the buttons widget scrollable if the path is too long
}

const QUrl &LSLocationBar::path()
{
    return p->m_currentPath;
}

void LSLocationBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if(event->oldSize().width() != event->size().width()) {
        updateScrollButtonVisibility();
        QTimer::singleShot(0, this, SLOT(ensureToggledVisible()));
    }
}

void LSLocationBar::wheelEvent(QWheelEvent* event) {
    QWidget::wheelEvent(event);
    QAbstractSlider::SliderAction action = QAbstractSlider::SliderNoAction;
    int vDelta = event->angleDelta().y();
    if(vDelta > 0) {
        if(p->m_scrollToStart->isEnabled()) {
            action = QAbstractSlider::SliderSingleStepSub;
        }
    }
    else if(vDelta < 0) {
        if(p->m_scrollToEnd->isEnabled()) {
            action = QAbstractSlider::SliderSingleStepAdd;
        }
    }
    p->m_scrollArea->horizontalScrollBar()->triggerAction(action);
}

void LSLocationBar::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
        openEditor();
    else if(event->button() == Qt::MiddleButton)
    {
        PathButton* btn = qobject_cast<PathButton*>(childAt(event->pos().x(), event->pos().y()));
        if(btn != nullptr) {
            p->m_scrollArea->ensureWidgetVisible(btn,
                                             1); // a harmless compensation for a miscalculation in Qt
            Q_EMIT middleClickChdir(pathForButton(btn));
        }
    }
}

void LSLocationBar::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = new QMenu(this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);

    QAction* action = menu->addAction(tr("&Edit Path"));
    connect(action, &QAction::triggered, this, &LSLocationBar::openEditor);

    action = menu->addAction(tr("&Copy Path"));
    connect(action, &QAction::triggered, this, &LSLocationBar::copyPath);

    menu->popup(mapToGlobal(event->pos()));
}

void LSLocationBar::updateScrollButtonVisibility() {
    // Wait for the horizontal scrollbar to be completely shaped.
    // Without this, the enabled state of arrow buttons might be
    // wrong when the pathbar is created for the first time.
    QTimer::singleShot(0, this, SLOT(setScrollButtonVisibility()));
}

void LSLocationBar::setScrollButtonVisibility() {
    bool showScrollers;
    if(p->m_tempPathEdit != nullptr) {
        showScrollers = false;
    }
    else {
        showScrollers = (p->m_buttonsLayout->sizeHint().width() > width());
    }
    p->m_scrollToStart->setVisible(showScrollers);
    p->m_scrollToEnd->setVisible(showScrollers);
    if(showScrollers) {
        QScrollBar* sb = p->m_scrollArea->horizontalScrollBar();
        int value = sb->value();
        p->m_scrollToStart->setEnabled(value != sb->minimum());
        p->m_scrollToEnd->setEnabled(value != sb->maximum());
        // align scroll buttons horizontally
        p->m_scrollToStart->setMaximumHeight(qMax(p->m_buttonsWidget->height(), p->m_scrollToStart->minimumSizeHint().height()));
        p->m_scrollToEnd->setMaximumHeight(qMax(p->m_buttonsWidget->height(), p->m_scrollToEnd->minimumSizeHint().height()));
    }
}

QUrl LSLocationBar::pathForButton(PathButton* btn)
{
    if(p->m_currentPath.scheme() == "applications")
        return QUrl("applications://");

    std::string fullPath;
    int buttonCount = p->m_buttonsLayout->count() - 1; // the last item is a spacer
    for(int i = 0; i < buttonCount; ++i) {
        if(!fullPath.empty() && fullPath.back() != '/') {
            fullPath += '/';
        }
        PathButton* elem = static_cast<PathButton*>(p->m_buttonsLayout->itemAt(i)->widget());
        fullPath += elem->name();
        if(elem == btn)
            break;
    }
    return QUrl::fromLocalFile(fullPath.c_str());
}

void LSLocationBar::onButtonToggled(bool checked)
{
    if(p->m_currentPath.scheme() == "applications")
        return;

    if(checked) {
        PathButton* btn = static_cast<PathButton*>(sender());
        p->m_toggledBtn = btn;
        p->m_currentPath = pathForButton(btn);
        Q_EMIT chdir(p->m_currentPath);

        // since scrolling to the toggled buton will happen correctly only when the
        // layout is updated and because the update is disabled on creating buttons
        // in setPath(), the update status can be used as a sign to know when to wait
        if(updatesEnabled()) {
            p->m_scrollArea->ensureWidgetVisible(btn, 1);
        }
        else {
            QTimer::singleShot(0, this, SLOT(ensureToggledVisible()));
        }
    }
}

void LSLocationBar::ensureToggledVisible() {
    if(p->m_toggledBtn != nullptr && p->m_tempPathEdit == nullptr) {
        p->m_scrollArea->ensureWidgetVisible(p->m_toggledBtn, 1);
    }
}

void LSLocationBar::onScrollButtonClicked() {
    QToolButton* btn = static_cast<QToolButton*>(sender());
    QAbstractSlider::SliderAction action = QAbstractSlider::SliderNoAction;
    if(btn == p->m_scrollToEnd) {
        action = QAbstractSlider::SliderSingleStepAdd;
    }
    else if(btn == p->m_scrollToStart) {
        action = QAbstractSlider::SliderSingleStepSub;
    }
    p->m_scrollArea->horizontalScrollBar()->triggerAction(action);
}

void LSLocationBar::setPath(const QUrl &path)
{
    if(p->m_currentPath == path)
        return;

    auto oldPath = std::move(p->m_currentPath);
    p->m_currentPath = std::move(path);
    // check if we already have a button for this path
    int buttonCount = p->m_buttonsLayout->count() - 1; // the last item is a spacer
    if(oldPath.isValid() && p->m_currentPath.isParentOf(oldPath)) {
        for(int i = buttonCount - 1; i >= 0; --i) {
            auto btn = static_cast<PathButton*>(p->m_buttonsLayout->itemAt(i)->widget());
            if(pathForButton(btn) == p->m_currentPath) {
                btn->setChecked(true); // toggle the button
                /* we don't need to emit chdir signal here since later
                 * toggled signal will be triggered on the button, which
                 * in turns emit chdir. */
                return;
            }
        }
    }

    /* FIXME: if the new path is the subdir of our full path, actually
     *        we can append several new buttons rather than re-create
     *        all of the buttons. This can reduce flickers. */

    setUpdatesEnabled(false);
    p->m_toggledBtn = nullptr;
    // we do not have the path in the buttons list
    // destroy existing path element buttons and the spacer
    QLayoutItem* item;
    while((item = p->m_buttonsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if(p->m_currentPath.scheme() == "applications")
    {
        auto btn = new PathButton("applications://", "Applications", true, p->m_buttonsWidget);
        btn->show();
        connect(btn, &QAbstractButton::toggled, this, &LSLocationBar::onButtonToggled);
        p->m_buttonsLayout->insertWidget(0, btn);

    }
    if(p->m_currentPath.scheme() == "file")
    {
        // create new buttons for the new path
        auto btnPath = p->m_currentPath.toLocalFile();

        QDir dir(btnPath);
        while(dir.isReadable()) {
            std::string name;
            QString displayName;
            auto parentPaths = btnPath.split('/');
            parentPaths.removeLast();
            auto parent = parentPaths.join('/');
            if(parent == "" && btnPath != "/")
                parent = "/";
            // FIXME: some buggy uri types, such as menu://, fail to return NULL when there is no parent path.
            // Instead, the path itself is returned. So we check if the parent path is the same as current path.
            QUrl pdir(parent);
            auto isRoot = !pdir.isValid() || parent == btnPath;
            if(isRoot) {
                displayName = btnPath;
                name = btnPath.toStdString();
            }
            else {
                displayName = btnPath.split("/").last();
                // NOTE: "name" is used for making the path from its components in PathBar::pathForButton().
                // In places like folders inside trashes of mounted volumes, FilePath::baseName() cannot be
                // used for making a full path. On the other hand, the base name of FilePath::displayName()
                // causes trouble when a file name contains newline or tab.
                //
                // Therefore, we simply set "name" to the last component of FilePath::toString().
                auto pathStr = btnPath;
                pathStr = pathStr.section(QLatin1Char('/'), -1);
                name = pathStr.toStdString();
            }
            // double ampersands to distinguish them from mnemonics
            displayName.replace(QLatin1Char('&'), QLatin1String("&&"));
            auto btn = new PathButton(name, displayName, isRoot, p->m_buttonsWidget);
            btn->show();
            connect(btn, &QAbstractButton::toggled, this, &LSLocationBar::onButtonToggled);
            p->m_buttonsLayout->insertWidget(0, btn);
            if(isRoot) { // this is the root element of the path
                break;
            }
            btnPath = parent;
        }
    }

    p->m_buttonsLayout->addStretch(1); // add a spacer at the tail of the buttons

    // we don't want to scroll vertically. make the scroll area fit the height of the buttons
    // FIXME: this is a little bit hackish :-(
    p->m_scrollArea->setFixedHeight(p->m_buttonsLayout->sizeHint().height());
    updateScrollButtonVisibility();

    // to guarantee that the button will be scrolled to correctly,
    // it should be toggled only after the layout update starts above
    buttonCount = p->m_buttonsLayout->count() - 1;
    if(buttonCount > 0) {
        PathButton* lastBtn = static_cast<PathButton*>(p->m_buttonsLayout->itemAt(buttonCount - 1)->widget());
        // we don't have to emit the chdir signal since the "onButtonToggled()" slot will be triggered by this.
        lastBtn->setChecked(true);
    }

    setUpdatesEnabled(true);
}

void LSLocationBar::openEditor()
{
    if(p->m_tempPathEdit == nullptr)
    {
        p->m_tempPathEdit = new QLineEdit(this);
        delete layout()->replaceWidget(p->m_scrollArea, p->m_tempPathEdit, Qt::FindDirectChildrenOnly);
        p->m_scrollArea->hide();
        p->m_scrollToStart->setVisible(false);
        p->m_scrollToEnd->setVisible(false);
        p->m_tempPathEdit->setText(p->m_currentPath.toDisplayString());

        connect(p->m_tempPathEdit, &QLineEdit::returnPressed, this, &LSLocationBar::onReturnPressed);
        connect(p->m_tempPathEdit, &QLineEdit::editingFinished, this, &LSLocationBar::closeEditor);
    }
    p->m_tempPathEdit->selectAll();
    QApplication::clipboard()->setText(p->m_tempPathEdit->text(), QClipboard::Selection);
    QTimer::singleShot(0, p->m_tempPathEdit, SLOT(setFocus()));
}

void LSLocationBar::closeEditor()
{
    if(p->m_tempPathEdit == nullptr)
        return;

    // If a menu has popped up synchronously (with QMenu::exec), the path buttons may be drawn
    // but the path-edit may not disappear until the menu is closed. So, we hide it here.
    // However, since hiding the path-edit makes it lose focus and emit editingFinished(),
    // we should first disconnect from it to avoid recursive calling of the current function.
    p->m_tempPathEdit->disconnect();
    p->m_tempPathEdit->setVisible(false);
    delete layout()->replaceWidget(p->m_tempPathEdit, p->m_scrollArea, Qt::FindDirectChildrenOnly);
    p->m_scrollArea->show();
    if(p->m_buttonsLayout->sizeHint().width() > width()) {
        p->m_scrollToStart->setVisible(true);
        p->m_scrollToEnd->setVisible(true);
    }

    p->m_tempPathEdit->deleteLater();
    p->m_tempPathEdit = nullptr;
    updateScrollButtonVisibility();

    Q_EMIT editingFinished();
}

void LSLocationBar::copyPath()
{
    QApplication::clipboard()->setText(p->m_currentPath.toDisplayString());
}

void LSLocationBar::onReturnPressed()
{
    QByteArray pathStr = p->m_tempPathEdit->text().toLocal8Bit();
    //setPath(Fm::FilePath::fromPathStr(pathStr.constData()));
}

void LSLocationBar::setArrowEnabledState(int value)
{
    if(p->m_buttonsLayout->sizeHint().width() > width())
    {
        QScrollBar* sb = p->m_scrollArea->horizontalScrollBar();
        p->m_scrollToStart->setEnabled(value != sb->minimum());
        p->m_scrollToEnd->setEnabled(value != sb->maximum());
    }
}
