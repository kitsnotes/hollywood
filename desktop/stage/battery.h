#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <QToolButton>
#include <QObject>

class UPowerDeviceInterface;
class UPowerInterface;
class BatteryMonitor : public QToolButton
{
public:
    BatteryMonitor(QWidget *parent = nullptr);
public slots:
    void updateDpiAwareSettings();
private slots:
    void batteryChanged();
private:
    QMenu *m_menu = nullptr;
    UPowerInterface *m_upower = nullptr;
    UPowerDeviceInterface *m_battery = nullptr;

    QAction* m_state = nullptr;
    QAction* m_time = nullptr;
};

#endif // BATTERYMONITOR_H
