#ifndef SURFACEMANAGER_H
#define SURFACEMANAGER_H

#include <QObject>
#include <QButtonGroup>

class PlasmaWindowManagement;
class PlasmaWindow;
class TaskButton;
class StageHost;
class SurfaceManager : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceManager(StageHost *parent = nullptr);
    PlasmaWindowManagement* windowManager() { return m_wndmgr; }
signals:
    void windowCreated(TaskButton *btn);
    void windowDestroyed(TaskButton *btn);
private slots:
    void newPlasmaWindow(PlasmaWindow *c);
    void buttonClicked();
    void windowClosed();
    void windowMinimized();
    void minimizedChanged();
    void stackingOrderChanged();
private:
    StageHost *m_parent = nullptr;
    PlasmaWindowManagement *m_wndmgr = nullptr;

    QList<TaskButton*> m_windows;
    QButtonGroup *m_group = nullptr;
};

#endif // SURFACEMANAGER_H
