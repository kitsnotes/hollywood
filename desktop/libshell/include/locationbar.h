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

class PathButton;
class LSLocationBarPrivate;
class LIBSHELL_EXPORT LSLocationBar: public QWidget {
    Q_OBJECT
public:
    explicit LSLocationBar(QWidget* parent = nullptr);

    const QUrl& path();

    void setPath(const QUrl& path);

signals:
    void chdir(const QUrl& path);
    void middleClickChdir(const QUrl& path);
    void editingFinished();

public slots:
    void openEditor();
    void closeEditor();
    void copyPath();

private slots:
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
    LSLocationBarPrivate *p;
};

#endif // LOCATIONBAR_H
