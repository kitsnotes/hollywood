/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qeglfsdeviceintegration_p.h"
#include "qeglfsintegration_p.h"
#ifndef QT_NO_OPENGL
# include "qeglfscursor_p.h"
#endif
#include "qeglfswindow_p.h"
#include "eglfsscreen_p.h"
#include "qeglfshooks_p.h"

#include <QtGui/private/qeglconvenience_p.h>
#include <QGuiApplication>
#include <private/qguiapplication_p.h>
#include <QScreen>
#include <QDir>
#if QT_CONFIG(regularexpression)
#  include <QRegularExpression>
#endif
#include <QLoggingCategory>

#if defined(Q_OS_LINUX)
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#endif

#include <private/qfactoryloader_p.h>
#include <private/qcore_unix_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcEglDevDebug, "compositor.egldeviceintegration", QtInfoMsg)

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
                          (QEglFSDeviceIntegrationFactoryInterface_iid, QLatin1String("/hwdeviceintegrations"), Qt::CaseInsensitive))

#if QT_CONFIG(library)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader,
                          (QEglFSDeviceIntegrationFactoryInterface_iid, QLatin1String(""), Qt::CaseInsensitive))

#endif // QT_CONFIG(library)

QStringList HWEglFSDeviceIntegrationFactory::keys(const QString &pluginPath)
{
    QStringList list;
#if QT_CONFIG(library)
    if (!pluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(pluginPath);
        list = directLoader()->keyMap().values();
        if (!list.isEmpty()) {
            const QString postFix = QLatin1String(" (from ")
                    + QDir::toNativeSeparators(pluginPath)
                    + QLatin1Char(')');
            const QStringList::iterator end = list.end();
            for (QStringList::iterator it = list.begin(); it != end; ++it)
                (*it).append(postFix);
        }
    }
#else
    Q_UNUSED(pluginPath);
#endif
    list.append(loader()->keyMap().values());
    qCDebug(qLcEglDevDebug) << "EGL device integration plugin keys:" << list;
    return list;
}

HWEglFSDeviceIntegration *HWEglFSDeviceIntegrationFactory::create(const QString &key, const QString &pluginPath)
{
    HWEglFSDeviceIntegration *integration = nullptr;
#if QT_CONFIG(library)
    if (!pluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(pluginPath);
        integration = qLoadPlugin<HWEglFSDeviceIntegration, HWEglFSDeviceIntegrationPlugin>(directLoader(), key);
    }
#else
    Q_UNUSED(pluginPath);
#endif
    if (!integration)
        integration = qLoadPlugin<HWEglFSDeviceIntegration, HWEglFSDeviceIntegrationPlugin>(loader(), key);
    if (integration)
        qCDebug(qLcEglDevDebug) << "Using EGL device integration" << key;
    else
        qCWarning(qLcEglDevDebug) << "Failed to load EGL device integration" << key;

    return integration;
}

static int framebuffer = -1;

QByteArray HWEglFSDeviceIntegration::fbDeviceName() const
{
#ifdef Q_OS_LINUX
    QByteArray fbDev = qgetenv("QT_QPA_EGLFS_FB");
    if (fbDev.isEmpty())
        fbDev = QByteArrayLiteral("/dev/fb0");

    return fbDev;
#else
    return QByteArray();
#endif
}

int HWEglFSDeviceIntegration::framebufferIndex() const
{
    int fbIndex = 0;
#if QT_CONFIG(regularexpression)
    QRegularExpression fbIndexRx(QStringLiteral("fb(\\d+)"));
    QRegularExpressionMatch match = fbIndexRx.match(QString::fromLocal8Bit(fbDeviceName()));
    if (match.hasMatch())
        fbIndex = match.captured(1).toInt();
#endif
    return fbIndex;
}

void HWEglFSDeviceIntegration::platformInit()
{
#ifdef Q_OS_LINUX
    QByteArray fbDev = fbDeviceName();

    framebuffer = qt_safe_open(fbDev, O_RDONLY);

    if (Q_UNLIKELY(framebuffer == -1)) {
        qWarning("EGLFS: Failed to open %s", fbDev.constData());
        qFatal("EGLFS: Can't continue without a display");
    }

#ifdef FBIOBLANK
    ioctl(framebuffer, FBIOBLANK, VESA_NO_BLANKING);
#endif
#endif
}

void HWEglFSDeviceIntegration::platformDestroy()
{
#ifdef Q_OS_LINUX
    if (framebuffer != -1)
        close(framebuffer);
#endif
}

EGLNativeDisplayType HWEglFSDeviceIntegration::platformDisplay() const
{
    return EGL_DEFAULT_DISPLAY;
}

EGLDisplay HWEglFSDeviceIntegration::createDisplay(EGLNativeDisplayType nativeDisplay)
{
    return eglGetDisplay(nativeDisplay);
}

bool HWEglFSDeviceIntegration::usesDefaultScreen()
{
    return true;
}

void HWEglFSDeviceIntegration::screenInit()
{
    // Nothing to do here. Called only when usesDefaultScreen is false.
}

void HWEglFSDeviceIntegration::screenDestroy()
{
    QGuiApplication *app = qGuiApp;
    while (!app->screens().isEmpty())
        QWindowSystemInterface::handleScreenRemoved(app->screens().constLast()->handle());
}

QSizeF HWEglFSDeviceIntegration::physicalScreenSize() const
{
    return q_physicalScreenSizeFromFb(framebuffer, screenSize());
}

