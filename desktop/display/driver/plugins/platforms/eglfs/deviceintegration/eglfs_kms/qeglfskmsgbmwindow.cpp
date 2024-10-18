/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qeglfskmsgbmwindow_p.h"
#include "qeglfskmsgbmintegration_p.h"
#include "qeglfskmsgbmscreen_p.h"

#include <QtGui/private/qeglconvenience_p.h>

#ifndef EGL_EXT_platform_base
typedef EGLSurface (EGLAPIENTRYP PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC) (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
#endif

void HWEglFSKmsGbmWindow::resetSurface()
{
    HWEglFSKmsGbmScreen *gbmScreen = static_cast<HWEglFSKmsGbmScreen *>(screen());
    EGLDisplay display = gbmScreen->display();
    QSurfaceFormat platformFormat = m_integration->surfaceFormatFor(window()->requestedFormat());
    m_config = HWEglFSDeviceIntegration::chooseConfig(display, platformFormat);
    m_format = q_glFormatFromConfig(display, m_config, platformFormat);
    // One fullscreen window per screen -> the native window is simply the gbm_surface the screen created.
    m_window = reinterpret_cast<EGLNativeWindowType>(gbmScreen->createSurface(m_config));

    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC createPlatformWindowSurface = nullptr;
    const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (extensions && (strstr(extensions, "EGL_KHR_platform_gbm") || strstr(extensions, "EGL_MESA_platform_gbm"))) {
        createPlatformWindowSurface = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(
            eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT"));
    }

    if (createPlatformWindowSurface) {
        QVector<EGLint> contextAttributes;
#ifdef EGL_EXT_protected_content
        if (platformFormat.testOption(QSurfaceFormat::ProtectedContent)) {
            if (q_hasEglExtension(display, "EGL_EXT_protected_content")) {
                contextAttributes.append(EGL_PROTECTED_CONTENT_EXT);
                contextAttributes.append(EGL_TRUE);
                qCDebug(qLcEglfsKmsDebug, "Enabled EGL_PROTECTED_CONTENT_EXT for eglCreatePlatformWindowSurfaceEXT");
            } else {
                m_format.setOption(QSurfaceFormat::ProtectedContent, false);
            }
        }
#endif
        contextAttributes.append(EGL_NONE);

        m_surface = createPlatformWindowSurface(display, m_config, reinterpret_cast<void *>(m_window), contextAttributes.constData());
    } else {
        qCDebug(qLcEglfsKmsDebug, "No eglCreatePlatformWindowSurface for GBM, falling back to eglCreateWindowSurface");
        m_surface = eglCreateWindowSurface(display, m_config, m_window, nullptr);
    }
}

void HWEglFSKmsGbmWindow::invalidateSurface()
{
    HWEglFSKmsGbmScreen *gbmScreen = static_cast<HWEglFSKmsGbmScreen *>(screen());
    HWEglFSWindow::invalidateSurface();
    gbmScreen->resetSurface();
}

bool HWEglFSKmsGbmWindow::resizeSurface(const QSize &size)
{
    HWEglFSKmsGbmIntegration *integration = const_cast<HWEglFSKmsGbmIntegration *>(m_integration);

    HWEglFSKmsGbmScreen *gbmScreen = static_cast<HWEglFSKmsGbmScreen *>(screen());
    EGLDisplay display = gbmScreen->display();
    QSurfaceFormat platformFormat = m_integration->surfaceFormatFor(window()->requestedFormat());
    m_config = HWEglFSDeviceIntegration::chooseConfig(display, platformFormat);
    m_format = q_glFormatFromConfig(display, m_config, platformFormat);
    // One fullscreen window per screen -> the native window is simply the gbm_surface the screen created.
    EGLNativeWindowType window = reinterpret_cast<EGLNativeWindowType>(gbmScreen->createGbmSurface(m_config, size));

    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC createPlatformWindowSurface = nullptr;
    const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (extensions && (strstr(extensions, "EGL_KHR_platform_gbm") || strstr(extensions, "EGL_MESA_platform_gbm"))) {
        createPlatformWindowSurface = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(
            eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT"));
    }

    EGLSurface surface = EGL_NO_SURFACE;

    if (createPlatformWindowSurface) {
        surface = createPlatformWindowSurface(display, m_config, reinterpret_cast<void *>(window), nullptr);
    } else {
        qCDebug(qLcEglfsKmsDebug, "No eglCreatePlatformWindowSurface for GBM, falling back to eglCreateWindowSurface");
        surface = eglCreateWindowSurface(display, m_config, window, nullptr);
    }

    if (Q_UNLIKELY(surface == EGL_NO_SURFACE)) {
        integration->destroyNativeWindow(window);
        return false;
    }

    gbmScreen->setSurface(reinterpret_cast<gbm_surface *>(window));

    // Keep track of the old surface
    EGLSurface oldSurface = m_surface;
    EGLNativeWindowType oldWindow = m_window;

    // Switch to the new one
    m_window = window;
    m_surface = surface;

    // New surface created: destroy the old one
    if (oldSurface != EGL_NO_SURFACE)
        eglDestroySurface(display, oldSurface);
    if (oldWindow)
        integration->destroyNativeWindow(oldWindow);

    return true;
}
