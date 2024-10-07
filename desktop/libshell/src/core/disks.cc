/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "disks.h"
#include "udisks2.h"
#include <QDebug>
#include <QTimer>
#include <QtDBus/QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QXmlStreamReader>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QTextStream>

LSUDiskDevice::LSUDiskDevice(const QString block, QObject *parent)
    : QObject(parent)
    , path(block)
    , isOptical(false)
    , isRemovable(false)
    , hasMedia(false)
    , opticalDataTracks(0)
    , opticalAudioTracks(0)
    , isBlankDisc(false)
    , hasPartition(false)
    , dbus(0)
{
    QDBusConnection system = QDBusConnection::systemBus();
    dbus = new QDBusInterface(DBUS_SERVICE, path, QString("%1.Block").arg(DBUS_SERVICE), system, parent);
    system.connect(dbus->service(), dbus->path(), DBUS_PROPERTIES, "PropertiesChanged", this, SLOT(handlePropertiesChanged(QString,QMap<QString,QVariant>)));
    updateDeviceProperties();
}

void LSUDiskDevice::mount()
{
    qDebug() << "mount?" << path << mountpoint;
    if (!dbus->isValid() || !mountpoint.isEmpty()) { return; }
    QString reply;
    if (isOptical) {
        reply = uDisks2::mountOptical(path);
        qDebug() << "optical=" << uDisks2::getMountPointOptical(path);
    }
    else { reply = uDisks2::mountDevice(path); }
    if (!reply.isEmpty()) {
        emit errorMessage(path, reply);
        return;
    }
    updateDeviceProperties();
}

void LSUDiskDevice::unmount()
{
    if (!dbus->isValid() || (mountpoint.isEmpty() && !isOptical)) { return; }
    QString reply;
    if (isOptical) { reply = uDisks2::unmountOptical(path); }
    else { reply = uDisks2::unmountDevice(path); }
    updateDeviceProperties();
    if (!reply.isEmpty() || (!mountpoint.isEmpty())) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to umount %1").arg(name); }
        emit errorMessage(path, reply);
        return;
    }
    if (isOptical) { eject(); }
}

void LSUDiskDevice::eject()
{
    if (!dbus->isValid()) { return; }
    QString reply = uDisks2::ejectDevice(drive);
    updateDeviceProperties();
    if (!reply.isEmpty()/* || hasMedia*/) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to eject %1").arg(name); }
        emit errorMessage(path, reply);
    }
}

void LSUDiskDevice::updateDeviceProperties()
{
    if (!dbus->isValid()) { return; }

    bool hadMedia =  hasMedia;
    QString lastMountpoint = mountpoint;
    QString lastName = name;

    drive = uDisks2::getDrivePath(path);
    name = uDisks2::getDeviceLabel(path);

    if (name.isEmpty()) { name = uDisks2::getDeviceName(drive); }
    if (name.isEmpty()) { name = QObject::tr("Storage"); }

    dev = path.split("/").takeLast();
    isRemovable = uDisks2::isRemovable(drive);
    filesystem = uDisks2::getFileSystem(path);
    isOptical = uDisks2::isOptical(drive);
    if (isOptical) {
        mountpoint = uDisks2::getMountPointOptical(path);
    } else {
        mountpoint = uDisks2::getMountPoint(path);
    }
    hasMedia = uDisks2::hasMedia(drive);
    opticalDataTracks = uDisks2::opticalDataTracks(drive);
    opticalAudioTracks = uDisks2::opticalAudioTracks(drive);
    isBlankDisc = uDisks2::opticalMediaIsBlank(drive);
    hasPartition = uDisks2::hasPartition(path);

    if (hadMedia != hasMedia) {
        emit mediaChanged(path, hasMedia);
    }
    if (lastMountpoint != mountpoint) {
        emit mountpointChanged(path, mountpoint);
    }
    if (lastName != name) {
        emit nameChanged(path, name);
    }
}

void LSUDiskDevice::handlePropertiesChanged(const QString &interfaceType, const QMap<QString, QVariant> &changedProperties)
{
    Q_UNUSED(interfaceType)
    Q_UNUSED(changedProperties)
    updateDeviceProperties();
}

LSUDisks::LSUDisks(QObject *parent)
    : QObject(parent)
    , dbus(Q_NULLPTR)
{
    setupDBus();
    timer.setInterval(60000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkUDisks()));
    timer.start();
}

