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

class uDisks2
{
public:
    static const QString getDrivePath(QString path);
    static bool hasPartition(QString path);
    static const QString getFileSystem(QString path);
    static bool isRemovable(QString path);
    static bool isOptical(QString path);
    static bool hasMedia(QString path);
    static bool hasOpticalMedia(QString path);
    static bool canEject(QString path);
    static bool opticalMediaIsBlank(QString path);
    static int opticalDataTracks(QString path);
    static int opticalAudioTracks(QString path);
    static const QString getMountPointOptical(QString path);
    static const QString getMountPoint(QString path);
    static const QString getDeviceName(QString path);
    static const QString getDeviceLabel(QString path);
    static const QString mountDevice(QString path);
    static const QString mountOptical(QString path);
    static const QString unmountDevice(QString path);
    static const QString unmountOptical(QString path);
    static const QString ejectDevice(QString path);
    static const QStringList getDevices();
};

class LSUDiskDevice : public QObject
{
    Q_OBJECT

public:
    explicit LSUDiskDevice(const QString block, QObject *parent = Q_NULLPTR);
    QString name;
    QString path;
    QString dev;
    QString drive;
    QString mountpoint;
    QString filesystem;
    bool isOptical;
    bool isRemovable;
    bool hasMedia;
    int opticalDataTracks;
    int opticalAudioTracks;
    bool isBlankDisc;
    bool hasPartition;

private:
    QDBusInterface *dbus;

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
};

class LSUDisks : public QObject
{
    Q_OBJECT

public:
    explicit LSUDisks(QObject *parent = Q_NULLPTR);
    QMap<QString, LSUDiskDevice*> devices;

private:
    QDBusInterface *dbus;
    QTimer timer;

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
