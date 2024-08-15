// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "appmodel.h"
#include "mimeapps.h"
#include "desktopentry.h"
#include "private/appmodel_p.h"

AppModelPrivate::AppModelPrivate(ApplicationModel *parent)
    : m_mimeapps(new LSMimeApplications(parent))
    , d(parent)
{}

AppModelPrivate::~AppModelPrivate() {}

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
    , p(new AppModelPrivate(this))
{
    p->m_mimeapps->cacheAllDesktops();
    p->m_allApps = p->m_mimeapps->allApps();
    for(auto app : p->m_allApps)
    {
        if(app->contains("X-Hollywood-System"))
            p->m_allApps.removeOne(app);

        if(app->contains("NoDisplay"))
            p->m_allApps.removeOne(app);
    }
}

QVariant ApplicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    switch(role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
            case 0: return tr("Application");
            case 1: return tr("Comment");
        }
        break;
    }

    return QVariant();
}

int ApplicationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return p->m_allApps.count();
}

int ApplicationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    LSDesktopEntry *d = p->m_allApps.at(index.row());
    if(!d)
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
            case 0: return d->value("Name");
            case 1: return d->value("Comment");
        }
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return !d->icon().isNull() ? d->icon() : QIcon::fromTheme("application-x-executable");
        break;
    case Qt::TextAlignmentRole:
        if (index.column() == 1)
            return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
        break;
    case Qt::UserRole+1:
        return d->fileName();
    }

    return QVariant();
}
