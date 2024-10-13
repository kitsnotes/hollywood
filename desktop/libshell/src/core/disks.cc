/*
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "disks.h"
#include <QDebug>
#include <QTimer>
#include <QtDBus/QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusInterface>
#include <QXmlStreamReader>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QMap>

static QMap<QString,QString> g_device_names{{"thumb", "USB Flash Drive"},
                                            {"flash", "Flash Card"},
                                            {"flash_cf", "CompactFlash Card"},
                                            {"flash_ms", "MemoryStick"},
                                            {"flash_sm", "SmartMedia"},
                                            {"flash_sd", "Secure Digital"},
                                            {"flash_sdhc", "Secure Digital (High Capacity)"},
                                            {"flash_sdxc", "Secure Digital (eXtended Capacity)"},
                                            {"flash_sdio", "Secure Digital (Input Output)"},
                                            {"flash_sd_combo", "Secure Digital (IO/Storage Combo)"},
                                            {"flash_mmc", "MultiMediaCard"},
                                            {"floppy", "Floppy Disk"},
                                            {"floppy_zip", "Zip Disk"},
                                            {"floppy_jaz", "Jaz Disk"},
                                            {"optical", "Optical Disc"},
                                            {"optical_cd", "CD-ROM"},
                                            {"optical_cd_r", "CD-R"},
                                            {"optical_cd_rw", "CD-RW"},
                                            {"optical_dvd", "DVD-ROM"},
                                            {"optical_dvd_r", "DVD-R"},
                                            {"optical_dvd_rw", "DVD-RW"},
                                            {"optical_dvd_ram", "DVD-RAM"},
                                            {"optical_dvd_plus_r", "DVD+R"},
                                            {"optical_dvd_plus_rw", "DVD+RW"},
                                            {"optical_dvd_plus_r_dl", "DVD+R (Dual Layer)"},
                                            {"optical_dvd_plus_rw_dl", "DVD+RW (Dual Layer)"},
                                            {"optical_bd", "BD-ROM"},
                                            {"optical_bd_r", "BD-R"},
                                            {"optical_bd_re", "BD-RE"},
                                            {"optical_hddvd", "HD-DVD"},
                                            {"optical_hddvd_r", "HD-DVD (Recordable}"},
                                            {"optical_hddvd_rw", "HD-DVD (Rewritable)"},
                                            {"optical_mo", "Magneto Optical"},
                                            {"optical_mrw", "Can read Mount Rainer media"},
                                            {"optical_mrw_w", "Can write Mount Rainer media"}};

LSUDiskDevice::LSUDiskDevice(const QString block, QObject *parent)
    : QObject(parent)
    , m_dbus(nullptr)
    , m_path(block)
    , m_isOptical(false)
    , m_removable(false)
    , m_hasMedia(false)
    , m_opticalDataTracks(0)
    , m_opticalAudioTracks(0)
    , m_isBlankDisc(false)
    , m_hasPartition(false)
{
    QDBusConnection system = QDBusConnection::systemBus();
    m_dbus = new QDBusInterface(DBUS_SERVICE, m_path, QString("%1.Block").arg(DBUS_SERVICE), system, parent);
    system.connect(m_dbus->service(), m_dbus->path(), DBUS_PROPERTIES, "PropertiesChanged", this, SLOT(handlePropertiesChanged(QString,QMap<QString,QVariant>)));
    updateDeviceProperties();
}

bool LSUDiskDevice::isLoopDevice() const
{
    if(m_path.isEmpty())
        return false;
    QStringList pathparts = path().split("/");
    auto devlst = pathparts.takeLast();
    auto type = pathparts.takeLast();
    if(type == "block_devices"
        && devlst.startsWith("loop"))
        return true;

    return false;
}

QVariant LSUDiskDevice::queryDriveProperty(const QString &property) const
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_drive,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());

    if (!iface.isValid())
        return QVariant();
    return iface.property(property.toUtf8());
}

QVariant LSUDiskDevice::queryBlockProperty(const QString &property) const
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_path,
                         QString("%1.Block").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());

    if (!iface.isValid())
        return QVariant();
    return iface.property(property.toUtf8());
}

QVariant LSUDiskDevice::queryPartitionProperty(const QString &property) const
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_path,
                         QString("%1.Partition").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());

    if (!iface.isValid())
        return QVariant();
    return iface.property(property.toUtf8());
}

void LSUDiskDevice::mount()
{
    if (!m_dbus->isValid() || !m_mountpoint.isEmpty()) { return; }
    QString reply;
    if (m_isOptical) {
        reply = mountOptical();
    }
    else { reply = mountDevice(); }
    if (!reply.isEmpty()) {
        emit errorMessage(m_path, reply);
        return;
    }
    updateDeviceProperties();
}

void LSUDiskDevice::unmount()
{
    if (!m_dbus->isValid() || (m_mountpoint.isEmpty() && !m_isOptical)) { return; }
    QString reply;
    if (m_isOptical) { reply = unmountOptical(); }
    else { reply = unmountDevice(); }
    updateDeviceProperties();
    if (!reply.isEmpty() || (!m_mountpoint.isEmpty())) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to umount %1").arg(m_name); }
        emit errorMessage(m_path, reply);
        return;
    }
    if (m_isOptical) { eject(); }
}

void LSUDiskDevice::eject()
{
    if (!m_dbus->isValid()) { return; }
    QString reply = ejectDevice();
    updateDeviceProperties();
    if (!reply.isEmpty()/* || hasMedia*/) {
        if (reply.isEmpty()) { reply = QObject::tr("Failed to eject %1").arg(m_name); }
        emit errorMessage(m_path, reply);
    }
}

