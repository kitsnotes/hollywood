#ifndef APPLICATIONMODEL_H
#define APPLICATIONMODEL_H

#include <QAbstractItemModel>

class AppModelPrivate;
class ApplicationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ApplicationModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
private:
    AppModelPrivate *p;
};

#endif // APPLICATIONMODEL_H
