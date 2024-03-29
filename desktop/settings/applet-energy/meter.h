#ifndef BATTERYMETERWIDGET_H
#define BATTERYMETERWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>

class UPowerDeviceInterface;
class BatteryMeterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BatteryMeterWidget(UPowerDeviceInterface *up, QWidget *parent = nullptr);
    ~BatteryMeterWidget();
private slots:
    void upowerChanged();
private:
    UPowerDeviceInterface *m_upower = nullptr;

    QHBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_detailLayout = nullptr;
    QLabel *m_icon = nullptr;
    QLabel *m_info = nullptr;
    QProgressBar *m_level = nullptr;
    QLabel *m_health = nullptr;
    QPushButton *m_details = nullptr;
};

#endif // BATTERYMETERWIDGET_H
