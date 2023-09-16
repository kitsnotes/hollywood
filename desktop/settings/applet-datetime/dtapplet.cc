/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dtapplet.h"
#include "analogclock.h"
#include "tzwidget.h"
#include "timezone.h"

#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QToolButton>
#include <QStringList>

HWDateTimeApplet::HWDateTimeApplet(QObject *parent)
    : QObject(parent)
    , SettingsAppletInterface()
{
}

HWDateTimeApplet::~HWDateTimeApplet()
{
    if(m_timer)
    {
        m_timer->stop();
        m_timer->deleteLater();
    }
}

bool HWDateTimeApplet::init()
{
    setupWidget();
    getCurrentTimezone();
    loadSettings();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &HWDateTimeApplet::updateTime);
    m_timer->start(100);
    return true;
}

bool HWDateTimeApplet::closeUp()
{
    if(m_timer)
        m_timer->stop();

    delete m_timer;
    delete m_host;

    return true;
}

bool HWDateTimeApplet::loadSettings()
{
    readNtpConfig();
    m_ntp->setChecked(ntpServiceConfigEnabled());
    updateTime();
    return true;
}

bool HWDateTimeApplet::saveSettings()
{
    return true;
}

QString HWDateTimeApplet::id() const
{
    return QLatin1String("org.originull.datetime");
}

QString HWDateTimeApplet::name() const
{
    return tr("Date & Time");
}

QString HWDateTimeApplet::description() const
{
    return tr("Date, Time, Time Zone, and NTP Configuration");
}

QIcon HWDateTimeApplet::icon() const
{
    const QIcon mine(QIcon::fromTheme("preferences-system-time"));
    return mine;
}

QWidget *HWDateTimeApplet::applet() const
{
    // return a const version of our m_host applet
    if(!m_host)
        return nullptr;

    return const_cast<QWidget*>(m_host);
}

SettingsAppletInterface::Category HWDateTimeApplet::category() const
{
    return System;
}

QStringList HWDateTimeApplet::searchTokens() const
{
    QStringList tokens;
    tokens << tr("Change date & time");
    tokens << tr("Change system timezone");

    return tokens;
}

void HWDateTimeApplet::updateTime()
{
    auto dt = QDateTime::currentDateTime();
    m_time->setTime(dt.time());
    m_date->setDate(dt.date());
    m_calendar->setSelectedDate(dt.date());
}

void HWDateTimeApplet::polkitActivate()
{
    PolkitQt1::Gui::Action *action
            = qobject_cast<PolkitQt1::Gui::Action *>(sender());
    action->activate();
}

void HWDateTimeApplet::polkitActivated()
{
    PolkitQt1::Gui::Action *action = qobject_cast<PolkitQt1::Gui::Action *>(sender());
    qDebug() << "pretending to be the mechanism for action:" << action->actionId();

    PolkitQt1::Authority::Result result;
    PolkitQt1::UnixProcessSubject subject(static_cast<uint>(QCoreApplication::applicationPid()));

    result = PolkitQt1::Authority::instance()->checkAuthorizationSync(action->actionId(), subject,
             PolkitQt1::Authority::AllowUserInteraction);
    if (result == PolkitQt1::Authority::Yes) {
        // in the helper you will do the action
        qDebug() << "caller is authorized to do:" << action->actionId();
        m_bt->setIcon(QIcon::fromTheme("emblem-unlocked"));
        m_polkit_label->setText(tr("Click the lock to prevent further changes."));
        enableElevated(true);
    } else {
        // OR return false to notify the caller that the action is not authorized.
        qDebug() << "caller is NOT authorized to do:" << action->actionId();
    }
}

void HWDateTimeApplet::enableElevated(bool enable)
{
    m_ntp->setEnabled(enable);
    m_ntpservers->setEnabled(enable);
    m_date->setEnabled(enable);
    m_time->setEnabled(enable);
    m_calendar->setEnabled(enable);
    m_timezones->setEnabled(enable);
}

