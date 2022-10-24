#ifndef MOUSEAPPLET_H
#define MOUSEAPPLET_H

#include <hollywood/appletinterface.h>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>

class ASGeneralApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit ASGeneralApplet(QObject *parent = nullptr);

    // Reflect enums in platformtheme.h
    enum SettingApperance {
        SetLight,
        SetDark,
        SetTwilight,
        SetAutomatic
    };
    enum FontSize {
        Small,
        Normal,
        Large,
        XLarge
    };
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool canExit() const { return true; }
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
private slots:
    void widgetUpdate();
private:
    void setupWidget();

    QWidget *m_host = nullptr;

    QRadioButton *m_light;
    QRadioButton *m_dark;
    QRadioButton *m_twilight;

    QRadioButton *ac_blue;
    QRadioButton *ac_purple;
    QRadioButton *ac_pink;
    QRadioButton *ac_red;
    QRadioButton *ac_orange;
    QRadioButton *ac_yellow;
    QRadioButton *ac_green;
    QRadioButton *ac_gray;

    QCheckBox *m_allowAppAccent;

    QSlider *m_fontSize;

    QComboBox *m_browser;
    QComboBox *m_email;

    QButtonGroup *bg_accentcolor;
    QButtonGroup *bg_apperance;

};

#endif // MOUSEAPPLET_H
