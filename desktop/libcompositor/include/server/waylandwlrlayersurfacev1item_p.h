// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_WAYLANDWLRLAYERSURFACEV1ITEM_P_H
#define LIRI_WAYLANDWLRLAYERSURFACEV1ITEM_P_H

#include <QMutexLocker>
#include "libcompositor_global.h"

class WaylandWlrLayerSurfaceV1;

class LIBCOMPOSITOR_EXPORT WaylandWlrLayerSurfaceV1ItemPrivate
{
    Q_DECLARE_PUBLIC(WaylandWlrLayerSurfaceV1Item)
public:
    WaylandWlrLayerSurfaceV1ItemPrivate(WaylandWlrLayerSurfaceV1Item *self);

    void _q_configure();

    WaylandWlrLayerSurfaceV1 *layerSurface = nullptr;

protected:
    WaylandWlrLayerSurfaceV1Item *q_ptr = nullptr;
};

#endif // LIRI_WAYLANDWLRLAYERSURFACEV1ITEM_P_H
