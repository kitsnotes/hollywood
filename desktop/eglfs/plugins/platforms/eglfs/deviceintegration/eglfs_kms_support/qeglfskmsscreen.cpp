/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
** Copyright (C) 2016 Pelagicore AG
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qeglfskmsscreen.h"
#include "qeglfskmsdevice.h"
#include <private/qeglfsintegration_p.h>

#include <QtCore/QLoggingCategory>

#include <QtGui/private/qguiapplication_p.h>
#include <hollywood/private/vthandler_p.h>

Q_DECLARE_LOGGING_CATEGORY(qLcEglfsKmsDebug)

class HWEglFSKmsInterruptHandler : public QObject
{
public:
    HWEglFSKmsInterruptHandler(HWEglFSKmsScreen *screen) : m_screen(screen) {
        // TODO: fix
        m_vtHandler = static_cast<HWEglFSIntegration *>(QGuiApplicationPrivate::platformIntegration())->vtHandler();
        //connect(m_vtHandler, &Originull::Platform::VtHandler::interrupted, this, &HWEglFSKmsInterruptHandler::restoreVideoMode);
        //connect(m_vtHandler, &Originull::Platform::VtHandler::aboutToSuspend, this, &HWEglFSKmsInterruptHandler::restoreVideoMode);
    }

public slots:
    void restoreVideoMode() { m_screen->restoreMode(); }

private:
    Originull::Platform::VtHandler *m_vtHandler;
    HWEglFSKmsScreen *m_screen;
};

HWEglFSKmsScreen::HWEglFSKmsScreen(HWEglFSKmsDevice *device, const HWKmsOutput &output, bool headless)
    : HWEglFSScreen(static_cast<HWEglFSIntegration *>(QGuiApplicationPrivate::platformIntegration())->display())
    , m_device(device)
    , m_output(output)
    , m_cursorOutOfRange(false)
    , m_powerState(PowerStateOn)
    , m_interruptHandler(new HWEglFSKmsInterruptHandler(this))
    , m_headless(headless)
{
    m_siblings << this; // gets overridden later

    if (m_output.edid_blob) {
        QByteArray edid(reinterpret_cast<const char *>(m_output.edid_blob->data), m_output.edid_blob->length);
        if (m_edid.parse(edid))
            qCDebug(qLcEglfsKmsDebug, "EDID data for output \"%s\": identifier '%s', manufacturer '%s', model '%s', serial '%s', physical size: %.2fx%.2f",
                    name().toLatin1().constData(),
                    m_edid.identifier.toLatin1().constData(),
                    m_edid.manufacturer.toLatin1().constData(),
                    m_edid.model.toLatin1().constData(),
                    m_edid.serialNumber.toLatin1().constData(),
                    m_edid.physicalSize.width(), m_edid.physicalSize.height());
        else
            qCDebug(qLcEglfsKmsDebug) << "Failed to parse EDID data for output" << name(); // keep this debug, not warning
    } else {
        qCDebug(qLcEglfsKmsDebug) << "No EDID data for output" << name();
    }
}

HWEglFSKmsScreen::~HWEglFSKmsScreen()
{
    m_output.cleanup(m_device);
    delete m_interruptHandler;
}

void HWEglFSKmsScreen::setVirtualPosition(const QPoint &pos)
{
    m_pos = pos;
}

// Reimplement rawGeometry(), not geometry(). The base class implementation of
// geometry() calls rawGeometry() and may apply additional transforms.
QRect HWEglFSKmsScreen::rawGeometry() const
{
    if (m_headless)
        return QRect(QPoint(0, 0), m_device->screenConfig()->headlessSize());

    return QRect(m_pos.x(), m_pos.y(),
                 m_output.size.width(),
                 m_output.size.height());
}

int HWEglFSKmsScreen::depth() const
{
    return format() == QImage::Format_RGB16 ? 16 : 32;
}

QImage::Format HWEglFSKmsScreen::format() const
{
    // the result can be slightly incorrect, it won't matter in practice
    switch (m_output.drm_format) {
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_ABGR8888:
        return QImage::Format_ARGB32;
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        return QImage::Format_RGB16;
    case DRM_FORMAT_XRGB2101010:
        return QImage::Format_RGB30;
    case DRM_FORMAT_XBGR2101010:
        return QImage::Format_BGR30;
    case DRM_FORMAT_ARGB2101010:
        return QImage::Format_A2RGB30_Premultiplied;
    case DRM_FORMAT_ABGR2101010:
        return QImage::Format_A2BGR30_Premultiplied;
    default:
        return QImage::Format_RGB32;
    }
}

QSizeF HWEglFSKmsScreen::physicalSize() const
{
    if (!m_output.physical_size.isEmpty()) {
        return m_output.physical_size;
    } else {
        const QSize s = geometry().size();
        return QSizeF(0.254 * s.width(), 0.254 * s.height());
    }
}

QDpi HWEglFSKmsScreen::logicalDpi() const
{
    return logicalBaseDpi();
}

QDpi HWEglFSKmsScreen::logicalBaseDpi() const
{
    return QDpi(100, 100);
}

Qt::ScreenOrientation HWEglFSKmsScreen::nativeOrientation() const
{
    return Qt::PrimaryOrientation;
}

Qt::ScreenOrientation HWEglFSKmsScreen::orientation() const
{
    return Qt::PrimaryOrientation;
}

QString HWEglFSKmsScreen::name() const
{
    return !m_headless ? m_output.name : QStringLiteral("qt_Headless");
}

QString HWEglFSKmsScreen::manufacturer() const
{
    return m_edid.manufacturer;
}

QString HWEglFSKmsScreen::model() const
{
    return m_edid.model.isEmpty() ? m_edid.identifier : m_edid.model;
}

QString HWEglFSKmsScreen::serialNumber() const
{
    return m_edid.serialNumber;
}

void HWEglFSKmsScreen::waitForFlip()
{
}

void HWEglFSKmsScreen::restoreMode()
{
    m_output.restoreMode(m_device);
}

qreal HWEglFSKmsScreen::refreshRate() const
{
    if (m_headless)
        return 60;

    quint32 refresh = m_output.modes[m_output.mode].vrefresh;
    return refresh > 0 ? refresh : 60;
}

QList<QPlatformScreen::Mode> HWEglFSKmsScreen::modes() const
{
    QList<QPlatformScreen::Mode> list;
    list.reserve(m_output.modes.size());

    for (const drmModeModeInfo &info : qAsConst(m_output.modes))
        list.append({QSize(info.hdisplay, info.vdisplay),
                     qreal(info.vrefresh > 0 ? info.vrefresh : 60)});

    return list;
}

int HWEglFSKmsScreen::currentMode() const
{
    return m_output.mode;
}

int HWEglFSKmsScreen::preferredMode() const
{
    return m_output.preferred_mode;
}

QPlatformScreen::SubpixelAntialiasingType HWEglFSKmsScreen::subpixelAntialiasingTypeHint() const
{
    return m_output.subpixelAntialiasingTypeHint();
}

QPlatformScreen::PowerState HWEglFSKmsScreen::powerState() const
{
    return m_powerState;
}

void HWEglFSKmsScreen::setPowerState(QPlatformScreen::PowerState state)
{
    m_output.setPowerState(m_device, state);
    m_powerState = state;
}

/* Informs exact page flip timing which can be used rendering optimization.
   Consider this is from drm event reader thread. */
void HWEglFSKmsScreen::pageFlipped(unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec)
{
    Q_UNUSED(sequence);
    Q_UNUSED(tv_sec);
    Q_UNUSED(tv_usec);
}

QT_END_NAMESPACE
