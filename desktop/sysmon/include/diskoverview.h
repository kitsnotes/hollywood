#ifndef DiskOverview_H
#define DiskOverview_H

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
class DiskOverview : public QWidget
{
    Q_OBJECT
public:
    explicit DiskOverview(QWidget *parent = nullptr);
public slots:
    void poll();
    void setGraphPollTimeTitle(const QString &title);
private slots:
    void diskDataAvailable(qulonglong read, qulonglong write);
private:
    QString findRootDisk();
    uint findDiskSectorSize(const QString &disk);
private:
    ChartObject *m_diskchart = nullptr;
    StatPoller *m_poll = nullptr;

    uint m_disk_block_size = 512;

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

#endif // DiskOverview_H
