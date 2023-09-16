/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DTAPPLET_H
#define DTAPPLET_H

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

#include <polkitqt1-gui-actionbutton.h>
#include <polkitqt1-gui-actionbuttons.h>
#include <polkitqt1-authority.h>

#define OPENRC_NTP_SVC      "openntpd"
#define NTP_CONFIG_FILE     "/etc/ntpd.conf"

#define HW_APP_HELP_TOPIC "settings/20-datetime"

class QLabel;
class ASAnalogClock;
class TimeZoneWidget;
class ZonesModel;
class HWDateTimeApplet : public QObject, SettingsAppletInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID SettingsAppletInterfaceIID FILE "applet-dt.json")
    Q_INTERFACES(SettingsAppletInterface)
public:
    explicit HWDateTimeApplet(QObject *parent = nullptr);
    ~HWDateTimeApplet();
    bool init();
    bool closeUp();
    bool available() const { return true; }
    bool loadSettings();
    bool saveSettings();
    bool canExit() const { return true; }
    QString id() const;
    QString name() const;
    QString description() const;
    QIcon icon() const;
    QWidget* applet() const;
    Category category() const;
    QString helpTopic() const  { return QString(HW_APP_HELP_TOPIC); }
    QStringList searchTokens() const;
private slots:
    void updateTime();
    void polkitActivate();
    void polkitActivated();
private:
    void enableElevated(bool enable);
    void setupWidget();
    void readNtpConfig();
    bool hasNtpService();
    bool ntpServiceConfigEnabled();
    void getCurrentTimezone();
private:
    QTimer *m_timer = nullptr;
    QWidget* m_host = nullptr;
    QGridLayout *m_hostlayout;
    QTabWidget *m_tabs;
    QWidget *tabDateTime;

    QCheckBox *m_ntp;
    QComboBox *m_ntpservers;

    QDateEdit *m_date;
    QCalendarWidget *m_calendar;
    QSpacerItem *hs_gap;

    QTimeEdit *m_time;
    ASAnalogClock *m_clockView;

    QWidget *tabTimeZone;
    ZonesModel *m_zones;
    TimeZoneWidget *m_timezones;

    QToolButton *m_polkit_lock;
    QLabel *m_polkit_label;
    PolkitQt1::Gui::ActionButton *m_bt;
};

#endif // DTAPPLET_H
