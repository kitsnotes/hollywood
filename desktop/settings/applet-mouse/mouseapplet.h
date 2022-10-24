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

class MouseApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet-mouse.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit MouseApplet(QObject *parent = nullptr);
    bool init();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool closeUp() { delete m_host; m_host = nullptr; return true; }
    bool canExit() const { return true; }
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

    QSlider *m_scroll = nullptr;
    QSlider *m_dblclk = nullptr;
    QRadioButton *m_left = nullptr;
    QRadioButton *m_right = nullptr;
    QCheckBox *m_scrolldir = nullptr;
    QSpacerItem *vsend = nullptr;
    QSpacerItem *hsright = nullptr;
    QButtonGroup *m_leftright = nullptr;

};

#endif // MOUSEAPPLET_H
