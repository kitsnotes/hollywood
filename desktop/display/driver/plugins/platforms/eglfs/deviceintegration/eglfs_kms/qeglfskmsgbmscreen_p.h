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

#ifndef QEGLFSKMSGBMSCREEN_H
#define QEGLFSKMSGBMSCREEN_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <hollywood/qeglfskmsscreen.h>
#include <hollywood/qeglfskmsdevice.h>

#include <QMutex>
#include <QWaitCondition>

#include <gbm.h>

class HWEglFSKmsGbmCursor;

class Q_EGLFS_EXPORT HWEglFSKmsGbmScreen : public HWEglFSKmsScreen
{
public:
    HWEglFSKmsGbmScreen(HWEglFSKmsDevice *device, const HWKmsOutput &output, bool headless);
    ~HWEglFSKmsGbmScreen();

    QPlatformCursor *cursor() const override;

    gbm_surface *createGbmSurface(EGLConfig eglConfig, const QSize &size);
    gbm_surface *createSurface(EGLConfig eglConfig);
    void resetSurface();

    void initCloning(QPlatformScreen *screenThisScreenClones,
                     const QList<QPlatformScreen *> &screensCloningThisScreen);

    void waitForFlip() override;

    virtual void flip();

    void setCursorTheme(const QString &name, int size) override;
    virtual void updateFlipStatus();

    void setSurface(gbm_surface *surface);

    virtual uint32_t gbmFlags() { return GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING; }

    //bool setNewMode(const QSize size, const int refresh);

protected:
    void flipFinished();
    void ensureModeSet(uint32_t fb);
    void cloneDestFlipFinished(HWEglFSKmsGbmScreen *cloneDestScreen);

    gbm_surface *m_gbm_surface;

    gbm_bo *m_gbm_bo_current;
    gbm_bo *m_gbm_bo_next;
    bool m_flipPending;

    QMutex m_flipMutex;
    QWaitCondition m_flipCond;

    QScopedPointer<HWEglFSKmsGbmCursor> m_cursor;

    struct FrameBuffer {
        uint32_t fb = 0;
    };
    static void bufferDestroyedHandler(gbm_bo *bo, void *data);
    FrameBuffer *framebufferForBufferObject(gbm_bo *bo);

    HWEglFSKmsGbmScreen *m_cloneSource;
    struct CloneDestination {
        HWEglFSKmsGbmScreen *screen = nullptr;
        bool cloneFlipPending = false;
    };
    QList<CloneDestination> m_cloneDests;
};

#endif // QEGLFSKMSGBMSCREEN_H
