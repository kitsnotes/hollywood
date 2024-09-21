/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the qmake spec of the Qt Toolkit.
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

#include "qeglfshooks_p.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qLcEglDevDebug)

#ifdef EGLFS_PLATFORM_HOOKS

QEglFSDeviceIntegration *qt_egl_device_integration()
{
    extern QEglFSHooks *platformHooks;
    return platformHooks;
}

#else

namespace {
class DeviceIntegration
{
public:
    DeviceIntegration();
    ~DeviceIntegration() { delete m_integration; }
    HWEglFSDeviceIntegration *integration() { return m_integration; }
private:
    HWEglFSDeviceIntegration *m_integration;
};
}

Q_GLOBAL_STATIC(DeviceIntegration, deviceIntegration)

DeviceIntegration::DeviceIntegration()
    : m_integration(nullptr)
{
    qDebug() << "finding EGL device integration";
    QStringList pluginKeys = HWEglFSDeviceIntegrationFactory::keys();
    if (!pluginKeys.isEmpty()) {
        // Some built-in logic: Prioritize KMS/DRM.
        const QString kmskey = QStringLiteral("eglfs_kms");
        if (pluginKeys.contains(kmskey)) {
            pluginKeys.removeOne(kmskey);
            pluginKeys.prepend(kmskey);
        }

        QByteArray requested;

        // The environment variable can override everything.
        if (qEnvironmentVariableIsSet("QT_QPA_EGLFS_INTEGRATION")) {
            requested = qgetenv("QT_QPA_EGLFS_INTEGRATION");
            qDebug() << requested;
        } else {
            // Device-specific makespecs may define a preferred plugin.
            requested = "eglfs_kms";
        }

        qDebug() << "requested EGL:" << requested;
        // Treat "none" as special. There has to be a way to indicate
        // that plugins must be ignored when the device is known to be
        // functional with the default, non-specialized integration.
        if (requested != QByteArrayLiteral("none")) {
            if (!requested.isEmpty()) {
                QString reqStr = QString::fromLocal8Bit(requested);
                pluginKeys.removeOne(reqStr);
                pluginKeys.prepend(reqStr);
            }
            qCDebug(qLcEglDevDebug) << "EGL device integration plugin keys (sorted):" << pluginKeys;
            while (!m_integration && !pluginKeys.isEmpty()) {
                QString key = pluginKeys.takeFirst();
                qCDebug(qLcEglDevDebug) << "Trying to load device EGL integration" << key;
                m_integration = HWEglFSDeviceIntegrationFactory::create(key);
            }
        }
    }

    if (!m_integration) {
        // Use a default, non-specialized device integration when no plugin is available.
        // For some systems this is sufficient.
        qCDebug(qLcEglDevDebug) << "Using base device integration";
        m_integration = new HWEglFSDeviceIntegration;
    }
}

HWEglFSDeviceIntegration *qt_egl_device_integration()
{
    return deviceIntegration()->integration();
}

#endif // EGLFS_PLATFORM_HOOKS
