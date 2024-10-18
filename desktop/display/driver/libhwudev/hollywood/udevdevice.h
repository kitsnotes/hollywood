// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVDEVICE_H
#define QTUDEVDEVICE_H

#include <QtCore/QObject>

#include <hollywood/qtudevglobal.h>

struct udev_device;

namespace QtUdev {

class Udev;
class UdevEnumerate;
class UdevDevicePrivate;
class UdevMonitorPrivate;

class QTUDEV_EXPORT UdevDevice
{
    Q_DECLARE_PRIVATE(UdevDevice)
public:
    enum DeviceType {
        UnknownDevice = 0x00,
        KeyboardDevice = 0x01,
        MouseDevice = 0x02,
        TouchpadDevice = 0x04,
        TouchscreenDevice = 0x08,
        TabletDevice = 0x10,
        JoystickDevice = 0x20,
        GenericVideoDevice = 0x40,
        PrimaryVideoDevice = 0x80,
        InputDevice_Mask = KeyboardDevice | MouseDevice | TouchpadDevice | TouchscreenDevice | TabletDevice | JoystickDevice,
        VideoDevice_Mask = GenericVideoDevice
    };
    Q_DECLARE_FLAGS(DeviceTypes, DeviceType)

    explicit UdevDevice();
    ~UdevDevice();

    bool isValid() const;

    DeviceTypes type() const;

    QString subsystem() const;
    QString devType() const;
    QString name() const;
    QString driver() const;

    QString deviceNode() const;
    QStringList alternateDeviceSymlinks() const;

    QString sysfsPath() const;
    int sysfsNumber() const;

    QString property(const QString &name) const;
    bool hasProperty(const QString &name) const;

    QStringList deviceProperties() const;
    QStringList sysfsProperties() const;

    UdevDevice *parent() const;

private:
    void initialize(udev_device *dev);

    UdevDevicePrivate *const d_ptr;

    friend class Udev;
    friend class UdevEnumerate;
    friend class UdevMonitorPrivate;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(UdevDevice::DeviceTypes)

QDebug operator<<(QDebug, const UdevDevice &);

} // namespace QtUdev

Q_DECLARE_METATYPE(QtUdev::UdevDevice)

#endif // QTUDEVDEVICE_H
