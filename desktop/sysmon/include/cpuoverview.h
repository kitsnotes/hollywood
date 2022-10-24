// Hollywood System Monitor
// (C) 2022 Cat Stevenson <cat@originull.org>
// (C) 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CPUOVERVIEWWIDGET_H
#define CPUOVERVIEWWIDGET_H

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

class GPUGauge;
class StatPoller;
class ChartObject;
class CPUOverview : public QWidget
{
    Q_OBJECT
public:
    explicit CPUOverview(QWidget *parent = nullptr);

    struct CpuID {
        uint processor = 0;
        uint cores = 1;
        float mhz = 0;
        QByteArray cache = NULL;
        float mips = 0;
        QByteArray vendor = NULL;
        QByteArray model = NULL;
        QByteArray modelname = NULL;
        QByteArray stepping = 0;
        int revision = 0;
        QByteArray x86vendor = NULL;
        QByteArray physicalPackage = NULL;
        QByteArray coreSiblings = NULL;

        bool has_cpufreq = false;
        uint cpufreq_min = 0;
        uint cpufreq_max = 0;
    };

public slots:
    void poll();
    void setGraphPollTimeTitle(const QString &title);
signals:
protected:
    void changeEvent(QEvent *event);
private slots:
    void cpuAverageAvailable(float data);

private:
    void setupGovernorGraph();
    void arm_cpu_decode(struct CpuID *desc);
    void getCpuidInfo();
    void processCpuidBlock(QList<QByteArray> d);
    QList<CpuID> m_cpus;
    ChartObject *m_cpuchart = nullptr;
    StatPoller *m_poll = nullptr;

    QVBoxLayout *vl_main;
    QHBoxLayout *hl_top_main;
    QLabel *m_icon;
    QVBoxLayout *vl_hostname;
    QLabel *m_cpuname;
    QLabel *m_cpuinfo;
    QGraphicsView *m_speed;
    QHBoxLayout *vl_graphtoggle;
    QToolButton *m_average;
    QToolButton *m_percpu;

    QChartView *m_chart;
    QHBoxLayout *hl_infocenter;
    QHBoxLayout *fl_details;
    QLabel *m_sockets;
    QLabel *m_cores;
    QLabel *m_vm;
    QLabel *m_caches;

    bool m_cpufreq = false;
    GPUGauge *m_speedGauge = nullptr;
    QGraphicsScene *m_scene = nullptr;
    uint m_cpufreq_max_mhz;
    uint m_cpufreq_min_mhz;

};

#endif // CPUOVERVIEWWIDGET_H