void LSUDiskDevice::updateDeviceProperties()
{
    if (!m_dbus->isValid()) { return; }

    bool hadMedia =  m_hasMedia;
    QString lastMountpoint = m_mountpoint;
    QString lastName = m_name;

    auto driveObj = queryBlockProperty("Drive");
    QDBusObjectPath drive = driveObj.value<QDBusObjectPath>();
    m_drive = drive.path();
    m_name = queryBlockProperty("IdLabel").toString();

    if (m_name.isEmpty()) { m_name = getDeviceName(); }
    if (m_name.isEmpty()) { m_name = QObject::tr("Storage"); }

    dev = m_path.split("/").takeLast();
    m_removable = queryDriveProperty("Removable").toBool();
    m_filesystem = queryBlockProperty("IdType").toString();
    m_isOptical = isOptical();
    m_mountpoint = getMountPoint();

    m_hintIgnore = queryBlockProperty("HintIgnore").toBool();
    m_hasMedia = queryDriveProperty("MediaAvailable").toBool();
    m_currentMediaType = queryDriveProperty("Media").toString();
    m_supportedMediaTypes = queryDriveProperty("MediaCompatibility").toStringList();
    m_opticalDataTracks = queryDriveProperty("OpticalNumDataTracks").toInt();
    m_opticalAudioTracks = queryDriveProperty("OpticalNumAudioTracks").toInt();
    m_isBlankDisc = queryDriveProperty("OpticalBlank").toBool();
    m_hasPartition = queryBlockProperty("Number").toUInt() >= 1 ? true : false;
    m_blockSize = queryBlockProperty("Size").toULongLong();

    if (hadMedia != m_hasMedia)
        emit mediaChanged(m_path, m_hasMedia);

    if (lastMountpoint != m_mountpoint)
        emit mountpointChanged(m_path, m_mountpoint);

    if (lastName != m_name)
        emit nameChanged(m_path, m_name);
}

void LSUDiskDevice::handlePropertiesChanged(const QString &interfaceType, const QMap<QString, QVariant> &changedProperties)
{
    Q_UNUSED(interfaceType)
    Q_UNUSED(changedProperties)
    updateDeviceProperties();
}

