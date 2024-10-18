// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVENUMERATE_P_H
#define QTUDEVENUMERATE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtUdev.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "udevdevice.h"

extern "C" {
#include <libudev.h>
}

class LogindSeat;

namespace QtUdev {

class UdevEnumeratePrivate
{
public:
    UdevEnumeratePrivate(UdevDevice::DeviceTypes t, Udev *u);
    ~UdevEnumeratePrivate();

    LogindSeat *logindSeat = nullptr;
    UdevDevice::DeviceTypes types;
    Udev *udev;
    udev_enumerate *enumerate;
};

} // namespace QtUdev

#endif // UDEVENUMERATE_P_H
