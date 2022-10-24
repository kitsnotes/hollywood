/*
 * Copyright (C) 2016  Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *
 *
 */

#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include "libshell_int.h"
#include <QWidget>
#include <QUrl>

class QToolButton;
class QScrollArea;
class QPushButton;
class QHBoxLayout;

class PathEdit;
class PathButton;

class LIBSHELL_EXPORT LSLocationBar: public QWidget {
    Q_OBJECT
public:
    explicit LSLocationBar(QWidget* parent = nullptr);

    const QUrl& path() {
        return currentPath_;
    }

    void setPath(const QUrl& path);

Q_SIGNALS:
    void chdir(const QUrl& path);
    void middleClickChdir(const QUrl& path);
    void editingFinished();

public Q_SLOTS:
    void openEditor();
    void closeEditor();
    void copyPath();

private Q_SLOTS:
  void onButtonToggled(bool checked);
  void onScrollButtonClicked();
  void onReturnPressed();
  void setArrowEnabledState(int value);
  void setScrollButtonVisibility();
  void ensureToggledVisible();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void updateScrollButtonVisibility();
    QUrl pathForButton(PathButton* btn);

private:
    QToolButton* scrollToStart_;
    QToolButton* scrollToEnd_;
    QScrollArea* scrollArea_;
    QWidget* buttonsWidget_;
    QHBoxLayout* buttonsLayout_;
    QLineEdit* tempPathEdit_;

    QUrl currentPath_;   // currently active path
    PathButton* toggledBtn_;
};

#endif // LOCATIONBAR_H
