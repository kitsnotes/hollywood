#ifndef STAGEAPPLET_H
#define STAGEAPPLET_H

#include <appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class StageApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit StageApplet(QObject *parent = nullptr);
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
    QHBoxLayout *m_topLayout = nullptr;
    QLabel *m_duckLabel = nullptr;
    QVBoxLayout *m_rightSideLayout = nullptr;
    QLabel *m_welcome = nullptr;
    QLabel *m_description = nullptr;
    QSpacerItem *verticalSpacer = nullptr;
    QHBoxLayout *m_bottomLayaout = nullptr;
    QVBoxLayout *m_optionLayout = nullptr;
    QCheckBox *m_quackMidnight = nullptr;
    QCheckBox *m_quackHourly = nullptr;
    QPushButton *m_quackNow = nullptr;
    QSpacerItem *horizontalSpacer = nullptr;
    QSpacerItem *verticalSpacer_2 = nullptr;
};

#endif // STAGEAPPLET_H
