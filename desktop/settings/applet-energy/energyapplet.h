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

class EnergyApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit EnergyApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings() { return true; }
    bool saveSettings() { return true; }
    bool canExit() const { return true; }
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
private:
    void setupWidget();
    QWidget* m_host = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    QTabWidget *m_tabs;
    QWidget *m_mains;
    QFormLayout *formLayout;
    QLabel *lbl_mains_comp_sleep;
    QSlider *m_mains_comp_sleep;
    QLabel *lbl_mains_disp_sleep;
    QSlider *m_mains_disp_sleep;
    QSpacerItem *vs_mains;
    QWidget *m_battery;
    QFormLayout *formLayout_3;
    QLabel *lbl_batt_comp_sleep;
    QSlider *m_batt_comp_sleep;
    QLabel *lbl_batt_disp_sleep;
    QSlider *m_batt_disp_sleep;
    QSpacerItem *vs_batt;
    QHBoxLayout *hl_bottom;
    QCheckBox *m_showinmenu;
    QSpacerItem *hs_bottom;
    QToolButton *m_help;

};

#endif // ENERGYAPPLET_H