void HWDateTimeApplet::setupWidget()
{
    m_host = new QWidget(0);
    m_host->setObjectName(QString::fromUtf8("DateTimeAppletHost"));

    m_hostlayout = new QGridLayout(m_host);
    m_tabs = new QTabWidget(m_host);

    tabDateTime = new QWidget();
    auto dateTimeGridLayout = new QGridLayout(tabDateTime);
    auto ntplayout = new QHBoxLayout();

    m_ntp = new QCheckBox(tabDateTime);
    m_ntpservers = new QComboBox(tabDateTime);
    m_ntp->setText(tr("Automatically set the date && time"));

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(2);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_ntpservers->sizePolicy().hasHeightForWidth());
    m_ntpservers->setSizePolicy(sizePolicy);
    m_ntpservers->setEditable(true);

    if(!hasNtpService())
    {
        m_ntp->setVisible(false);
        m_ntpservers->setVisible(false);
    }
    else
    {
        auto hs_left_ntp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        auto hs_right_ntp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        ntplayout->addItem(hs_left_ntp);
        ntplayout->addWidget(m_ntp);
        ntplayout->addWidget(m_ntpservers);
        ntplayout->addItem(hs_right_ntp);
    }

    auto hl_datetime = new QHBoxLayout();
    auto hs_dt_left = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_dt_right = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);


    m_date = new QDateEdit(tabDateTime);
    m_calendar = new QCalendarWidget(tabDateTime);
    // The vertical layout for date/calendar (left side)
    auto vl_calendar = new QVBoxLayout();
    auto hl_date = new QHBoxLayout();
    auto hs_date = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_date_end = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_gap = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

    // Construct our edit box centered
    hl_date->addItem(hs_date);
    hl_date->addWidget(m_date);
    hl_date->addItem(hs_date_end);
    // Construct our vertical layout with edit box & calendar
    vl_calendar->addLayout(hl_date);
    vl_calendar->addWidget(m_calendar);

    // Construct our vertical clock layout (right side)
    m_time = new QTimeEdit(tabDateTime);
    m_clockView = new ASAnalogClock(tabDateTime);
    auto vl_clock = new QVBoxLayout();
    auto hl_time = new QHBoxLayout();
    auto hs_timest = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto hs_timend = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hl_time->addItem(hs_timest);
    hl_time->addWidget(m_time);
    hl_time->addItem(hs_timend);
    vl_clock->addLayout(hl_time);
    vl_clock->addWidget(m_clockView);

    // Constructor our horizontal layout combining clock and calendar stuff
    hl_datetime->addItem(hs_dt_left);
    hl_datetime->addLayout(vl_calendar);
    hl_datetime->addItem(hs_gap);
    hl_datetime->addLayout(vl_clock);
    hl_datetime->addItem(hs_dt_right);

    // Build our master tab layout
    auto vs_end = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    dateTimeGridLayout->addLayout(ntplayout, 0, 0, 1, 1);
    dateTimeGridLayout->addLayout(hl_datetime, 1, 0, 1, 1);
    dateTimeGridLayout->addItem(vs_end, 2, 0, 1, 1);

    m_tabs->addTab(tabDateTime, QString());
    m_tabs->setTabText(m_tabs->indexOf(tabDateTime), tr("Date && Time"));

    // END "Date & Time" Tab

    // Construct the "Time Zone" tab
    tabTimeZone = new QWidget();
    auto tzlayout = new QVBoxLayout(tabTimeZone);
    m_zones = new ZonesModel(this);
    m_timezones = new TimeZoneWidget(m_zones, tabTimeZone);

    tzlayout->addWidget(m_timezones);
    m_tabs->addTab(tabTimeZone, QString());
    m_tabs->setTabText(m_tabs->indexOf(tabTimeZone), tr("Time Zone"));
    // END "Time Zone" Tab

    m_polkit_lock = new QToolButton(m_host);
    m_polkit_label = new QLabel(m_host);
    m_polkit_lock->setIconSize(QSize(32,32));
    m_polkit_lock->setAutoRaise(true);
    m_polkit_label->setText(tr("Click the lock to authenticate for changes."));

    m_bt = new PolkitQt1::Gui::ActionButton(m_polkit_lock,
               "org.originull.hwsettings.datetime.auth", this);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::triggered,
            this,  &HWDateTimeApplet::polkitActivate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::clicked,
            m_bt, &PolkitQt1::Gui::ActionButton::activate);
    connect(m_bt, &PolkitQt1::Gui::ActionButton::authorized,
            this, &HWDateTimeApplet::polkitActivated);
    m_bt->setIcon(QIcon::fromTheme("emblem-locked"));

    auto polkit_layout = new QHBoxLayout();
    polkit_layout->addWidget(m_polkit_lock);
    polkit_layout->addWidget(m_polkit_label, 1);

    auto vs_end_hl = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_hostlayout->addWidget(m_tabs, 0, 0, 1, 1);
    m_hostlayout->addLayout(polkit_layout, 1, 0);
    m_hostlayout->addItem(vs_end_hl, 2, 0, 1, 1);

    m_tabs->setCurrentIndex(0);
    enableElevated(false);
}

