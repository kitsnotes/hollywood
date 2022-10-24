// Hollywood System Monitor
// (C) 2022 Cat Stevenson <cat@originull.org>
// (C) 2022 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHARTOBJECT_H
#define CHARTOBJECT_H

#include <QObject>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QAreaSeries>
#include <QCategoryAxis>

#include "window.h"

class ChartObject : public QObject
{
    Q_OBJECT
public:
    enum ChartType {
        CPU,
        RAM,
        Disk,
        Network
    };
    explicit ChartObject(ChartType type, QObject *parent = nullptr, qulonglong ramsize = 0);

    QChart* chart();
    void updateXAxis(const QString &speed);
    void pushSample(float value);
    void setChartAccentColor(const QColor &color);
    void pushMemory(qulonglong total, qulonglong free, qulonglong avail, qulonglong buffers, qulonglong cache);
    void pushDiskStat(qulonglong read, qulonglong write);
protected:
    static const int samples = 60;
private:
    void updateDiskCeiling();
    void setIOColors();
    void setupRAMColors();
private:
    QCategoryAxis *m_x = nullptr;
    QValueAxis *m_y = nullptr;
    QValueAxis *m_y2 = nullptr; // used for RAM, disk IO, network IO

    ChartType m_type = CPU;
    QChart *m_chart = nullptr;
    QVector<QPointF> m_values;
    QVector<QPointF> m_values2;
    QAreaSeries *m_series = nullptr;
    QAreaSeries *m_series2 = nullptr;

    qulonglong m_disk_ceiling;

    QString m_currentXString;
};

#endif // CHARTOBJECT_H
