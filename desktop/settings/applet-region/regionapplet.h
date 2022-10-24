#ifndef REGIONAPPLET_H
#define REGIONAPPLET_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>

class RegionalApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit RegionalApplet(QObject *parent = nullptr);
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
    void addLocaleToCombo(QComboBox *combo, const QLocale &locale);
    void initCombo(QComboBox *combo, const QList<QLocale> &allLocales);
    void connectCombo(QComboBox *combo);
    void updateExample();
    bool hasChanged;
    QList<QComboBox *> m_combos;
    QWidget* m_host = nullptr;
    QVBoxLayout *mainLayout = nullptr;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QFormLayout *formLayout;
    QLabel *labelGlobal;
    QComboBox *comboGlobal;
    QGroupBox *checkDetailed;
    QFormLayout *formLayout_2;
    QLabel *labelNumbers;
    QComboBox *comboNumbers;
    QLabel *labelTime;
    QComboBox *comboTime;
    QLabel *labelCurrency;
    QComboBox *comboCurrency;
    QLabel *labelMeasurement;
    QComboBox *comboMeasurement;
    QLabel *labelCollate;
    QComboBox *comboCollate;
    QGroupBox *exampleBox;
    QFormLayout *formLayout_3;
    QLabel *lexNumbers;
    QLabel *exampleNumbers;
    QLabel *lexTime;
    QLabel *exampleTime;
    QLabel *lexCurrency;
    QLabel *exampleCurrency;
    QLabel *lexMeasurement_2;
    QLabel *exampleMeasurement;
    QSpacerItem *verticalSpacer_3;
};

#endif // REGIONAPPLET_H
