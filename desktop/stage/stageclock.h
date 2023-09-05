#ifndef STAGECLOCK_H
#define STAGECLOCK_H

#include <QToolButton>
#include <QWidget>
#include <QCalendarWidget>

class StageClock : public QToolButton
{
    Q_OBJECT
public:
    explicit StageClock(QWidget *parent = nullptr);
public slots:
    void clockSettingsChanged(bool showClock, bool showDate, bool showSeconds, bool use24hr, bool ampm);
private slots:
    void updateClock();
private:
    QTimer *m_timer = nullptr;
    bool show_date = false;
    bool show_seconds = false;
    bool show_24hr = true;
    bool show_ampm = true;

    QString m_format;

    QCalendarWidget *m_cal = nullptr;
};

#endif // STAGECLOCK_H
