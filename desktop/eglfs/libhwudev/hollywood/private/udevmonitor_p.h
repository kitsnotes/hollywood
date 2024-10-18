// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVMONITOR_P_H
#define QTUDEVMONITOR_P_H

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

#include "udevmonitor.h"

namespace QtUdev {

class UdevMonitorPrivate
{
    Q_DECLARE_PUBLIC(UdevMonitor)
public:
    UdevMonitorPrivate(UdevMonitor *qq, Udev *u);
    ~UdevMonitorPrivate();

    void _q_udevEventHandler();

    Udev *udev;
    struct udev_monitor *monitor;

protected:
    UdevMonitor *q_ptr;
};

} // namespace QtUdev

#endif // QTUDEVMONITOR_P_H
