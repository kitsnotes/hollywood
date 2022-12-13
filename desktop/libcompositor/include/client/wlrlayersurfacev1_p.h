// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_WLRLAYERSURFACEV1_P_CLIENT_H
#define LIRI_WLRLAYERSURFACEV1_P_CLIENT_H

#include <QLoggingCategory>
#include "wlrlayersurfacev1.h"
#include "libcompositor_global.h"

class WlrLayerSurfaceV1Private
{
public:
    WlrLayerSurfaceV1Private() = default;

    bool initialized = false;
    bool setLayerEnabled = false;
    QWindow *window = nullptr;
    WlrLayerSurfaceV1::Layer layer = WlrLayerSurfaceV1::TopLayer;
    QString role = QStringLiteral("unknown");
    WlrLayerSurfaceV1::Anchors anchors;
    qint32 exclusiveZone = 0;
    QMargins margins;
    WlrLayerSurfaceV1::KeyboardInteractivity keyboardInteractivity = WlrLayerSurfaceV1::ExclusiveKeyboardInteractivity;
};

#endif // LIRI_WLRLAYERSURFACEV1_P_CLIENT_H
