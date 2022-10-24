#include "taskbutton.h"
#include "wndmgmt.h"

TaskButton::TaskButton(PlasmaWindow *wnd, QWidget *parent)
    :QToolButton(parent)
    ,m_window(wnd)
{

}

PlasmaWindow *TaskButton::window()
{
    return m_window;
}
