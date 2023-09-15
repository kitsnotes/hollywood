#ifndef ENERGYAPPLET_H
#define ENERGYAPPLET_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QGroupBox>

#include "upower.h"
#include "labeledslider.h"

class BatteryMeterWidget;
class SleepSlider : public LabeledSlider
{
    Q_OBJECT
public:
    explicit SleepSlider(QWidget *parent = nullptr);
    uint valueFromSliderPosition(uint sliderPos);
    uint valueToSliderPosition(uint value);
    QString formatTime(uint minutes);
protected:
    void keyPressEvent(QKeyEvent *ev) override;
};

class EnergyApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit EnergyApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool canExit() const { return true; }
    bool closeUp();
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
private:
    void setupWidget();
    bool queryForPortability();
    void enumerateUPower();
    uint normalizeValue(uint minutes);

    QList<BatteryMeterWidget*> m_batteries;
    QList<BatteryMeterWidget*> m_ups_batteries;

    QWidget* m_host = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    UPowerInterface *m_upower = nullptr;
    QGroupBox *m_batterysettings;
    QGroupBox *m_mainssettings;

    QLabel *lbl_mains_disp_sleep;
    SleepSlider *m_mains_disp_sleep;
    QLabel *m_disp_sleep_val;
    QCheckBox *m_preventsleep;
    QCheckBox *m_preventsleeplid;
    QCheckBox *m_harddisks;

    QFormLayout *fl_battery;
    QLabel *lbl_batt_disp_sleep;
    SleepSlider *m_batt_disp_sleep;
    QLabel *m_disp_bat_sleep_val;
    QCheckBox *m_harddisks_battery;
    QSpacerItem *vs_batt;
    QHBoxLayout *hl_bottom;
    QCheckBox *m_showinmenu;
    QSpacerItem *hs_bottom;

    QLabel *m_warning_icon;
    QLabel *m_energy_warning;
    QToolButton *m_help;

};

#endif // ENERGYAPPLET_H
