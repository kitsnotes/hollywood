// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVENUMERATE_H
#define QTUDEVENUMERATE_H

#include <hollywood/udevdevice.h>

namespace QtUdev {

class UdevEnumeratePrivate;

class QTUDEV_EXPORT UdevEnumerate
{
    Q_DECLARE_PRIVATE(UdevEnumerate)
public:
    UdevEnumerate(UdevDevice::DeviceTypes types, Udev *udev);
    ~UdevEnumerate();

    QList<UdevDevice *> scan() const;

private:
    UdevEnumeratePrivate *const d_ptr;
};

} // namespace QtUdev

#endif // QTUDEVENUMERATE_H
