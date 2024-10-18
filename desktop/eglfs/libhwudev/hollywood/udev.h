// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEV_H
#define QTUDEV_H

#include <QtCore/QString>

#include <hollywood/qtudevglobal.h>

namespace QtUdev {

class UdevDevice;
class UdevPrivate;

class QTUDEV_EXPORT Udev
{
    Q_DECLARE_PRIVATE(Udev)
public:
    Udev();
    ~Udev();

    bool isValid() const;

    UdevDevice *deviceFromFileName(const QString &fileName) const;
    UdevDevice *deviceFromSubSystemAndName(const QString &subSystem, const QString &name) const;
    UdevDevice *deviceFromSysfsPath(const QString &sysfsPath) const;

private:
    UdevPrivate *const d_ptr;
};

} // namespace QtUdev

#endif // QTUDEV_H
