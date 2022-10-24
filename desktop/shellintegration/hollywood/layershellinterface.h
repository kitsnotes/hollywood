/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef LAYERSHELLQTSHELL_H
#define LAYERSHELLQTSHELL_H

#include "layershellqt_export.h"
#include "layershellwindow.h"
#include <QString>

namespace QtWaylandClient
{
/**
 * Sets the right environment so the shells created from now on use wlr-layer-shell.
 */
class LAYERSHELLQT_EXPORT Shell
{
public:
    static void useLayerShell();
};

}

#endif
