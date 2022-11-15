#ifndef STAGEHOST_H
#define STAGEHOST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QScreen>
#include "wndmgmt.h"
#include <hollywood/layershellwindow.h>

class StageClock;
class TaskButton;
class StageHost : public QWidget
{
    Q_OBJECT
public:
    enum Alignment
    {
        Vertical,
        Horizontal
    };
    explicit StageHost(QScreen *screen, QWidget *parent = nullptr);

    void setAlignment(Alignment align);
    void createWindowButton(PlasmaWindow *wnd);
    void removeWindowButton(PlasmaWindow *wnd);

public slots:
    void show();
    void stackingOrderChanged();
protected:
    void resizeEvent(QResizeEvent *event) override;
protected slots:
    void buttonClicked();
    void windowClosed();
    void minimizedChanged();
private:
    bool m_ready = false;
    LayerShellQt::Window *m_lswnd;
    QScreen *m_screen = nullptr;
    Alignment m_align = Horizontal;
    QVBoxLayout *m_vbox = nullptr;
    QHBoxLayout *m_hbox = nullptr;
    QSpacerItem *m_spacer = nullptr;
    QToolButton *m_menu = nullptr;
    QToolButton *m_showdesktop = nullptr;

    QList<TaskButton*> m_windows;
    QButtonGroup *m_group = nullptr;

    StageClock *m_clock = nullptr;
};

#endif // STAGEHOST_H
