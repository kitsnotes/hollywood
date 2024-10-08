// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "stageclock.h"
#include "stage.h"

#include <QTimer>
#include <QDateTime>
#include <QFont>
#include <QSettings>
#include <QMenu>
#include <QWidgetAction>
#include <hollywood/hollywood.h>

StageClock::StageClock(QWidget *parent)
    : QToolButton(parent)
    , m_timer(new QTimer(this))
    , m_cal(new QCalendarWidget(this))
{
    m_cal->setDateEditEnabled(false);
    m_cal->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    QMenu *menu = new QMenu(this);
    setMenu(menu);
    QWidgetAction *act = new QWidgetAction(this);
    act->setDefaultWidget(m_cal);
    menu->addAction(act);
    menu->addSeparator();
    auto settingsAct = menu->addAction(tr("Date && Time Settings..."));
    connect(settingsAct, &QAction::triggered, []() {
        StageApplication::instance()->openSettingsApplet(HOLLYWOOD_DT_APPLET);
    });
    connect(menu, &QMenu::aboutToShow, [this]() {
        m_cal->showToday();
        m_cal->setSelectedDate(QDate::currentDate());
    });

    setStyleSheet("QToolButton {border: 0px; }"
                  "QToolButton:pressed { background-color: palette(highlight);"
                  " border: none; color: palette(highlighted-text); }"
                  " QToolButton::menu-indicator { image: none; }");
    setAutoRaise(true);

    setPopupMode(QToolButton::InstantPopup);
    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));
    settings.beginGroup("Stage");
    show_date = settings.value("ShowDateInClock", HOLLYWOOD_STCLK_USEDATE).toBool();
    show_seconds = settings.value("ShowSecondsInClock", HOLLYWOOD_STCLK_USESECONDS).toBool();
    show_24hr = settings.value("Use24HourClock", HOLLYWOOD_STCLK_24HOUR).toBool();
    show_ampm = settings.value("ShowAMPMInClock", HOLLYWOOD_STCLK_USEAMPM).toBool();

    m_format = QString("HH:mm");
    if(!show_24hr)
        m_format = QString("h:mm");

    if(show_seconds)
        m_format.append(":ss");

    if(!show_24hr && show_ampm)
        m_format.append(" ap");

    if(show_date)
        m_format.prepend("MMMM d ");

    settings.endGroup();
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &StageClock::updateClock);
    updateClock();
    m_timer->start();
}

void StageClock::updateDpiAwareSettings()
{
    if(parent() == nullptr)
        return;

    auto w = qobject_cast<QWidget*>(parent());
    if(w == nullptr)
        return;

    QFont myFont = font();
    auto size = myFont.pointSize()+1;
    size *= w->window()->devicePixelRatio();
    myFont.setPointSize(qMax(10,size));
    setFont(myFont);
    qDebug() << "setting font size" << qMax(10,size) << w->window()->devicePixelRatio();
}

void StageClock::clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm)
{
    show_date = showDate;
    show_seconds = showSeconds;
    show_24hr = use24hr;
    show_ampm = ampm;

    m_format = QString("HH:mm");
    if(!show_24hr)
        m_format = QString("h:mm");

    if(show_seconds)
        m_format.append(":ss");

    if(!show_24hr && show_ampm)
        m_format.append(" ap");

    if(show_date)
        m_format.prepend("MMMM d ");

    setVisible(showClock);
    updateClock();
}

void StageClock::updateClock()
{
    auto time = QDateTime::currentDateTime().toString(m_format);
    setText(time);
}
