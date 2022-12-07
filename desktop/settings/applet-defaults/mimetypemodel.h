#ifndef MIMETYPEMODEL_H
#define MIMETYPEMODEL_H

#include <QAbstractListModel>
#include <QMimeType>
#include <mimeapps.h>

struct MimeTypeItem
{
    QIcon icon;
    QString mimeType;
    QString comment;
    QString defaultAppDesktop;
    QString defaultAppName;
};

class MimeTypeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MimeTypeModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QList<MimeTypeItem> m_allMimes;
    LSMimeApplications *m_mimeapps;
};

#endif // MIMETYPEMODEL_H
