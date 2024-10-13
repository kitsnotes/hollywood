/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#pragma once

#include <QObject>
#include <QMap>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QTimer>
#include <QStringList>

#define DBUS_SERVICE "org.freedesktop.UDisks2"
#define DBUS_PATH "/org/freedesktop/UDisks2"
#define DBUS_OBJMANAGER "org.freedesktop.DBus.ObjectManager"
#define DBUS_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_INTROSPECTABLE "org.freedesktop.DBus.Introspectable"
#define DBUS_DEVICE_ADDED "InterfacesAdded"
#define DBUS_DEVICE_REMOVED "InterfacesRemoved"

class LSUDiskDevice : public QObject
{
    Q_OBJECT
public:
    explicit LSUDiskDevice(const QString block, QObject *parent = nullptr);
    bool isLoopDevice() const;
    bool isOptical() const;
    bool hasOpticalMedia();
    bool canEject();
    bool hasMedia() const;
    bool hasPartition() const;
    bool removable() const;
    QString name() const;
    QString path() const;
    QString mountpoint() const;
    QString filesystem() const;
    QIcon icon() const;
    bool hintIgnore() const;
    bool isBlankDisc() const;
    QString currentMedia() const;
    QStringList supportedMedia() const;
    QString currentMediaDisplayName() const;
    QString devDevice() const;
    qulonglong blockDeviceSize() const;
signals:
    void mediaChanged(QString devicePath, bool mediaPresent);
    void mountpointChanged(QString devicePath, QString deviceMountpoint);
    void nameChanged(QString devicePath, QString deviceName);
    void errorMessage(QString devicePath, QString deviceError);
public slots:
    void mount();
    void unmount();
    void eject();
private slots:
    void updateDeviceProperties();
    void handlePropertiesChanged(const QString &interfaceType, const QMap<QString, QVariant> &changedProperties);
private:
    QVariant queryDriveProperty(const QString &property) const;
    QVariant queryBlockProperty(const QString &property) const;
    QVariant queryPartitionProperty(const QString &property) const;

    QString getMountPointOptical() const;
    QString getMountPoint() const;
    QString getDeviceName() const;
    QString mountDevice() const;
    QString mountOptical() const;
    QString unmountDevice() const;
    QString unmountOptical() const;
    QString ejectDevice() const;

    QDBusInterface *m_dbus;
    QString m_name = QString();
    QString m_path = QString();
    QString dev = QString();
    QString m_drive = QString();
    QStringList m_supportedMediaTypes = QStringList();
    QString m_mountpoint = QString();
    QString m_filesystem = QString();
    bool m_isOptical = false;
    bool m_removable = false;
    bool m_hasMedia = false;
    QString m_currentMediaType = QString();
    int m_opticalDataTracks = 0;
    int m_opticalAudioTracks = 0;
    bool m_isBlankDisc = false;
    bool m_hasPartition = false;
    bool m_hintIgnore = false;
    qulonglong m_blockSize = 0;
};

class LSUDisks : public QObject
{
    Q_OBJECT
public:
    explicit LSUDisks(QObject *parent = nullptr);
    QMap<QString, LSUDiskDevice*> devices;
    LSUDiskDevice* deviceForMountpath(const QString &mount);
    LSUDiskDevice* deviceForDevPath(const QString &mount);

private:
    QDBusInterface *dbus;
    QTimer timer;
private:
    static const QStringList getDevices();

signals:
    void updatedDevices();
    void mediaChanged(QString path, bool media);
    void mountpointChanged(QString path, QString mountpoint);
    void deviceErrorMessage(QString path, QString error);
    void foundNewDevice(QString path);
    void removedDevice(QString path);

private slots:
    void setupDBus();
    void scanDevices();
    void deviceAdded(const QDBusObjectPath &obj);
    void deviceRemoved(const QDBusObjectPath &obj);
    void handleDeviceMediaChanged(QString devicePath, bool mediaPresent);
    void handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint);
    void handleDeviceErrorMessage(QString devicePath, QString deviceError);
    void checkUDisks();

};
