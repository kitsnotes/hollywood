// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#include "placemodel.h"

LSPlaceModel::LSPlaceModel(QObject *parent)
    : QStandardItemModel(parent),
      m_favoriteRoot(new QStandardItem(tr("Favorites")))
{
    m_favoriteRoot->setSelectable(false);
    appendRow(m_favoriteRoot);
    populateFavorites();
    for(PlaceItem pi : m_favorites)
    {
        QStandardItem *item = new QStandardItem(pi.displayIcon, pi.displayName);
        item->setData(pi.location);
        m_favoriteRoot->appendRow(item);
        m_favoriteItems.append(item);
    }
}

bool LSPlaceModel::hasPlaceForUrl(const QUrl &url) const
{
    for(auto pi : m_favorites)
    {
        if(pi.location == url)
            return true;
    }

    return false;
}

LSPlaceModel::PlaceItem LSPlaceModel::place(const QUrl &url)
{
    PlaceItem pi;
    for(auto item : m_favorites)
    {
        if(item.location == url)
            pi = item;
    }

    return pi;
}

LSPlaceModel::PlaceItem LSPlaceModel::place(const QModelIndex &idx)
{
    auto chkItem = itemFromIndex(idx);
    PlaceItem pi;
    for(auto item : m_favorites)
    {
        if(item.location == chkItem->data())
            pi = item;
    }

    return pi;
}

QModelIndex LSPlaceModel::indexForPlace(PlaceItem pi) const
{
    for(auto item : m_favoriteItems)
    {
        if(item->data() == pi.location)
            return indexFromItem(item);
    }

    return QModelIndex();
}

LSPlaceModel::PlaceItem LSPlaceModel::constructLocalPlace(const QString &path)
{
    PlaceItem pi;
    pi.location = QUrl::fromLocalFile(path);
    pi.type = FileSystem;
    QDir dir(path);
    pi.displayName = dir.dirName();

    return pi;
}

void LSPlaceModel::populateFavorites()
{
    PlaceItem pia;
    pia.location = QUrl("applications://");
    pia.type = Applications;
    pia.displayName = tr("Applications");
    pia.displayIcon = QIcon::fromTheme("folder-activities");

    m_favorites.append(pia);

    PlaceItem pi;

    pi = constructLocalPlace(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());
    pi.displayIcon = QIcon::fromTheme("folder-documents");
    m_favorites.append(pi);
    pi = constructLocalPlace(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first());
    pi.displayIcon = QIcon::fromTheme("folder-downloads");
    m_favorites.append(pi);
    pi = constructLocalPlace(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    pi.displayIcon = QIcon::fromTheme("user-desktop");
    m_favorites.append(pi);
    pi = constructLocalPlace(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());
    pi.displayIcon = QIcon::fromTheme("user-home");
    m_favorites.append(pi);

    PlaceItem pit;
    pit.location = QUrl("trash://");
    pit.type = Trash;
    pit.displayName = tr("Trash");
    pit.displayIcon = QIcon::fromTheme("user-trash");

    m_favorites.append(pit);

}
