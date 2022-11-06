#ifndef TASKBUTTON_H
#define TASKBUTTON_H

#include <QToolButton>
#include <QWidget>

class PlasmaWindow;
class TaskButton : public QToolButton
{
    Q_OBJECT
public:
    explicit TaskButton(PlasmaWindow *wnd, QWidget *parent = nullptr);
    PlasmaWindow* window();
private slots:
    void themeIconChanged(const QString &icon);
    void displayContextMenu(const QPoint &pos);
    void titleChanged(const QString &title);
    void restore();
    void move();
    void size();
    void minimize();
    void maximize();
    void close();
private:
    PlasmaWindow *m_window = nullptr;
    QMenu *m_context = nullptr;
    QAction *m_restore = nullptr;
    QAction *m_move = nullptr;
    QAction *m_size = nullptr;
    QAction *m_minimize = nullptr;
    QAction *m_maximize = nullptr;
    QAction *m_close = nullptr;
};

#endif // TASKBUTTON_H