void LSUDisks::setupDBus()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_ADDED, this, SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, DBUS_DEVICE_REMOVED, this, SLOT(deviceRemoved(const QDBusObjectPath&)));
        if (dbus==Q_NULLPTR) { dbus = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_OBJMANAGER, system); } // only used to verify the UDisks is running
        scanDevices();
    }
}

void LSUDisks::scanDevices()
{
    if (!dbus) { return; }
    if (!dbus->isValid()) { return; }
    QStringList foundDevices = uDisks2::getDevices();
    for (int i=0;i<foundDevices.size();i++) {
        QString foundDevicePath = foundDevices.at(i);
        bool hasDevice = devices.contains(foundDevicePath);
        if (hasDevice) { continue; }
        LSUDiskDevice *newDevice = new LSUDiskDevice(foundDevicePath, this);
        connect(newDevice, SIGNAL(mediaChanged(QString,bool)), this, SLOT(handleDeviceMediaChanged(QString,bool)));
        connect(newDevice, SIGNAL(mountpointChanged(QString,QString)), this, SLOT(handleDeviceMountpointChanged(QString,QString)));
        connect(newDevice, SIGNAL(errorMessage(QString,QString)), this, SLOT(handleDeviceErrorMessage(QString,QString)));
        devices[foundDevicePath] = newDevice;
    }
    emit updatedDevices();
}

void LSUDisks::deviceAdded(const QDBusObjectPath &obj)
{
    if (!dbus) { return; }
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    scanDevices();
    emit foundNewDevice(path);
}

void LSUDisks::deviceRemoved(const QDBusObjectPath &obj)
{
    if (!dbus) { return; }
    if (!dbus->isValid()) { return; }
    QString path = obj.path();
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString("%1/jobs").arg(DBUS_PATH))) { return; }

    if (deviceExists) {
        if (uDisks2::getDevices().contains(path)) { return; }
        delete devices.take(path);
    }
    scanDevices();
    emit removedDevice(path);
}

void LSUDisks::handleDeviceMediaChanged(QString devicePath, bool mediaPresent)
{
    emit mediaChanged(devicePath, mediaPresent);
}

void LSUDisks::handleDeviceMountpointChanged(QString devicePath, QString deviceMountpoint)
{
    emit mountpointChanged(devicePath, deviceMountpoint);
}

void LSUDisks::handleDeviceErrorMessage(QString devicePath, QString deviceError)
{
    emit deviceErrorMessage(devicePath, deviceError);
}

void LSUDisks::checkUDisks()
{
    if (!QDBusConnection::systemBus().isConnected()) {
        setupDBus();
        //return;
    }
    if (!dbus) { return; }
    if (dbus->isValid()) { scanDevices(); }
}

const QString uDisks2::getDrivePath(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Block").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QString(); }
    QDBusObjectPath drive = iface.property("Drive").value<QDBusObjectPath>();
    return drive.path();
}

bool uDisks2::hasPartition(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Partition").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    if (iface.property("Number").toUInt()>=1) { return true; }
    return false;
}

const QString uDisks2::getFileSystem(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Block").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QString(); }
    return iface.property("IdType").toString();
}

bool uDisks2::isRemovable(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("Removable").toBool();
}

bool uDisks2::isOptical(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QStringList compat = iface.property("MediaCompatibility").toStringList();
    for (int i=0;i<compat.size();i++) {
        if (compat.at(i).startsWith("optical_")) { return true; }
    }
    return false;
}

bool uDisks2::hasMedia(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("MediaAvailable").toBool();
}

bool uDisks2::hasOpticalMedia(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QString type = iface.property("Media").toString();
    if (type.startsWith("optical_")) { return true; }
    return false;
}

bool uDisks2::canEject(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("Ejectable").toBool();
}

bool uDisks2::opticalMediaIsBlank(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("OpticalBlank").toBool();
}

int uDisks2::opticalDataTracks(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("OpticalNumDataTracks").toBool();
}

int uDisks2::opticalAudioTracks(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    return iface.property("OpticalNumAudioTracks").toBool();
}

