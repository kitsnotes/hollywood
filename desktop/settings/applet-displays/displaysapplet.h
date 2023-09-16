#ifndef DISPLAYSAPPLET_H
#define DISPLAYSAPPLET_H

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
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>

#define HW_APP_HELP_TOPIC "settings/21-displays"

class ScreenManager;
class ScreenManagerWidget;
class DisplaysApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit DisplaysApplet(QObject *parent = nullptr);
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
    QString helpTopic() const  { return QString(HW_APP_HELP_TOPIC); }
    QStringList searchTokens() const;
private:
    void setupWidget();
    void disableAll();
    void initialScreenProvision();
private slots:
    void selectedScreenChanged();
private:
    QWidget* m_host = nullptr;
    QVBoxLayout *m_layout;
    ScreenManager *m_manager;
    ScreenManagerWidget *m_sman;

    QRadioButton *m_mode_off;
    QRadioButton *m_mode_primary;
    QRadioButton *m_mode_extend;
    QRadioButton *m_mode_mirror;
    QComboBox *m_mirrorselect;
    QLabel *l_res;
    QComboBox *m_resolution;
    QLabel *l_orientation;
    QComboBox *m_orientation;
    QLabel *l_position;
    QComboBox *m_position_select;
    QLabel *l_position_2;
    QComboBox *m_position_buddy;
    QButtonGroup *bg_mode;

};

#endif // DISPLAYSAPPLET_H
