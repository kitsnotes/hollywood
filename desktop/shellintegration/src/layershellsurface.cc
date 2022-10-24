/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "layershellinterface.h"
#include <QByteArray>
#ifndef LAYERSHELLQT_EXPORT_H
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LAYERSHELLQT)
#endif
#include <qglobal.h>

using namespace QtWaylandClient;

void Shell::useLayerShell()
{
    const bool ret = qputenv("QT_WAYLAND_SHELL_INTEGRATION", "layer-shell");
    if (!ret) {
        qCDebug(LAYERSHELLQT) << "Unable to set QT_WAYLAND_SHELL_INTEGRATION=layer-shell";
    }
}
