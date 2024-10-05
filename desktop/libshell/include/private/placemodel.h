#pragma once

#include <QStandardItemModel>
#include <QStandardItem>
#include "libshell_int.h"

class LSPlaceModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum PlaceType
    {
        FileSystem,
        NetworkLocation,
        Applications,
        MountedVolume,
        Tag,
        Trash
    };
    struct PlaceItem
    {
        QString displayName;
        QIcon displayIcon;
        QUrl location;
        PlaceType type;
    };
    explicit LSPlaceModel(QObject *parent = nullptr);
    bool hasPlaceForUrl(const QUrl &url) const;
    QModelIndex indexForPlace(PlaceItem) const;
    PlaceItem place(const QUrl &url);
    PlaceItem place(const QModelIndex &idx);
private:
    PlaceItem constructLocalPlace(const QString &path);
    void populateFavorites();
private:
    QStandardItem *m_favoriteRoot;
    QList<PlaceItem> m_favorites;
    QList<QStandardItem*> m_favoriteItems;
};