QSize HWEglFSDeviceIntegration::screenSize() const
{
    return q_screenSizeFromFb(framebuffer);
}

QDpi HWEglFSDeviceIntegration::logicalDpi() const
{
    const QSizeF ps = physicalScreenSize();
    const QSize s = screenSize();

    if (!ps.isEmpty() && !s.isEmpty())
        return QDpi(25.4 * s.width() / ps.width(),
                    25.4 * s.height() / ps.height());
    else
        return QDpi(100, 100);
}

QDpi HWEglFSDeviceIntegration::logicalBaseDpi() const
{
    return QDpi(100, 100);
}

Qt::ScreenOrientation HWEglFSDeviceIntegration::nativeOrientation() const
{
    return Qt::PrimaryOrientation;
}

Qt::ScreenOrientation HWEglFSDeviceIntegration::orientation() const
{
    return Qt::PrimaryOrientation;
}

int HWEglFSDeviceIntegration::screenDepth() const
{
    return q_screenDepthFromFb(framebuffer);
}

QImage::Format HWEglFSDeviceIntegration::screenFormat() const
{
    return screenDepth() == 16 ? QImage::Format_RGB16 : QImage::Format_RGB32;
}

qreal HWEglFSDeviceIntegration::refreshRate() const
{
    return q_refreshRateFromFb(framebuffer);
}

EGLint HWEglFSDeviceIntegration::surfaceType() const
{
    return EGL_WINDOW_BIT;
}

QSurfaceFormat HWEglFSDeviceIntegration::surfaceFormatFor(const QSurfaceFormat &inputFormat) const
{
    QSurfaceFormat format = inputFormat;

    static const bool force888 = qEnvironmentVariableIntValue("QT_QPA_EGLFS_FORCE888");
    if (force888) {
        format.setRedBufferSize(8);
        format.setGreenBufferSize(8);
        format.setBlueBufferSize(8);
    }

    return format;
}

bool HWEglFSDeviceIntegration::filterConfig(EGLDisplay, EGLConfig) const
{
    return true;
}

HWEglFSWindow *HWEglFSDeviceIntegration::createWindow(QWindow *window) const
{
    return new HWEglFSWindow(window);
}

EGLNativeWindowType HWEglFSDeviceIntegration::createNativeWindow(QPlatformWindow *platformWindow,
                                                    const QSize &size,
                                                    const QSurfaceFormat &format)
{
    Q_UNUSED(platformWindow);
    Q_UNUSED(size);
    Q_UNUSED(format);
    return 0;
}

EGLNativeWindowType HWEglFSDeviceIntegration::createNativeOffscreenWindow(const QSurfaceFormat &format)
{
    Q_UNUSED(format);
    return 0;
}

void HWEglFSDeviceIntegration::destroyNativeWindow(EGLNativeWindowType window)
{
    Q_UNUSED(window);
}

bool HWEglFSDeviceIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    Q_UNUSED(cap);
    return false;
}

QPlatformCursor *HWEglFSDeviceIntegration::createCursor(QPlatformScreen *screen) const
{
#ifndef QT_NO_OPENGL
    return new QEglFSCursor(static_cast<HWEglFSScreen *>(screen));
#else
    Q_UNUSED(screen);
    return nullptr;
#endif
}

void HWEglFSDeviceIntegration::waitForVSync(QPlatformSurface *surface) const
{
    Q_UNUSED(surface);

#if defined(Q_OS_LINUX) && defined(FBIO_WAITFORVSYNC)
    static const bool forceSync = qEnvironmentVariableIntValue("QT_QPA_EGLFS_FORCEVSYNC");
    if (forceSync && framebuffer != -1) {
        int arg = 0;
        if (ioctl(framebuffer, FBIO_WAITFORVSYNC, &arg) == -1)
            qWarning("Could not wait for vsync.");
    }
#endif
}

void HWEglFSDeviceIntegration::presentBuffer(QPlatformSurface *surface)
{
    Q_UNUSED(surface);
}

bool HWEglFSDeviceIntegration::supportsPBuffers() const
{
    return true;
}

bool HWEglFSDeviceIntegration::supportsSurfacelessContexts() const
{
    return true;
}

QFunctionPointer HWEglFSDeviceIntegration::platformFunction(const QByteArray &function) const
{
    Q_UNUSED(function);
    return nullptr;
}

void *HWEglFSDeviceIntegration::nativeResourceForIntegration(const QByteArray &name)
{
    Q_UNUSED(name);
    return nullptr;
}

void *HWEglFSDeviceIntegration::nativeResourceForScreen(const QByteArray &resource, QScreen *screen)
{
    Q_UNUSED(resource);
    Q_UNUSED(screen);
    return nullptr;
}

void *HWEglFSDeviceIntegration::wlDisplay() const
{
    return nullptr;
}

EGLConfig HWEglFSDeviceIntegration::chooseConfig(EGLDisplay display, const QSurfaceFormat &format)
{
    class Chooser : public QEglConfigChooser {
    public:
        Chooser(EGLDisplay display)
            : QEglConfigChooser(display) { }
        bool filterConfig(EGLConfig config) const override {
            return qt_egl_device_integration()->filterConfig(display(), config)
                    && QEglConfigChooser::filterConfig(config);
        }
    };

    Chooser chooser(display);
    chooser.setSurfaceType(qt_egl_device_integration()->surfaceType());
    chooser.setSurfaceFormat(format);
    return chooser.chooseConfig();
}

QT_END_NAMESPACE
