#pragma once

#include <QAbstractListModel>

class LSUDiskDevice;
class LSDiskModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LSDiskModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    LSUDiskDevice* device(const QModelIndex &index);
    LSUDiskDevice* deviceForPath(const QString &path);
    LSUDiskDevice* deviceForMount(const QString &mount);
    QModelIndex indexOf(LSUDiskDevice* dev) const;
private slots:
    void mediaChanged(QString path, bool media);
    void mountpointChanged(QString path, QString mountpoint);
    void foundNewDevice(QString path);
    void removedDevice(QString path);
private:
    void initialPopulation();
    QString driveDisplayName(const LSUDiskDevice* dev) const;
    QIcon driveIcon(const LSUDiskDevice* dev) const;
private:
    QList<LSUDiskDevice*> m_devices;
};
