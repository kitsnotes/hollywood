/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 * Copyright (C) 2016 The Qt Company Ltd.
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

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/qpa/qwindowsysteminterface.h>
#include <QtGui/QPointingDevice>
#include <QtGui/QScreen>
#include <QtGui/QPointingDevice>
#include <QtGui/QPointingDevice>
#include <QtGui/QScreen>
#include <QtGui/QPointingDevice>
#include <QtGui/private/qhighdpiscaling_p.h>
#include <QtGui/private/qpointingdevice_p.h>
#include "private/outputmapping_p.h"
#include "private/libinputlogging_p.h"
#include "libinputhandler.h"
#include "libinputtouch.h"

#include <libinput.h>

namespace Originull {

namespace Platform {

QWindowSystemInterface::TouchPoint *LibInputTouch::DeviceState::point(int32_t slot)
{
    const int id = qMax(0, slot);

    for (int i = 0; i < m_points.count(); ++i)
        if (m_points.at(i).id == id)
            return &m_points[i];

    return nullptr;
}

LibInputTouch::DeviceState *LibInputTouch::deviceState(libinput_event_touch *e)
{
    libinput_device *dev = libinput_event_get_device(libinput_event_touch_get_base_event(e));
    return &m_devState[dev];
}

QRect LibInputTouch::screenGeometry(DeviceState *state)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!state->m_screenName.isEmpty()) {
        if (!m_screen) {
            const QList<QScreen *> screens = QGuiApplication::screens();
            for (QScreen *s : screens) {
                if (s->name() == state->m_screenName) {
                    m_screen = s;
                    break;
                }
            }
        }
        if (m_screen)
            screen = m_screen;
    }
    return screen ? QHighDpi::toNativePixels(screen->geometry(), screen) : QRect();
}

QPointF LibInputTouch::getPos(libinput_event_touch *e)
{
    DeviceState *state = deviceState(e);
    QRect geom = screenGeometry(state);
    const double x = libinput_event_touch_get_x_transformed(e, geom.width());
    const double y = libinput_event_touch_get_y_transformed(e, geom.height());
    return geom.topLeft() + QPointF(x, y);
}

LibInputTouch::LibInputTouch(LibInputHandler *h)
    :m_handler(h) {}

QPointingDevice* LibInputTouch::registerDevice(libinput_device *device)
{
    struct udev_device *udev_device;
    udev_device = libinput_device_get_udev_device(device);
    QString devNode = QString::fromUtf8(udev_device_get_devnode(udev_device));
    QString devName = QString::fromUtf8(libinput_device_get_name(device));

    QOutputMapping *mapping = QOutputMapping::get();
    QRect geom;
    if (mapping->load()) {
        m_devState[device].m_screenName = mapping->screenNameForDeviceNode(devNode);
        if (!m_devState[device].m_screenName.isEmpty()) {
            geom = screenGeometry(&m_devState[device]);
            qCDebug(lcInput) << "libinput: Mapping device" << devNode
                                 << "to screen" << m_devState[device].m_screenName
                                 << "with geometry" << geom;
        }
    }

    auto devnum = udev_device_get_devnum(udev_device);
    QPointingDevice *td =new QPointingDevice(devName, devnum,
               QInputDevice::DeviceType::TouchScreen, QPointingDevice::PointerType::Finger,
               QPointingDevice::Capability::Position | QPointingDevice::Capability::Area,
                                             16, 0);

    auto devPriv = QPointingDevicePrivate::get(td);
    if (!geom.isNull())
        devPriv->setAvailableVirtualGeometry(geom);
    m_devState[device].m_touchDevice = td;

    return td;
}

void LibInputTouch::unregisterDevice(libinput_device *device, QPointingDevice **td)
{
    if (td && *td)
        *td = m_devState[device].m_touchDevice;
}

void LibInputTouch::handleTouchUp(libinput_event_touch *event)
{
    DeviceState *state = deviceState(event);

    int slot = libinput_event_touch_get_slot(event);
    QWindowSystemInterface::TouchPoint *touchPoint = state->point(slot);
    if (touchPoint) {
        touchPoint->state = QEventPoint::State::Released;

        // There may not be a Frame event after the last Up. Work this around.
        QEventPoint::States s;
        for (int i = 0; i < state->m_points.count(); ++i)
            s |= state->m_points.at(i).state;
        if (s == QEventPoint::State::Released)
            handleTouchFrame(event);
    } else {
        qCWarning(lcInput) << "Received a touch up without prior touch down for slot" << slot;
    }
}

void LibInputTouch::handleTouchDown(libinput_event_touch *event)
{
    DeviceState *state = deviceState(event);

    int slot = libinput_event_touch_get_slot(event);
    QWindowSystemInterface::TouchPoint *touchPoint = state->point(slot);
    if (touchPoint) {
        // There shouldn't be already a touch point
        qCWarning(lcInput) << "Touch point already present for slot" << slot;
    } else {
        QWindowSystemInterface::TouchPoint newTouchPoint;
        newTouchPoint.id = qMax(0, slot);
        newTouchPoint.state = QEventPoint::State::Pressed;
        newTouchPoint.area = QRect(0, 0, 8, 8);
        newTouchPoint.area.moveCenter(getPos(event));
        state->m_points.append(newTouchPoint);
    }
}

void LibInputTouch::handleTouchMotion(libinput_event_touch *event)
{
    DeviceState *state = deviceState(event);

    int slot = libinput_event_touch_get_slot(event);
    QWindowSystemInterface::TouchPoint *touchPoint = state->point(slot);
    if (touchPoint) {
        const QPointF pos = getPos(event);
        if (touchPoint->area.center() != pos) {
            touchPoint->area.moveCenter(pos);

            if (touchPoint->state != QEventPoint::State::Pressed)
                touchPoint->state = QEventPoint::State::Updated;
        } else {
            touchPoint->state = QEventPoint::State::Stationary;
        }
    } else {
        qCWarning(lcInput) << "Received a touch motion without prior touch down for slot" << slot;
    }
}

void LibInputTouch::handleTouchCancel(libinput_event_touch *event)
{
    DeviceState *state = deviceState(event);
    if (state->m_touchDevice) {
        LibInputTouchEvent e;
        e.device = state->m_touchDevice;
        e.touchPoints = state->m_points;
        e.modifiers = QGuiApplication::keyboardModifiers();
        Q_EMIT m_handler->touchCancel(e);
    } else {
        qCWarning(lcInput) << "Received a touch canceled without a device";
    }
}

void LibInputTouch::handleTouchFrame(libinput_event_touch *event)
{
    DeviceState *state = deviceState(event);
    if (!state->m_touchDevice) {
        qCWarning(lcInput) << "Received a touch frame without a device";
        return;
    }

    if (state->m_points.isEmpty())
        return;

    LibInputTouchEvent e;
    e.device = state->m_touchDevice;
    e.touchPoints = state->m_points;
    e.modifiers = QGuiApplication::keyboardModifiers();
    Q_EMIT m_handler->touchEvent(e);

    for (int i = 0; i < state->m_points.size(); i++) {
        QWindowSystemInterface::TouchPoint &tp(state->m_points[i]);
        if (tp.state == QEventPoint::State::Released)
            state->m_points.removeAt(i--);
        else if (tp.state == QEventPoint::State::Pressed)
            tp.state = QEventPoint::State::Stationary;
    }
}

} // namespace Platform

} // namespace Liri