bool LSUDiskDevice::isOptical() const
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_drive,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QStringList compat = iface.property("MediaCompatibility").toStringList();
    for (int i=0;i<compat.size();i++) {
        if (compat.at(i).startsWith("optical_")) { return true; }
    }
    return false;
}

bool LSUDiskDevice::hasOpticalMedia()
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_drive,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QString type = iface.property("Media").toString();
    if (type.startsWith("optical_")) { return true; }
    return false;
}

bool LSUDiskDevice::canEject()
{
    return queryDriveProperty("Ejectable").toBool();
}

bool LSUDiskDevice::hasMedia() const { return m_hasMedia; }

bool LSUDiskDevice::hasPartition() const { return m_hasPartition; }

bool LSUDiskDevice::removable() const { return m_removable; }

QString LSUDiskDevice::name() const
{
    if(isOptical() && isBlankDisc())
    {
        auto type = tr("Disc");
        if(g_device_names.contains(m_currentMediaType))
            type = g_device_names[m_currentMediaType];
        return tr("Blank %1").arg(type);
    }

    return m_name;
}

QString LSUDiskDevice::path() const { return m_path; }

QString LSUDiskDevice::mountpoint() const { return m_mountpoint; }

QString LSUDiskDevice::filesystem() const { return m_filesystem; }

QIcon LSUDiskDevice::icon() const
{
    // ISO 9660/UDF loop device ISO mount
    if(isLoopDevice() && (filesystem() == "iso9660" || filesystem() == "udf"))
        return QIcon::fromTheme("application-x-cd-image");

    if(isOptical())
    {
        if(hasMedia())
        {
            if(isBlankDisc())
                return QIcon::fromTheme("media-optical-recordable", QIcon::fromTheme("media-optical"));
            else
            {
                // TODO: see if we have audio / video
                return QIcon::fromTheme("media-optical-data");
            }
        }
        else
        {
            // drive is empty - show a icon corresponding to the drive type
            auto media = supportedMedia();
            // icon preference: bluray/hddvd - dvd - cd
            if(media.contains("optical_bd") || media.contains("optical_hddvd"))
                return QIcon::fromTheme("media-optical-blu-ray", QIcon::fromTheme("media-optical"));

            if(media.contains("optical_dvd"))
                return QIcon::fromTheme("media-optical-dvd", QIcon::fromTheme("media-optical"));

            return QIcon::fromTheme("media-optical");
        }
    }

    if(mountpoint() == "/")
        return QIcon::fromTheme("drive-harddisk-root",
                                QIcon::fromTheme("drive-harddisk"));

    if(removable())
        return QIcon::fromTheme("drive-removable-media");

    return QIcon::fromTheme("drive-harddisk");
}

bool LSUDiskDevice::hintIgnore() const { return m_hintIgnore; }

bool LSUDiskDevice::isBlankDisc() const
{
    return m_isBlankDisc;
}

QString LSUDiskDevice::currentMedia() const
{
    return m_currentMediaType;
}

QStringList LSUDiskDevice::supportedMedia() const
{
    return m_supportedMediaTypes;
}

QString LSUDiskDevice::currentMediaDisplayName() const
{
    if(!m_currentMediaType.isEmpty())
    {
        if(g_device_names.contains(m_currentMediaType))
            return g_device_names[m_currentMediaType];
        else // we should never get to this point
            return tr("Unknown UDisk2 Device Type");
    }

    if(mountpoint() == "/")
        return tr("System Partition");

    if(mountpoint() == "/efi" ||
        mountpoint() == "/boot/efi")
        return tr("EFI System Partition");

    return m_currentMediaType;
}

QString LSUDiskDevice::devDevice() const
{
    if(m_path.isEmpty())
        return QString();

    auto dev = m_path.split("/").takeLast();
    return QString("/dev/%1").arg(dev);
}

