#ifndef OPENRCSERVICEMODEL_H
#define OPENRCSERVICEMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include "openrc.h"

class OpenRCServiceModel;
class OpenRCProcess;
class OpenRCServiceModelObject
{
public:
    OpenRCServiceModelObject();
    OpenRCServiceModelObject(OpenRCProcess *proc);
    OpenRCProcess* process() { return m_proc; }
private:
    friend class OpenRCServiceModel;
    bool m_valid = false;
    OpenRCProcess *m_proc = nullptr;
};

class OpenRCProcess;
class OpenRCMonitor;
class OpenRCServiceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum ColumnItem
    {
        Name,
        Description,
        Status,
        Runlevels
    };
    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    QVariant valueForIndex(const ColumnItem item, const QModelIndex &index) const;
    void update();
protected:
    friend class OpenRCMonitor;
    explicit OpenRCServiceModel(OpenRCMonitor *monitor, QObject *parent = nullptr);
private:
    OpenRCServiceModelObject *itemForIndex(const QModelIndex &index) const;
private:
    OpenRCServiceModelObject m_root;
    OpenRCMonitor *m_monitor;
    QList<ColumnItem> m_visibleColumnOrder;
    QList<OpenRCServiceModelObject*> m_sorted;
};

#endif // OPENRCSERVICEMODEL_H
