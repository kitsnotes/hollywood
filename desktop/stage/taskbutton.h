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
private:
    PlasmaWindow *m_window = nullptr;
};

#endif // TASKBUTTON_H
