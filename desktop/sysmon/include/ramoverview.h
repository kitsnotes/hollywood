#ifndef RAMOVERVIEW_H
#define RAMOVERVIEW_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QIcon>
#include <QChartView>
#include <QByteArray>
#include <QToolButton>
#include <QActionGroup>

class ChartObject;
class StatPoller;
class RAMOverview : public QWidget
{
    Q_OBJECT
public:
    explicit RAMOverview(QWidget *parent = nullptr);
public slots:
    void poll();
    void setGraphPollTimeTitle(const QString &title);
protected:
    void changeEvent(QEvent *event);
private slots:
    void ramDataAvailable(qulonglong total, qulonglong free, qulonglong avail, qulonglong buf, qulonglong cache);
private:
    ChartObject *m_ramchart = nullptr;
    StatPoller *m_poll = nullptr;

    QVBoxLayout *vl_main;
    QHBoxLayout *hl_top_main;
    QLabel *m_icon;
    QVBoxLayout *vl_hostname;
    QLabel *m_cpuname;
    QLabel *m_cpuinfo;
    QVBoxLayout *vl_updates;
    QHBoxLayout *hl_top_graphs;

    QChartView *m_chart;
};

#endif // RAMOVERVIEW_H
