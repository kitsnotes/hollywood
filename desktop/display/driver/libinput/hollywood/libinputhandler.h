/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_LIBINPUTHANDLER_H
#define LIRI_LIBINPUTHANDLER_H

#include <QtCore/QObject>
#include <QtGui/qpa/qwindowsysteminterface.h>

#include <hollywood/libinputglobal.h>

class QPointingDevice;

namespace Originull {

namespace Platform {

class LibInputHandlerPrivate;

struct LIRILIBINPUT_EXPORT LibInputKeyEvent
{
    int key;
    Qt::KeyboardModifiers modifiers;
    quint32 nativeScanCode;
    quint32 nativeVirtualKey;
    quint32 nativeModifiers;
    QString text;
    bool autoRepeat;
    ushort repeatCount;
};

struct LIRILIBINPUT_EXPORT LibInputMouseEvent
{
    QPoint pos;
    Qt::MouseButton button;
    Qt::MouseButtons buttons;
    Qt::KeyboardModifiers modifiers;
    QPoint wheelDelta;
};

struct LIRILIBINPUT_EXPORT LibInputTouchEvent
{
    QPointingDevice *device;
    QList<QWindowSystemInterface::TouchPoint> touchPoints;
    Qt::KeyboardModifiers modifiers;
};

class LIRILIBINPUT_EXPORT LibInputHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool suspended READ isSuspended NOTIFY suspendedChanged)
    Q_DECLARE_PRIVATE(LibInputHandler)
public:
    enum CapabilityFlag {
        Pointer = (1 << 0),
        Keyboard = (1 << 1),
        Touch = (1 << 2),
        Tablet = (1 << 3),
        Gesture = (1 << 4),
    };
    Q_DECLARE_FLAGS(Capabilities, CapabilityFlag)

    LibInputHandler(QObject *parent = 0);

    LibInputHandler::Capabilities capabilities() const;

    int keyboardCount() const;
    int pointerCount() const;
    int touchCount() const;
    int tabletCount() const;
    int gestureCount() const;

    void setPointerPosition(const QPoint &pos);

    bool isSuspended() const;

public Q_SLOTS:
    void suspend();
    void resume();

Q_SIGNALS:
    void ready();
    void capabilitiesChanged();
    void suspendedChanged(bool);

    void keyboardCountChanged(int count);
    void pointerCountChanged(int count);
    void touchCountChanged(int count);
    void tabletCountChanged(int count);
    void gestureCountChanged(int count);

    void touchDeviceRegistered(QPointingDevice *td);
    void touchDeviceUnregistered(QPointingDevice *td);

    void keyPressed(const Originull::Platform::LibInputKeyEvent &event);
    void keyReleased(const Originull::Platform::LibInputKeyEvent &event);

    void mousePressed(const Originull::Platform::LibInputMouseEvent &event);
    void mouseReleased(const Originull::Platform::LibInputMouseEvent &event);
    void mouseMoved(const Originull::Platform::LibInputMouseEvent &event);
    void mouseWheel(const Originull::Platform::LibInputMouseEvent &event);

    void touchEvent(const Originull::Platform::LibInputTouchEvent &event);
    void touchCancel(const Originull::Platform::LibInputTouchEvent &event);

private Q_SLOTS:
    void handleEvents();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LibInputHandler::Capabilities)

} // namespace Platform

} // namespace Liri

Q_DECLARE_METATYPE(Originull::Platform::LibInputKeyEvent)
Q_DECLARE_METATYPE(Originull::Platform::LibInputMouseEvent)
Q_DECLARE_METATYPE(Originull::Platform::LibInputTouchEvent)

#endif // LIRI_LIBINPUTHANDLER_H
