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

#include <QtCore/qtextstream.h>
#include <qpa/qwindowsysteminterface.h>
#include <qpa/qplatformintegration.h>
#include <private/qguiapplication_p.h>
#include <private/qwindow_p.h>
# include <QtGui/private/qopenglcontext_p.h>
# include <QtGui/QOpenGLContext>
# include <QtOpenGL/private/qopenglcompositorbackingstore_p.h>
#include <QtGui/private/qeglconvenience_p.h>

#include "qeglfswindow_p.h"
#ifndef QT_NO_OPENGL
# include "qeglfscursor_p.h"
#endif
#include "qeglfshooks_p.h"
#include "qeglfsdeviceintegration_p.h"

HWEglFSWindow::HWEglFSWindow(QWindow *w)
    : QPlatformWindow(w),
#ifndef QT_NO_OPENGL
      m_backingStore(0),
      m_rasterCompositingContext(0),
#endif
      m_winId(0),
      m_surface(EGL_NO_SURFACE),
      m_window(0)
{
}

HWEglFSWindow::~HWEglFSWindow()
{
    destroy();
}

static WId newWId()
{
    static WId id = 0;

    if (id == std::numeric_limits<WId>::max())
        qWarning("QEGLPlatformWindow: Out of window IDs");

    return ++id;
}

void HWEglFSWindow::create()
{
    if (m_flags.testFlag(Created))
        return;

    m_winId = newWId();

    // Save the original surface type before changing to OpenGLSurface.
    m_raster = (window()->surfaceType() == QSurface::RasterSurface);
    if (m_raster) // change to OpenGL, but not for RasterGLSurface
        window()->setSurfaceType(QSurface::OpenGLSurface);

    if (window()->type() == Qt::Desktop) {
        QRect fullscreenRect(QPoint(), screen()->availableGeometry().size());
        QWindowSystemInterface::handleGeometryChange(window(), fullscreenRect);
        return;
    }

    m_flags = Created;

    if (window()->type() == Qt::Desktop)
        return;

    // Stop if there is already a window backed by a native window and surface. Additional
    // raster windows will not have their own native window, surface and context. Instead,
    // they will be composited onto the root window's surface.
    HWEglFSScreen *screen = this->screen();
#ifndef QT_NO_OPENGL
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    if (screen->primarySurface() != EGL_NO_SURFACE) {
        if (Q_UNLIKELY(!isRaster() || !compositor->targetWindow())) {
#if !defined(Q_OS_ANDROID) || defined(Q_OS_ANDROID_EMBEDDED)
            // We can have either a single OpenGL window or multiple raster windows.
            // Other combinations cannot work.
            qFatal("EGLFS: OpenGL windows cannot be mixed with others.");
#endif
            return;
        }
        m_format = compositor->targetWindow()->format();
        return;
    }
#endif // QT_NO_OPENGL

    m_flags |= HasNativeWindow;
    setGeometry(QRect()); // will become fullscreen

    resetSurface();

    if (Q_UNLIKELY(m_surface == EGL_NO_SURFACE)) {
        EGLint error = eglGetError();
        eglTerminate(screen->display());
        qFatal("EGL Error : Could not create the egl surface: error = 0x%x\n", error);
    }

    screen->setPrimarySurface(m_surface);

#ifndef QT_NO_OPENGL
    if (isRaster()) {
        m_rasterCompositingContext = new QOpenGLContext;
        m_rasterCompositingContext->setShareContext(qt_gl_global_share_context());
        m_rasterCompositingContext->setFormat(m_format);
        m_rasterCompositingContext->setScreen(window()->screen());
        if (Q_UNLIKELY(!m_rasterCompositingContext->create()))
            qFatal("EGLFS: Failed to create compositing context");
#if QT_VERSION < 0x060500
        compositor->setTarget(m_rasterCompositingContext, window(), screen->rawGeometry());
#else
	compositor->setTargetWindow(window(), screen->rawGeometry());
#endif
        compositor->setRotation(qEnvironmentVariableIntValue("QT_QPA_EGLFS_ROTATION"));
        // If there is a "root" window into which raster and QOpenGLWidget content is
        // composited, all other contexts must share with its context.
        if (!qt_gl_global_share_context()) {
            qt_gl_set_global_share_context(m_rasterCompositingContext);
            // What we set up here is in effect equivalent to the application setting
            // AA_ShareOpenGLContexts. Set the attribute to be fully consistent.
            QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
        }
#if QT_VERSION > 0x060500
	QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
	    compositor->setTargetContext(m_rasterCompositingContext);
#endif
    }
#endif // QT_NO_OPENGL
}

void HWEglFSWindow::destroy()
{
#ifndef QT_NO_OPENGL
    QOpenGLCompositor::instance()->removeWindow(this);
#endif

    HWEglFSScreen *screen = this->screen();
    if (m_flags.testFlag(HasNativeWindow)) {
#ifndef QT_NO_OPENGL
        QEglFSCursor *cursor = qobject_cast<QEglFSCursor *>(screen->cursor());
        if (cursor)
            cursor->resetResources();
#endif
        if (screen->primarySurface() == m_surface)
            screen->setPrimarySurface(EGL_NO_SURFACE);

        invalidateSurface();

#ifndef QT_NO_OPENGL
        QOpenGLCompositor::destroy();
        delete m_rasterCompositingContext;
#endif
    }

    m_flags = { };
}

void HWEglFSWindow::invalidateSurface()
{
    if (m_surface != EGL_NO_SURFACE) {
        eglDestroySurface(screen()->display(), m_surface);
        m_surface = EGL_NO_SURFACE;
    }
    qt_egl_device_integration()->destroyNativeWindow(m_window);
    m_window = 0;
}

