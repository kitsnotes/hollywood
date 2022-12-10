#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QToolButton>

#include <preferencedialog.h>

class GeneralTab : public QWidget
{
    Q_OBJECT
public:
    GeneralTab(QWidget *parent = nullptr);
    ~GeneralTab();
    void loadSettings();
    void saveSettings();
private:
    void comboBoxChanged(int index);
    void radioChanged(bool checked);
private:
    QGridLayout *gridLayout;
    QComboBox *m_profile;
    QComboBox *m_cwd;
    QComboBox *m_profiletab;
    QComboBox *m_cwdtab;

    QRadioButton *m_historyunlimited;
    QRadioButton *m_historyLimited;
    QSpinBox *m_historyLimit;
    QCheckBox *m_useCtrlTabs;
};

class ProfileTab;
class PreferencesDialog : public HWPreferenceDialog
{
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent = nullptr);
private:
    GeneralTab *m_general;
    ProfileTab *m_profile;
};

#endif // PREFERENCES_H
