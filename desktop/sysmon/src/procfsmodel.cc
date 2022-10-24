#include "procfsmodel.h"
#include <mimeapps.h>
#include <desktopentry.h>

LSProcfsModel::LSProcfsModel(QObject *parent)
    : QAbstractTableModel(parent),
      m_timer(new QTimer(this)),
      m_mime(new LSMimeApplications(this))
{
    m_mime->cacheAllDesktops();
    m_clockticks = sysconf(_SC_CLK_TCK);
    m_pagesize = sysconf(_SC_PAGESIZE);
    createDefaultColumnOrder();
    reload();
}

QModelIndex LSProcfsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= rowCount(parent) || column >= columnCount(parent))
        return QModelIndex();

    if(row > m_flat.count()-1)
        return QModelIndex();

    return createIndex(row, column, m_flat[row]);
}

QModelIndex LSProcfsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int LSProcfsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(m_mode == FlatMode)
        return m_byid.count();

    return 0;
}

int LSProcfsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_visibleColumnOrder.count();
}

QVariant LSProcfsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this)
        return QVariant();

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return valueForIndex(m_visibleColumnOrder.at(index.column()), index);
        break;
    case Qt::DecorationRole:
        if (index.column() == 0) {
            QIcon myicon = QIcon();
            auto item = itemForIndex(index);
            if(item->desktopEntry != nullptr)
                myicon = item->desktopEntry->icon();

            return myicon;
        }
        break;
    case Qt::TextAlignmentRole:
        break;
    }

    return QVariant();
}

QVariant LSProcfsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DecorationRole:
        if (section == 0)
        {
            // ### TODO oh man this is ugly and doesn't even work all the way!
            // it is still 2 pixels off
            QImage pixmap(16, 1, QImage::Format_Mono);
            pixmap.fill(0);
            pixmap.setAlphaChannel(pixmap.createAlphaMask());
            return pixmap;
        }
        break;
    case Qt::TextAlignmentRole:
        return Qt::AlignLeft;
    }

    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);

    QString returnValue;
    switch (m_visibleColumnOrder.at(section))
    {
    case ProcessId: returnValue = tr("PID");
            break;
    case ParentProcessId: returnValue = tr("Parent PID");
            break;
    case UserId: returnValue = tr("User ID");
           break;
    case UserName: returnValue = tr("User");
            break;
    case ProcessName: returnValue = tr("Process");
            break;
    case FullCommandLine: returnValue = tr("Command Line");
            break;
    case CPUTime: returnValue = tr("CPU");
            break;
    case ResidentMemory: returnValue = tr("Memory");
            break;
    case ProcessState: returnValue = tr("State");
            break;
    default: return QVariant();
    }
    return returnValue;
}

QVariant LSProcfsModel::valueForIndex(const ColumnItem item, const QModelIndex &index) const
{
    if(!index.isValid())
        return QVariant();

    QVariant returnValue;
    auto procItem = itemForIndex(index);
    switch(item)
    {
    case ProcessId: returnValue = procItem->pid;
            break;
    case ParentProcessId: returnValue = procItem->ppid;
            break;
    case UserId: returnValue = procItem->user;
           break;
    case UserName: returnValue = username(procItem->user);
            break;
    case ProcessName: returnValue = procItem->processName;
            break;
    case FullCommandLine: returnValue = procItem->commandLine;
            break;
    case CPUTime: returnValue = cpuPercent(procItem);
            break;
    case ResidentMemory: returnValue = formatMemory(procItem);
            break;
    case ProcessState: returnValue = formatState(procItem->state);
            break;
    default: return QVariant();
    }

    return returnValue;
}

pid_t LSProcfsModel::pidForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;
    auto item = static_cast<LSProcessItem*>(index.internalPointer());
    Q_ASSERT(item);
    return item->pid;
}