qulonglong LSUDiskDevice::blockDeviceSize() const
{
    return m_blockSize;
}

QString LSUDiskDevice::getMountPointOptical() const
{
    QString mountpoint;
    QString device = m_path.split("/").takeLast();
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
    mountpoint.remove('\u0000');
    return mountpoint;
}

QString LSUDiskDevice::getMountPoint() const
{
    QString mountpoint;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_SERVICE,
                                                          m_path,
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
    // TODO: fix ugly hack
    mountpoint.remove('\u0000');
    return mountpoint;
}

QString LSUDiskDevice::getDeviceName() const
{
    QDBusInterface iface(DBUS_SERVICE,
                         m_drive,
                         QString("%1.Drive").arg(DBUS_SERVICE),
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QString(); }
    QString name = iface.property("Vendor").toString().simplified();
    if (!name.isEmpty()) { name.append(" "); }
    name.append(iface.property("Model").toString().simplified());
    return name;
}

QString LSUDiskDevice::mountDevice() const
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              m_path,
                              QString("%1.Filesystem").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QVariantMap options;

    auto fs = queryBlockProperty("IdType").toString();
    if (fs == "vfat") { options.insert("options", "flush"); }
    QDBusReply<QString> mountpoint =  filesystem.call("Mount", options);
    return mountpoint.error().message();
}

QString LSUDiskDevice::mountOptical() const
{
    // something is broken somewhere in udev/udisk, whatever ...
    // So we need to handle opticals using udisks cmd
    // https://bugs.archlinux.org/task/49643
    // https://bugs.freedesktop.org/show_bug.cgi?id=52357

    QProcess proc;
    proc.setProgram(QString("udisksctl"));
    proc.setArguments(QStringList() << "mount" << "-b" << QString("/dev/%1").arg((m_path.split("/").takeLast())));
    proc.start();
    proc.waitForFinished();
    return QString();
}

QString LSUDiskDevice::unmountDevice() const
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              m_path,
                              QString("%1.Filesystem").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = filesystem.call("Unmount", QVariantMap());
    return reply.arguments().first().toString();
}

QString LSUDiskDevice::unmountOptical() const
{
    // something is broken somewhere in udev/udisk, whatever ...
    // So we need to handle opticals using udisks cmd
    // https://bugs.archlinux.org/task/49643
    // https://bugs.freedesktop.org/show_bug.cgi?id=52357
    QProcess proc;
    proc.setProgram(QString("udisks2"));
    proc.setArguments(QStringList() << "unmount" << "-b" << QString("/dev/%1").arg((m_path.split("/").takeLast())));
    proc.start();
    proc.waitForFinished();
    return QString();
}

QString LSUDiskDevice::ejectDevice() const
{
    QDBusInterface filesystem(DBUS_SERVICE,
                              m_drive,
                              QString("%1.Drive").arg(DBUS_SERVICE),
                              QDBusConnection::systemBus());
    if (!filesystem.isValid()) { return QObject::tr("Failed D-Bus connection."); }
    QDBusMessage reply = filesystem.call("Eject", QVariantMap());
    return reply.arguments().first().toString();
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

LSUDiskDevice *LSUDisks::deviceForMountpath(const QString &mount)
{
    for(auto dev : devices.values())
    {
        if(dev->mountpoint() == mount)
            return dev;
    }

    return nullptr;
}

LSUDiskDevice *LSUDisks::deviceForDevPath(const QString &mount)
{
    for(auto dev : devices.values())
    {
        if(dev->devDevice() == mount)
            return dev;
    }

    return nullptr;
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
    QStringList foundDevices = getDevices();
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

const QStringList LSUDisks::getDevices()
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
        if (getDevices().contains(path)) { return; }
        emit removedDevice(path);
        delete devices.take(path);
    }
    scanDevices();
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



