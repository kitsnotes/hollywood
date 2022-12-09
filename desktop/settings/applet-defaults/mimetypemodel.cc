#include "mimetypemodel.h"
#include <QMimeDatabase>
#include <QMimeType>
#include <QIcon>
#include <desktopentry.h>

MimeTypeModel::MimeTypeModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_mimeapps(new LSMimeApplications(this))
{
    m_mimeapps->processGlobalMimeCache();
    QMimeDatabase mimedb;
    for(auto &mime : mimedb.allMimeTypes())
    {
        MimeTypeItem m;
        m.comment = mime.comment();
        m.icon = QIcon::fromTheme(mime.iconName(), QIcon::fromTheme(mime.genericIconName()));
        m.mimeType = mime.name();
        auto dt = m_mimeapps->defaultApp(m.mimeType);
        if(dt)
        {
            m.defaultAppName = dt->value("Name").toString();
            m.defaultAppDesktop = dt->fileName();
        }
        else
        {
            m.defaultAppName = "No application selected";
        }
        m_allMimes.append(m);
    }
}

QVariant MimeTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    switch(role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
            case 0: return tr("Type");
            case 1: return tr("MIME/URL");
            case 2: return tr("Default Application");
        }
        break;
    }

    return QVariant();
}

int MimeTypeModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_allMimes.count();
}

int MimeTypeModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return 3;
}

QVariant MimeTypeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto mt = m_allMimes.at(index.row());
    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
            case 0: qDebug() << mt.comment; return mt.comment;
            case 1: return mt.mimeType;
            case 2: return mt.defaultAppName;
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return mt.icon;
/*        if(index.column() == 2)
            return defaultAppIcon(mt.name());*/
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    }

    return QVariant();
}

