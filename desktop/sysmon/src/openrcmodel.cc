#include "openrcmodel.h"
#include "openrc.h"

OpenRCServiceModelObject::OpenRCServiceModelObject()
{
    m_valid = false;
}

OpenRCServiceModelObject::OpenRCServiceModelObject(OpenRCProcess *proc)
{
    m_valid = true;
    m_proc = proc;
}

OpenRCServiceModel::OpenRCServiceModel(OpenRCMonitor *monitor, QObject *parent)
    : QAbstractItemModel(parent)
    , m_monitor(monitor)
{
    m_visibleColumnOrder.append(ColumnItem::Name);
    m_visibleColumnOrder.append(ColumnItem::Status);
    m_visibleColumnOrder.append(ColumnItem::Runlevels);
    m_visibleColumnOrder.append(ColumnItem::Description);

    m_sorted.clear();
    for(auto p : m_monitor->m_processlist)
        m_sorted.append(new OpenRCServiceModelObject(p));
}

QModelIndex OpenRCServiceModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row,column,parent))
        return QModelIndex();

    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    if(row > m_monitor->m_processlist.count()-1)
        return QModelIndex();

    return createIndex(row, column, m_sorted[row]);
}

QModelIndex OpenRCServiceModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int OpenRCServiceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_sorted.count();
}

int OpenRCServiceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_visibleColumnOrder.count();
}

QVariant OpenRCServiceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return valueForIndex(m_visibleColumnOrder.at(index.column()), index);
        break;
    case Qt::DecorationRole:
        /*if (index.column() == 0) {
            QIcon myicon = QIcon();
            auto item = itemForIndex(index);
            if(item->desktopEntry != nullptr)
                myicon = item->desktopEntry->icon();

            return myicon;
        }*/
        break;
    case Qt::TextAlignmentRole:
        break;
    }

    return QVariant();
}

QVariant OpenRCServiceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    QString returnValue;
    switch (m_visibleColumnOrder.at(section))
    {
    case Name: returnValue = tr("Service");
            break;
    case Description: returnValue = tr("Description");
            break;
    case Status: returnValue = tr("Status");
           break;
    case Runlevels: returnValue = tr("Runlevel");
            break;
    default: return QVariant();
    }
    return returnValue;
}

void OpenRCServiceModel::sort(int column, Qt::SortOrder order)
{
    qDebug() << "sorting!";
    Q_UNUSED(column);
    Q_UNUSED(order);
    for(auto i : m_sorted)
        delete i;

    m_sorted.clear();
    for(auto p : m_monitor->m_processlist)
        m_sorted.append(new OpenRCServiceModelObject(p));
}

QVariant OpenRCServiceModel::valueForIndex(const ColumnItem item, const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    QVariant returnValue;
    auto si = itemForIndex(index);
    if(!si->m_valid)
        return QVariant();

    switch(item)
    {
    case Name: returnValue = si->m_proc->name();
        break;
    case Description: returnValue = si->m_proc->description();
        break;
    case Status: returnValue = si->m_proc->status();
        break;
    case Runlevels: returnValue = si->m_proc->runlevels().join(", ");
        break;
    default: return QVariant();
    }

    return returnValue;
}

void OpenRCServiceModel::update()
{
    sort(0);
}

OpenRCServiceModelObject *OpenRCServiceModel::itemForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return const_cast<OpenRCServiceModelObject*>(&m_root);
    auto item = static_cast<OpenRCServiceModelObject*>(index.internalPointer());
    Q_ASSERT(item);
    return item;
}
