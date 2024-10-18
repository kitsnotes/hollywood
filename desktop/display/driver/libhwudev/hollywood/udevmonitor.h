// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVMONITOR_H
#define QTUDEVMONITOR_H

#include <QtCore/QObject>

#include <hollywood/qtudevglobal.h>

namespace QtUdev {

class Udev;
class UdevDevice;
class UdevMonitorPrivate;

class QTUDEV_EXPORT UdevMonitor : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UdevMonitor)
public:
    explicit UdevMonitor(Udev *udev, QObject *parent = nullptr);
    ~UdevMonitor();

    bool isValid() const;

    void filterSubSystemDevType(const QString &subSystem, const QString &devType);
    void filterTag(const QString &tag);

Q_SIGNALS:
    void deviceAdded(const QtUdev::UdevDevice &device);
    void deviceRemoved(const QtUdev::UdevDevice &device);
    void deviceChanged(const QtUdev::UdevDevice &device);
    void deviceOnlined(const QtUdev::UdevDevice &device);
    void deviceOfflined(const QtUdev::UdevDevice &device);

private:
    UdevMonitorPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_func(), void _q_udevEventHandler())
};

} // namespace QtUdev

#endif // QTUDEVMONITOR_H
