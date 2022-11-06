/****************************************************************************
**
** Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qeglfskmsintegration.h"
#include "qeglfskmsscreen.h"

#include <hollywood/private/qkmsdevice_p.h>

#include <QtGui/qpa/qplatformwindow.h>
#include <QtGui/QScreen>

#include <xf86drm.h>
#include <xf86drmMode.h>

Q_LOGGING_CATEGORY(qLcEglfsKmsDebug, "qt.qpa.eglfs.kms")

HWEglFSKmsIntegration::HWEglFSKmsIntegration()
    : m_device(nullptr)
{
}

HWEglFSKmsIntegration::~HWEglFSKmsIntegration()
{
}

void HWEglFSKmsIntegration::platformInit()
{
    qCDebug(qLcEglfsKmsDebug, "platformInit: Load Screen Config");
    m_screenConfig = createScreenConfig();

    qCDebug(qLcEglfsKmsDebug, "platformInit: Opening DRM device");
    m_device = createDevice();
    if (Q_UNLIKELY(!m_device->open()))
        qFatal("Could not open DRM device");
}

void HWEglFSKmsIntegration::platformDestroy()
{
    qCDebug(qLcEglfsKmsDebug, "platformDestroy: Closing DRM device");
    m_device->close();
    delete m_device;
    m_device = nullptr;
    delete m_screenConfig;
    m_screenConfig = nullptr;
}

EGLNativeDisplayType HWEglFSKmsIntegration::platformDisplay() const
{
    Q_ASSERT(m_device);
    return (EGLNativeDisplayType) m_device->nativeDisplay();
}

bool HWEglFSKmsIntegration::usesDefaultScreen()
{
    return false;
}

void HWEglFSKmsIntegration::screenInit()
{
    m_device->createScreens();
}

QSurfaceFormat HWEglFSKmsIntegration::surfaceFormatFor(const QSurfaceFormat &inputFormat) const
{
    QSurfaceFormat format(inputFormat);
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    return format;
}

bool HWEglFSKmsIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case QPlatformIntegration::ThreadedPixmaps:
    case QPlatformIntegration::OpenGL:
    case QPlatformIntegration::ThreadedOpenGL:
        return true;
    default:
        return false;
    }
}

void HWEglFSKmsIntegration::waitForVSync(QPlatformSurface *surface) const
{
    QWindow *window = static_cast<QWindow *>(surface->surface());
    HWEglFSKmsScreen *screen = static_cast<HWEglFSKmsScreen *>(window->screen()->handle());

    screen->waitForFlip();
}

bool HWEglFSKmsIntegration::supportsPBuffers() const
{
    return m_screenConfig->supportsPBuffers();
}

void *HWEglFSKmsIntegration::nativeResourceForIntegration(const QByteArray &name)
{
    if (name == QByteArrayLiteral("dri_fd") && m_device)
        return (void *) (qintptr) m_device->fd();

#if QT_CONFIG(drm_atomic)
    if (name == QByteArrayLiteral("dri_atomic_request") && m_device)
        return (void *) (qintptr) m_device->threadLocalAtomicRequest();
#endif
    return nullptr;
}

void *HWEglFSKmsIntegration::nativeResourceForScreen(const QByteArray &resource, QScreen *screen)
{
    HWEglFSKmsScreen *s = static_cast<HWEglFSKmsScreen *>(screen->handle());
    if (s) {
        if (resource == QByteArrayLiteral("dri_crtcid"))
            return (void *) (qintptr) s->output().crtc_id;
        if (resource == QByteArrayLiteral("dri_connectorid"))
            return (void *) (qintptr) s->output().connector_id;
    }
    return nullptr;
}

HWKmsDevice *HWEglFSKmsIntegration::device() const
{
    return m_device;
}

HWKmsScreenConfig *HWEglFSKmsIntegration::screenConfig() const
{
    return m_screenConfig;
}

HWKmsScreenConfig *HWEglFSKmsIntegration::createScreenConfig()
{
    HWKmsScreenConfig *screenConfig = new HWKmsScreenConfig;
    //screenConfig->loadConfig();

    return screenConfig;
}
