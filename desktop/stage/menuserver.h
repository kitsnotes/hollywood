#ifndef MENUSERVER_H
#define MENUSERVER_H

#include <QWidget>
#include <hollywood/layershellwindow.h>
#include <QMenuBar>
#include <QHBoxLayout>

class NotifierHost;
class StatusNotifierButton;
class StageClock;
class TaskButton;
class MenuServer : public QWidget
{
    Q_OBJECT
public:
    explicit MenuServer(NotifierHost *notifier, QScreen *screen, QWidget *parent = nullptr);
    QMenuBar* menuBar() { return m_menuBar; }
    void installMenu(QMenu *menu);
    void cleanMenu();
    void setClock(StageClock *clock);
public slots:
    void show();
private slots:
    void createStatusButton(StatusNotifierButton *btn);
    void statusButtonRemoved(StatusNotifierButton *btn);
signals:
private:
    bool m_ready = false;
    LayerShellQt::Window *m_lswnd;
    QScreen *m_screen;
    QHBoxLayout *m_layout;
    QMenuBar *m_menuBar;
    QSpacerItem *m_spacer;

    NotifierHost *m_notifier = nullptr;
    StageClock *m_clock = nullptr;

    QWidget *m_opposite = nullptr;
    QHBoxLayout *vl_opposite = nullptr;
};

#endif // MENUSERVER_H