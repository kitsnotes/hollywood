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

#ifndef QEGLFSKMSSCREEN_H
#define QEGLFSKMSSCREEN_H

#include "hollywood/private/eglfsscreen_p.h"
#include <QtCore/QList>
#include <QtCore/QMutex>
#include "hollywood/private/qkmsdevice_p.h"
#include <QtGui/private/qedidparser_p.h>

QT_BEGIN_NAMESPACE

class HWEglFSKmsDevice;
class HWEglFSKmsInterruptHandler;

class Q_EGLFS_EXPORT HWEglFSKmsScreen : public HWEglFSScreen
{
public:
    HWEglFSKmsScreen(HWEglFSKmsDevice *device, const HWKmsOutput &output, bool headless = false);
    ~HWEglFSKmsScreen();

    void setVirtualPosition(const QPoint &pos);

    QRect rawGeometry() const override;

    int depth() const override;
    QImage::Format format() const override;

    QSizeF physicalSize() const override;
    QDpi logicalDpi() const override;
    QDpi logicalBaseDpi() const override;
    Qt::ScreenOrientation nativeOrientation() const override;
    Qt::ScreenOrientation orientation() const override;

    QString name() const override;

    QString manufacturer() const override;
    QString model() const override;
    QString serialNumber() const override;

    qreal refreshRate() const override;

    QList<QPlatformScreen *> virtualSiblings() const override { return m_siblings; }
    void setVirtualSiblings(QList<QPlatformScreen *> sl) { m_siblings = sl; }

    QList<QPlatformScreen::Mode> modes() const override;

    int currentMode() const override;
    int preferredMode() const override;

    HWEglFSKmsDevice *device() const { return m_device; }

    virtual void waitForFlip();

    HWKmsOutput &output() { return m_output; }
    void restoreMode();
    bool setMode(const QSize &size, qreal refresh);

    SubpixelAntialiasingType subpixelAntialiasingTypeHint() const override;

    QPlatformScreen::PowerState powerState() const override;
    void setPowerState(QPlatformScreen::PowerState state) override;

    bool isCursorOutOfRange() const { return m_cursorOutOfRange; }
    void setCursorOutOfRange(bool b) { m_cursorOutOfRange = b; }

    virtual void pageFlipped(unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec);
protected:
    HWEglFSKmsDevice *m_device;

    HWKmsOutput m_output;
    QEdidParser m_edid;
    QPoint m_pos;
    bool m_cursorOutOfRange;

    QList<QPlatformScreen *> m_siblings;

    PowerState m_powerState;

    HWEglFSKmsInterruptHandler *m_interruptHandler;

    bool m_headless;
};

QT_END_NAMESPACE

#endif
