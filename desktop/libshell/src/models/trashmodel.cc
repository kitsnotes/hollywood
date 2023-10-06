#include "trashmodel.h"
#include "trashmodel_p.h"
#include "fsnode.h"
#include "fileinfo.h"
#include <QFileSystemWatcher>

LSTrashModelItem::LSTrashModelItem(const QString &trashfile)
{
    auto xdg_data = qgetenv("XDG_DATA_HOME");
    if(xdg_data.isEmpty())
        xdg_data = qgetenv("HOME")+"/.local/share/";
    auto trash_dir = xdg_data+"/Trash/";

    bool has_file = false;
    bool has_info = false;

    QFile check(trash_dir+"files/"+trashfile);
    if(check.exists())
        has_file = true;

    QFile icheck(trash_dir+"info/"+trashfile);
    if(icheck.exists())
    {
        QSettings settings(trash_dir+"info/"+trashfile);
        settings.beginGroup(QLatin1String("Trash Info"));
        m_origin = settings.value(QLatin1String("Path")).toString();
        auto dt = settings.value(QLatin1String("DeletionDate")).toString();
        if(!dt.isEmpty() && !m_origin.isEmpty())
            has_info = true;
    }

    if(has_info && has_file)
        m_valid = true;
}

bool LSTrashModelItem::operator ==(const LSTrashModelItem &ti) const
{
    return m_valid == ti.valid()
            && m_name == ti.fileName()
            && m_origin == ti.origin();
}


LSTrashModelPrivate::LSTrashModelPrivate(LSTrashModel *parent)
    : d(parent)
    , m_fileInfoGatherer(new QFileSystemWatcher(parent))
{
    auto xdg_data = qgetenv("XDG_DATA_HOME");
    if(xdg_data.isEmpty())
        xdg_data = qgetenv("HOME")+"/.local/share/";
    m_trashdir = QDir(xdg_data+"/Trash/");
    m_fileInfoGatherer->addPath(xdg_data+"/Trash/files/");
}

bool LSTrashModelPrivate::hasFile(const QString &file)
{
    for(auto f : m_items)
    {
        if(f.fileName() == file)
            return true;
    }
    return false;
}

int LSTrashModelPrivate::rowForFile(const QString &file)
{
    for(auto f : m_items)
    {
        if(f.fileName() == file)
            return m_items.indexOf(f);
    }
    return -1;
}

LSTrashModel::LSTrashModel(QObject *parent)
    : QAbstractListModel(parent)
    , p(new LSTrashModelPrivate(this))
{
    refreshTrashFolder();
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::directoryChanged, this, &LSTrashModel::refreshTrashFolder);
    connect(p->m_fileInfoGatherer, &QFileSystemWatcher::fileChanged, this, &LSTrashModel::refreshTrashFolder);
}

int LSTrashModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return p->m_items.count();
}

int LSTrashModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant LSTrashModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    switch(role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
            case 0: return tr("Name");
            case 1: return tr("Kind");
            case 2: return tr("Trash Time");
            case 3: return tr("Origin");
        }
        break;
    }

    return QVariant();

}

QVariant LSTrashModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
            case 0: return name(index);
            case 1: return description(index);
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return icon(index);
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    }

    return QVariant();
}

QIcon LSTrashModel::icon(const QModelIndex &index) const
{
    //return p->m_items.at(index.row()).icon();
    return QIcon();
}

QString LSTrashModel::name(const QModelIndex &index) const
{
    return p->m_items.at(index.row()).displayName();
    return QString("");
}

QString LSTrashModel::description(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QString("");
}

void LSTrashModel::refreshTrashFolder()
{
    auto olditems = p->m_items;
    QList<LSTrashModelItem> newFiles;
    QDir dir(p->m_trashdir.absolutePath()+"/files/");
    auto items = dir.entryInfoList(QDir::AllEntries|QDir::NoDotAndDotDot);
    for (const auto &i : items)
    {
        QString fileName = i.fileName();
        qDebug() << fileName;
        Q_ASSERT(!fileName.isEmpty());
        int row = p->rowForFile(fileName);
        if(!p->hasFile(fileName))
        {
            LSTrashModelItem info(fileName);
            if(info.valid())
            {
                int row = p->m_items.count()-1;
                beginInsertRows(QModelIndex(), row,row);
                newFiles.append(info);
                endInsertRows();
            }
        }
        else
        {
            newFiles.append(olditems[row]);
            emit dataChanged(index(row, 0), index(row, columnCount()-1));
        }
    }
    for(auto &mp : olditems)
    {
        if(!p->m_items.contains(mp))
        {
            int row = p->rowForFile(mp.fileName());
            beginRemoveRows({}, row, row);
            endRemoveRows();
        }
    }
    p->m_items = newFiles;
}
