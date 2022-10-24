#ifndef WPMODEL_H
#define WPMODEL_H

#include <QAbstractListModel>

class WallpaperModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit WallpaperModel(QString target, QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QModelIndex indexForItem(QString item);
    bool hasItem(QString item);
    QString selectedDirectory() const;
private:
    bool m_colors = false;
    QString m_directory;
    QList<QString> m_validFiles;
    QList<QPair<QString,QString>> m_colorList;
};

#endif //WPMODEL_H
