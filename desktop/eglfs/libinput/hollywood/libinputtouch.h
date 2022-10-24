/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:QTLGPL$
 *
 * GNU Lesser General Public License Usage
 * This file may be used under the terms of the GNU Lesser General
 * Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPLv3 included in the
 * packaging of this file. Please review the following information to
 * ensure the GNU Lesser General Public License version 3 requirements
 * will be met: https://www.gnu.org/licenses/lgpl.html.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 2.0 or (at your option) the GNU General
 * Public license version 3 or any later version approved by the KDE Free
 * Qt Foundation. The licenses are as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 and LICENSE.GPLv3
 * included in the packaging of this file. Please review the following
 * information to ensure the GNU General Public License requirements will
 * be met: https://www.gnu.org/licenses/gpl-2.0.html and
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_LIBINPUTTOUCH_H
#define LIRI_LIBINPUTTOUCH_H

#include <hollywood/libinputglobal.h>
#include <QtGui/qpa/qwindowsysteminterface.h>

struct libinput_device;
struct libinput_event_touch;

class QTouchDevice;

namespace Originull {

namespace Platform {

class LibInputHandler;
class LIRILIBINPUT_EXPORT LibInputTouch
{
public:
    LibInputTouch(LibInputHandler *h);
    QPointingDevice* registerDevice(libinput_device *device);
    void unregisterDevice(libinput_device *device, QPointingDevice **td);

    void handleTouchUp(libinput_event_touch *event);
    void handleTouchDown(libinput_event_touch *event);
    void handleTouchMotion(libinput_event_touch *event);
    void handleTouchCancel(libinput_event_touch *event);
    void handleTouchFrame(libinput_event_touch *event);

private:
    struct DeviceState {
        DeviceState() : m_touchDevice(nullptr), m_screenName() { }
        QWindowSystemInterface::TouchPoint *point(int32_t slot);
        QList<QWindowSystemInterface::TouchPoint> m_points;
        QPointingDevice *m_touchDevice;
        QString m_screenName;
    };

    DeviceState *deviceState(libinput_event_touch *e);
    QRect screenGeometry(DeviceState *state);
    QPointF getPos(libinput_event_touch *e);

    QHash<libinput_device *, DeviceState> m_devState;
    mutable QPointer<QScreen> m_screen;
    LibInputHandler *m_handler = nullptr;
};

} // namespace Platform

} // namespace Liri

#endif // LIRI_LIBINPUTTOUCH_H