void HWEglFSWindow::resetSurface()
{
    EGLDisplay display = screen()->display();
    QSurfaceFormat platformFormat = qt_egl_device_integration()->surfaceFormatFor(window()->requestedFormat());

    m_config = HWEglFSDeviceIntegration::chooseConfig(display, platformFormat);
    m_format = q_glFormatFromConfig(display, m_config, platformFormat);
    const QSize surfaceSize = screen()->rawGeometry().size();
    m_window = qt_egl_device_integration()->createNativeWindow(this, surfaceSize, m_format);
    m_surface = eglCreateWindowSurface(display, m_config, m_window, NULL);
}

bool HWEglFSWindow::resizeSurface(const QSize &size)
{
    EGLDisplay display = screen()->display();
    EGLNativeWindowType window = qt_egl_device_integration()->createNativeWindow(this, size, m_format);
    EGLSurface surface = eglCreateWindowSurface(display, m_config, window, NULL);

    if (Q_UNLIKELY(surface == EGL_NO_SURFACE)) {
        qt_egl_device_integration()->destroyNativeWindow(window);
        return false;
    }

    // Keep track of the old surface
    EGLSurface oldSurface = m_surface;
    EGLNativeWindowType oldWindow = m_window;

    // Switch to the new one
    screen()->setPrimarySurface(surface);
    m_window = window;
    m_surface = surface;

    // New surface created: destroy the old one
    if (oldSurface != EGL_NO_SURFACE)
        eglDestroySurface(display, oldSurface);
    if (oldWindow)
        qt_egl_device_integration()->destroyNativeWindow(oldWindow);

    return true;
}

void HWEglFSWindow::setVisible(bool visible)
{
#ifndef QT_NO_OPENGL
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    QList<QOpenGLCompositorWindow *> windows = compositor->windows();
    QWindow *wnd = window();

    if (wnd->type() != Qt::Desktop) {
        if (visible) {
            compositor->addWindow(this);
        } else {
            compositor->removeWindow(this);
            windows = compositor->windows();
            if (windows.size())
                windows.last()->sourceWindow()->requestActivate();
        }
    }
#else
    QWindow *wnd = window();
#endif
    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));

    if (visible)
        QWindowSystemInterface::flushWindowSystemEvents(QEventLoop::ExcludeUserInputEvents);
}

void HWEglFSWindow::setGeometry(const QRect &r)
{
    QRect rect = r;
    if (m_flags.testFlag(HasNativeWindow))
        rect = screen()->availableGeometry();

    QPlatformWindow::setGeometry(rect);

    QWindowSystemInterface::handleGeometryChange(window(), rect);

    const QRect lastReportedGeometry = qt_window_private(window())->geometry;
    if (rect != lastReportedGeometry)
        QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), rect.size()));
}

QRect HWEglFSWindow::geometry() const
{
    // For yet-to-become-fullscreen windows report the geometry covering the entire
    // screen. This is particularly important for Quick where the root object may get
    // sized to some geometry queried before calling create().
    if (!m_flags.testFlag(Created) && screen()->primarySurface() == EGL_NO_SURFACE)
        return screen()->availableGeometry();

    return QPlatformWindow::geometry();
}

void HWEglFSWindow::requestActivateWindow()
{
#ifndef QT_NO_OPENGL
    if (window()->type() != Qt::Desktop)
        QOpenGLCompositor::instance()->moveToTop(this);
#endif
    QWindow *wnd = window();
    QWindowSystemInterface::handleFocusWindowChanged(wnd, Qt::ActiveWindowFocusReason);
    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
}

void HWEglFSWindow::raise()
{
    QWindow *wnd = window();
    if (wnd->type() != Qt::Desktop) {
#ifndef QT_NO_OPENGL
        QOpenGLCompositor::instance()->moveToTop(this);
#endif
        QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
    }
}

void HWEglFSWindow::lower()
{
#ifndef QT_NO_OPENGL
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    QList<QOpenGLCompositorWindow *> windows = compositor->windows();
    if (window()->type() != Qt::Desktop && windows.count() > 1) {
        int idx = windows.indexOf(this);
        if (idx > 0) {
            compositor->changeWindowIndex(this, idx - 1);
            QWindowSystemInterface::handleExposeEvent(windows.last()->sourceWindow(),
                                                      QRect(QPoint(0, 0), windows.last()->sourceWindow()->geometry().size()));
        }
    }
#endif
}

EGLSurface HWEglFSWindow::surface() const
{
    return m_surface != EGL_NO_SURFACE ? m_surface : screen()->primarySurface();
}

QSurfaceFormat HWEglFSWindow::format() const
{
    return m_format;
}

EGLNativeWindowType HWEglFSWindow::eglWindow() const
{
    return m_window;
}

HWEglFSScreen *HWEglFSWindow::screen() const
{
    return static_cast<HWEglFSScreen *>(QPlatformWindow::screen());
}

bool HWEglFSWindow::isRaster() const
{
    return m_raster || window()->surfaceType() == QSurface::RasterGLSurface;
}

#ifndef QT_NO_OPENGL
QWindow *HWEglFSWindow::sourceWindow() const
{
    return window();
}

const QPlatformTextureList *HWEglFSWindow::textures() const
{
    if (m_backingStore)
        return m_backingStore->textures();

    return 0;
}

void HWEglFSWindow::endCompositing()
{
    if (m_backingStore)
        m_backingStore->notifyComposited();
}
#endif

WId HWEglFSWindow::winId() const
{
    return m_winId;
}

void HWEglFSWindow::setOpacity(qreal)
{
    if (!isRaster())
        qWarning("QEglFSWindow: Cannot set opacity for non-raster windows");

    // Nothing to do here. The opacity is stored in the QWindow.
}