QString LSProcfsModel::desktopEntryIconForIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return QString();
    auto item = static_cast<LSProcessItem*>(index.internalPointer());
    Q_ASSERT(item);
    if(item->desktopEntry != nullptr)
        return item->desktopEntry->value(QLatin1String("Icon")).toString();

    return QString();
}

void LSProcfsModel::reload()
{
    poll();
}

LSProcessItem *LSProcfsModel::itemForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return const_cast<LSProcessItem*>(&m_root);
    auto item = static_cast<LSProcessItem*>(index.internalPointer());
    Q_ASSERT(item);
    return item;
}

void LSProcfsModel::clearStale()
{
    bool removed = false;
    for(auto item : m_byid)
    {
        if(!QFile::exists(QString("/proc/%1/stat").arg(item->pid)))
        {
            uint pid = item->pid;
            int pos = m_flat.indexOf(item);
            beginRemoveRows(QModelIndex(), pos, pos);
            m_byid.remove(pid);
            delete item;
            removed = true;
        }
    }
    if(removed)
        endRemoveRows();
}

QString LSProcfsModel::cpuPercent(LSProcessItem *item) const
{
    struct sysinfo s_info;
    if(sysinfo(&s_info) != 0)
        return QString("NaN");

    ulong uptime = s_info.uptime;
    ulong total_time = (item->utime + item->stime) / m_clockticks;
    ulong seconds = uptime - (item->starttime / m_clockticks);

    float percent = 100*((total_time/m_clockticks)/seconds);
    return QString("%1%").arg(percent);
}

void LSProcfsModel::poll()
{
    auto oldpids = m_flatpids;
    QList<pid_t> newpids;
    QDir dir("/proc");
    dir.setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::NoSort);
    for(QString i : dir.entryList())
    {
        bool ok;
        uint pid = QVariant(i).toUInt(&ok);
        if(ok)
        {
            int row = m_flatpids.indexOf(pid);
            newpids.append(pid);
            if(row >= 0)
            {
                // existing process let's update it
                m_flat[row]->process();
                emit dataChanged(index(row, 0), index(row, columnCount()-1));
            }
            else
            {
                // new PID let's insert a row
                auto *proc = new LSProcessItem(pid);
                proc->process();
                QString process(proc->commandLine);
                auto keys = process.split(" ");
                proc->desktopEntry = m_mime->findDesktopForExec(keys[0]);
                if(!m_byid.contains(pid))
                    m_byid.insert(pid, proc);

                beginInsertRows({}, m_flat.count()-1, m_flat.count()-1);
                m_flat.append(proc);
                m_flatpids.append(pid);
                endInsertRows();
            }
        }
    }

    // loop through our old PID list to purge
    for(auto &mp : oldpids)
    {
        if(!newpids.contains(mp))
        {
            int row = m_flatpids.indexOf(mp);
            beginRemoveRows({}, row, row);
            auto ls = m_flat.at(row);
            m_flat.removeAt(row);
            m_flatpids.removeAt(row);
            delete ls;
            endRemoveRows();
        }
    }

    m_initial = true;
    emit polled();
}

void LSProcfsModel::sortItems()
{
    /*switch(m_sort)
    {
    case ParentProcessId:
        m_flat.insert(QByteArray(proc->ppid), proc);
        break;
    case ProcessName:
        m_flatproc->processName, proc);
        break;
    case ProcessId:
    default:
        m_flat.insert(pid, proc);
        break;
    }*/
}

void LSProcfsModel::createDefaultColumnOrder()
{
    m_visibleColumnOrder.append(ColumnItem::ProcessId);
    m_visibleColumnOrder.append(ColumnItem::ProcessName);
    m_visibleColumnOrder.append(ColumnItem::UserName);
    m_visibleColumnOrder.append(ColumnItem::CPUTime);
    m_visibleColumnOrder.append(ColumnItem::ResidentMemory);
    m_visibleColumnOrder.append(ColumnItem::ProcessState);

}

