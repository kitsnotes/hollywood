// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEV_P_H
#define QTUDEV_P_H

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

#include <QLoggingCategory>

#include "qtudevglobal.h"

extern "C" {
#include <libudev.h>
}

Q_DECLARE_LOGGING_CATEGORY(lcUdev)

namespace QtUdev {

class Udev;

class QTUDEV_EXPORT UdevPrivate
{
public:
    UdevPrivate();
    ~UdevPrivate();

    static UdevPrivate *get(Udev *u);

    struct udev *udev;
};

} // namespace QtUdev

#endif // QTUDEV_P_H

