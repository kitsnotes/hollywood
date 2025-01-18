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
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QPushButton>

#include <Qt6GSettings/QGSettings>

#define HW_APP_HELP_TOPIC "settings/01-general"

class HWGeneralApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit HWGeneralApplet(QObject *parent = nullptr);

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
    QString helpTopic() const  { return QString(HW_APP_HELP_TOPIC); }
    QStringList searchTokens() const;
private slots:
    void widgetUpdate();
    void fontSliderValueChanged();
    void fontSizeApplyClicked();
    void fontComboChanged(const QFont &f);
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
    QLabel *m_fontPreview;
    QPushButton *m_fontApply;

    QFontComboBox *m_def_font;
    QFontComboBox *m_def_fixedsys;

    QButtonGroup *bg_accentcolor;
    QButtonGroup *bg_apperance;

    uint m_currentFontSize;

    QtGSettings::QGSettings *m_settings;
};

#endif // MOUSEAPPLET_H
