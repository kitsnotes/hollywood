// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVDEVICE_P_H
#define QTUDEVDEVICE_P_H

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

extern "C" {
#include <libudev.h>
}

namespace QtUdev {

class UdevDevicePrivate
{
public:
    UdevDevicePrivate();
    ~UdevDevicePrivate();

    udev_device *device = nullptr;
};

} // namespace QtUdev

#endif // QTUDEVDEVICE_P_H
