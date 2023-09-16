/* Hollywood Operating System
 * System Settings - System Startup Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */
#ifndef STARTUP_H
#define STARTUP_H

#include <hollywood/appletinterface.h>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QToolButton>
#include <QPushButton>

#include <polkitqt1-gui-actionbutton.h>
#include <polkitqt1-gui-actionbuttons.h>
#include <polkitqt1-authority.h>

#define HW_APP_HELP_TOPIC "settings/26-startup"

class QLabel;
class BootEntryListModel;
class EFIStartupSettingsApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet-efiboot.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit EFIStartupSettingsApplet(QObject *parent = nullptr);
    ~EFIStartupSettingsApplet();
    bool init();
    bool available() const;
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
    void polkitActivate();
    void polkitActivated();
    void restartSelected();
private:    
    void currentIndexChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void setupWidget();
    void enableElevated(bool enable);
    void pollEFI(bool showHidden = false);
    void show_progress_bar();
    void show_error(QString &title, QString &desc);
private:
    QWidget* m_host = nullptr;
    QVBoxLayout *m_hostlayout;
    QListView *m_bootentries;
    BootEntryListModel *m_model;

    QLabel *m_desc;
    QPushButton *m_reboot;
    QPushButton *m_advanced;
    QToolButton *m_polkit_lock;
    QLabel *m_polkit_label;
    PolkitQt1::Gui::ActionButton *m_bt;

    std::vector<uint16_t> order;
};
#endif // STARTUP_H
