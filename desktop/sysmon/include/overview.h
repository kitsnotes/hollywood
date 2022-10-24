#ifndef OVERVIEW_H
#define OVERVIEW_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFile>

class SelectorWidget : public QListView
{
public:
    explicit SelectorWidget(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *e) override;
};

class DiskOverview;
class RAMOverview;
class CPUOverview;
class OverviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverviewWidget(QWidget *parent = nullptr);
    void poll();
    void setGraphPollTimeTitle(const QString &title);
signals:
private slots:
    void selectionActivated(const QModelIndex &newIdx, const QModelIndex &old);
private:
    void updateHostUname();
    void setUpdatesOk();
    void setUpdatesRequired();
private:
    QVBoxLayout *vl_main;
    QHBoxLayout *hl_top_main;
    QLabel *m_icon;
    QVBoxLayout *vl_hostname;
    QLabel *m_hostname;
    QLabel *m_subtitle;
    QSpacerItem *sp_topLayout;
    QVBoxLayout *vl_updates;
    QHBoxLayout *hl_updates;
    QLabel *m_updateIcon;
    QLabel *m_updatelabel;
    QPushButton *m_applyUpdates;
    QSplitter *m_splitter;
    SelectorWidget *m_selection;
    QStandardItemModel *m_selmodel;
    CPUOverview *m_cpuoverview;
    RAMOverview *m_ramoverview;
    DiskOverview *m_diskoverview;
    QStandardItem *m_cpu = nullptr;
    QStandardItem *m_ram = nullptr;
    QStandardItem *m_disk = nullptr;
    QStandardItem *m_network = nullptr;
};

#endif // OVERVIEW_H
