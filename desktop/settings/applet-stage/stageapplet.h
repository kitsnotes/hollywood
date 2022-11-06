#ifndef STAGEAPPLET_H
#define STAGEAPPLET_H

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
#include <QFormLayout>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>

class StageApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit StageApplet(QObject *parent = nullptr);
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
    void layoutSelectionChanged();
    void stageSizeChanged();
    void showClockChecked();
    void updatePositionOptions();
    void layoutIndexChanged(int);
private:
    void setupWidget();
    Qt::Edge m_selectedPosition = Qt::BottomEdge;
    QWidget* m_host = nullptr;
    QHBoxLayout *mainLayout = nullptr;

    QLabel *lb_layoutdesc;
    QRadioButton *m_northern;
    QRadioButton *m_southern;
    QLabel *m_layoutdesc;
    QRadioButton *m_small;
    QRadioButton *m_large;
    QComboBox *m_position;
    QFrame *line;
    QCheckBox *m_showclock;
    QCheckBox *m_showdate;
    QCheckBox *m_seconds;
    QCheckBox *m_24hour;
    QLabel *lbl_clock;

    QButtonGroup *m_layout;
    QButtonGroup *m_stagesize;

};

#endif // STAGEAPPLET_H