const QString uDisks2::getMountPointOptical(QString path)
{
    QString mountpoint;
    QString device = path.split("/").takeLast();
    if (device.isEmpty()) { return mountpoint; }
    QFile mtabFile("/etc/mtab");
    if (!mtabFile.open(QIODevice::ReadOnly)) { return QString(); }
    QString mtab = mtabFile.readAll();
    QStringList lines = mtab.split("\n");
    QVector<QStringList> result;
    for (int i=0;i<lines.size();++i) {
        QString line = lines.at(i);
        QStringList info = line.split(" ", Qt::SkipEmptyParts);
        if (info.size()>=2) {
            QString dev = info.at(0);
            QString mnt = info.at(1);
            if (dev == QString("/dev/%1").arg(device)) { mountpoint = mnt; }
        }
    }
    mtabFile.close();
    return mountpoint;
}

const QString uDisks2::getMountPoint(QString path)
{
    QString mountpoint;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_SERVICE,
                                                          path,
                                                          DBUS_PROPERTIES,
                                                          "Get");
    QList<QVariant> args;
    args << QString("%1.Filesystem").arg(DBUS_SERVICE) << "MountPoints";
    message.setArguments(args);
    QDBusMessage reply = QDBusConnection::systemBus().call(message);
    QStringList mountpoints;
    QList<QByteArray> argList;
    foreach (QVariant arg, reply.arguments()) {
        if (!arg.value<QDBusVariant>().variant().isValid()) { continue; }
        arg.value<QDBusVariant>().variant().value<QDBusArgument>() >> argList;
    }
    foreach (QByteArray point, argList) { mountpoints.append(point); }
    mountpoint = mountpoints.join("");
    return mountpoint;
}

const QString uDisks2::getDeviceName(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QString(); }
    QString name = iface.property("Vendor").toString().simplified();
    if (!name.isEmpty()) { name.append(" "); }
    name.append(iface.property("Model").toString().simplified());
    return name;
}

const QString uDisks2::getDeviceLabel(QString path)
{
    QDBusInterface iface(DBUS_SERVICE,
                         path,
                         QString("%1.Block").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QString(); }
    return iface.property("IdLabel").toString();
}

const QString uDisks2::mountDevice(QString path)
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              path,
                              QString("%1.Filesystem").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QVariantMap options;
    if (getFileSystem(path) == "vfat") { options.insert("options", "flush"); }
    QDBusReply<QString> mountpoint =  filesystem.call("Mount", options);
    return mountpoint.error().message();
}

const QString uDisks2::mountOptical(QString path)
{
    // something is broken somewhere in udev/udisk, whatever ...
    // So we need to handle opticals using udisks cmd
    // https://bugs.archlinux.org/task/49643
    // https://bugs.freedesktop.org/show_bug.cgi?id=52357
    QProcess proc;
    proc.setProgram(QString("udisks"));
    proc.setArguments(QStringList() << "--mount" << QString("/dev/%1").arg((path.split("/").takeLast())));
    proc.start();
    proc.waitForFinished();
    return QString();
}

const QString uDisks2::unmountDevice(QString path)
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              path,
                              QString("%1.Filesystem").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = filesystem.call("Unmount", QVariantMap());
    return reply.arguments().first().toString();
}

const QString uDisks2::unmountOptical(QString path)
{
    // something is broken somewhere in udev/udisk, whatever ...
    // So we need to handle opticals using udisks cmd
    // https://bugs.archlinux.org/task/49643
    // https://bugs.freedesktop.org/show_bug.cgi?id=52357
    QProcess proc;
    proc.setProgram(QString("udisks"));
    proc.setArguments(QStringList() << "--unmount" << QString("/dev/%1").arg((path.split("/").takeLast())));
    proc.start();
    proc.waitForFinished();
    return QString();
}

const QString uDisks2::ejectDevice(QString path)
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              path,
                              QString("%1.Drive").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = filesystem.call("Eject", QVariantMap());
    return reply.arguments().first().toString();
}

const QStringList uDisks2::getDevices()
{
    QStringList result;
    QDBusMessage call = QDBusMessage::createMethodCall(DBUS_SERVICE,
                                                       QString("%1/block_devices").arg(DBUS_PATH),
                                                       DBUS_INTROSPECTABLE,
                                                       "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
    QList<QDBusObjectPath> devices;
    if (reply.isError()) { return result; }
    QXmlStreamReader xml(reply.value());
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "node" ) {
            QString name = xml.attributes().value("name").toString();
            if(!name.isEmpty()) { devices << QDBusObjectPath("/org/freedesktop/UDisks2/block_devices/" + name); }
        }
    }
    foreach (QDBusObjectPath device, devices) {
        result << device.path();
    }
    return result;
}