void HWDateTimeApplet::readNtpConfig()
{
    QString server, pool;
    QFile file(NTP_CONFIG_FILE);
    if(file.open(QFile::ReadOnly))
    {
        file.seek(0);
        QTextStream stream(&file);
        for (QString line = stream.readLine();
             !line.isNull();
             line = stream.readLine()) {
            if(line.startsWith("servers"))
            {
                // we have a pool, this is preferable
                auto bg = line.split(' ');
                if(bg.count() > 1)
                {
                    pool = bg[1];
                    break;
                }
            }
            if(line.startsWith("server"))
            {
                // an individual NTP server
                auto bg = line.split(' ');
                if(bg.count() > 1)
                {
                    server = bg[1];
                }
            }
        }
        file.close();
    }
    if(!pool.isEmpty())
    {
        m_ntpservers->addItem(pool);
        m_ntpservers->setCurrentText(pool);
    }
    if(!server.isEmpty())
    {
        m_ntpservers->addItem(server);
        if(pool.isEmpty())
            m_ntpservers->setCurrentText(server);
    }
}

bool HWDateTimeApplet::hasNtpService()
{
    QProcess ps;
    QByteArray ba;
    ps.setProcessChannelMode(QProcess::MergedChannels);
    // check unregistered services
    ps.start("/usr/bin/rc-status", QStringList() << "-u");

    if (ps.waitForStarted(-1))
        while(ps.waitForReadyRead(-1))
            ba += ps.readAll();

    for(auto l : ba.split('\n'))
    {
        auto lt = l.trimmed();
        if(lt.startsWith(OPENRC_NTP_SVC))
            return true;
    }

    // check registered services
    ba.clear();

    ps.start("/usr/bin/rc-status", QStringList() << "-a");
    if (ps.waitForStarted(-1))
        while(ps.waitForReadyRead(-1))
            ba += ps.readAll();

    for(auto l : ba.split('\n'))
    {
        auto lt = l.trimmed();
        if(lt.startsWith(OPENRC_NTP_SVC))
            return true;
    }

    return false;
}

bool HWDateTimeApplet::ntpServiceConfigEnabled()
{
    QProcess ps;
    QByteArray ba;
    ps.setProcessChannelMode(QProcess::MergedChannels);
    // check registered services
    ps.start("/bin/rc-status", QStringList() << "-a");

    if (ps.waitForStarted(-1))
        while(ps.waitForReadyRead(-1))
            ba += ps.readAll();

    for(auto &l : ba.split('\n'))
    {
        auto lt = l.trimmed();
        if(lt.startsWith(OPENRC_NTP_SVC))
            return true;
    }

    return false;
}

void HWDateTimeApplet::getCurrentTimezone()
{
    QFile f("/etc/localtime");
    if(!f.exists())
    {
        auto zone = m_zones->findtz(QLatin1String("Etc"), QLatin1String("UTC"));
        if(zone)
            m_timezones->setCurrentLocation(zone);
    }
    else
    {
        QStringList sl = f.symLinkTarget().split('/');

        auto location = sl.takeLast();
        auto region = sl.takeLast();

        auto zone = m_zones->findtz(region, location);
        if(zone)
            m_timezones->setCurrentLocation(zone);
        else
            m_timezones->setCurrentLocation(m_zones->findtz(QLatin1String("Etc"), QLatin1String("UTC")));
    }
}

