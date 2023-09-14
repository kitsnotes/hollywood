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
private slots:
    void upowerChanged();
private:
    UPowerDeviceInterface *m_upower = nullptr;

    QHBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_detailLayout = nullptr;
    QLabel *m_icon = nullptr;
    QProgressBar *m_level = nullptr;
};

#endif // BATTERYMETERWIDGET_H