QString LSProcfsModel::formatMemory(LSProcessItem *item) const
{
    return QLocale::system().formattedDataSize((item->rss-item->shmem)*m_pagesize);
}

QString LSProcfsModel::formatState(LSProcessItem::ProcessState state) const
{
    switch(state)
    {
    case LSProcessItem::PROC_RUNNING:
        return tr("Running");
        break;
    case LSProcessItem::PROC_SLEEPING:
        return tr("Sleeping");
        break;
    case LSProcessItem::PROC_SLEEP_UNINTERUPT:
        return tr("Sleeping (Uninterrupted)");
        break;
    case LSProcessItem::PROC_TRACED:
        return tr("Traced");
        break;
    case LSProcessItem::PROC_ZOMBIE:
        return tr("Zombie");
        break;
    case LSProcessItem::PROC_UNKNOWN:
    default:
        return tr("Unknown");
        break;
    }
}

QString LSProcfsModel::username(uid_t uid) const
{
    if(m_usercache.contains(uid))
        return m_usercache[uid];

    QString s;
    struct passwd *p;
    if((p = getpwuid(uid)) != nullptr)
    {
        s = QString(p->pw_name);
        m_usercache[uid] = s;
    }

    if(s.isEmpty())
        return QString("%1").arg(uid);

    return s;
}

LSProcessItem::LSProcessItem(uint pid)
{
    if(pid == 0)
        return;

    this->pid = pid;
}

bool LSProcessItem::process()
{
    QString fn = QString("/proc/%1/stat").arg(pid);
    if(!QFile::exists(fn))
        return false;

    QFile file(fn);
    file.open(QFile::ReadOnly);
    QString data = QString(file.readAll());
    file.close();

    struct stat statinfo;
    stat(fn.toUtf8().data(), &statinfo);

    // Filter out our title from stat data
    QRegularExpression regex("\\((.*)\\)");
    QRegularExpressionMatch cap = regex.match(data);
    if(cap.hasMatch() && cap.capturedTexts().count() == 2)
        processName = cap.capturedTexts().at(1);

    QStringList filteredData = data.remove(cap.capturedTexts().at(1)).split(" ");
    filteredData.removeFirst(); // process id
    filteredData.removeFirst(); // () (empty where processName was)

    switch(filteredData[0].toUtf8()[0])
    {
    case 'R':
        state = LSProcessItem::PROC_RUNNING;
        break;
    case 'S':
        state = LSProcessItem::PROC_SLEEPING;
        break;
    case 'D':
        state = LSProcessItem::PROC_SLEEP_UNINTERUPT;
        break;
    case 'Z':
        state = LSProcessItem::PROC_ZOMBIE;
        break;
    case 'T':
        state = LSProcessItem::PROC_RUNNING;
        break;
    default:
        state = LSProcessItem::PROC_UNKNOWN;
        break;
    }

    QFile mfile(QString("/proc/%1/statm").arg(pid));
    mfile.open(QFile::ReadOnly);
    QString mdata = QString(mfile.readAll());
    mfile.close();
    auto mFilteredData = mdata.split(' ');


    ppid = filteredData[1].toUInt();
    tty = filteredData[4];

    utime = filteredData[11].toInt();
    stime = filteredData[12].toInt();
    cutime = filteredData[13].toInt();
    cstime = filteredData[14].toInt();

    priority = filteredData[15].toInt();
    nice = filteredData[16].toInt();
    threads = filteredData[17].toULongLong();
    starttime = filteredData[19].toULongLong();
    vsize = filteredData[20].toULongLong();
    rss = filteredData[21].toULongLong();
    shmem = mFilteredData[2].toULongLong();
    user = statinfo.st_uid;

    QFile cmdline(QString("/proc/%1/cmdline").arg(pid));
    cmdline.open(QFile::ReadOnly);
    commandLine = cmdline.readAll().replace('\0', ' ').trimmed();
    cmdline.close();
    valid = true;

    return valid;
}

void LSProcessItem::secondProcess()
{

}
