#pragma once

#include <QToolButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLineEdit>

class PathEdit;
class PathButton;
class LSLocationBar;
class LSLocationBarPrivate
{
private:
    friend class LSLocationBar;
    LSLocationBar *d;
    LSLocationBarPrivate(LSLocationBar *parent);
    ~LSLocationBarPrivate();
    QToolButton *m_scrollToStart;
    QToolButton* m_scrollToEnd;
    QScrollArea* m_scrollArea;
    QWidget* m_buttonsWidget;
    QHBoxLayout* m_buttonsLayout;
    QLineEdit* m_tempPathEdit;

    QUrl m_currentPath;   // currently active path
    PathButton* m_toggledBtn;
};
