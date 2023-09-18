#ifndef STAGEHOST_H
#define STAGEHOST_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QScreen>
#include "wndmgmt.h"
#include <hollywood/layershellwindow.h>

class NotifierHost;
class StatusNotifierButton;
class StageClock;
class TaskButton;
class SurfaceManager;
class BatteryMonitor;
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
    void setClock(StageClock *clock);
    void takeClock();
    void setBattery(BatteryMonitor *clock);
    void takeBattery();
    void setMainEnabled(bool enabled);
public slots:
    void show();
    void createStatusButton(StatusNotifierButton *btn);
    void statusButtonRemoved(StatusNotifierButton *btn);
protected:
    void resizeEvent(QResizeEvent *event) override;
protected slots:
    void createWindowButton(TaskButton *btn);
    void windowClosed(TaskButton *btn);
private:
    bool m_ready = false;
    LayerShellQt::Window *m_lswnd;
    QScreen *m_screen = nullptr;
    Alignment m_align = Vertical;
    QVBoxLayout *m_vbox = nullptr;
    QHBoxLayout *m_hbox = nullptr;
    QSpacerItem *m_spacer = nullptr;
    QToolButton *m_menu = nullptr;
    QToolButton *m_showdesktop = nullptr;

    SurfaceManager *m_sm = nullptr;
    QSpacerItem *m_trayspacer = nullptr;
    StageClock *m_clock = nullptr;
    BatteryMonitor *m_battery = nullptr;
};

#endif // STAGEHOST_H
