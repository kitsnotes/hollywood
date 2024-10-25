// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>

#include <xcb/xcb.h>

namespace Xcb {

class Resources;

xcb_connection_t *connection();
xcb_screen_t *screen();
xcb_window_t rootWindow();
xcb_visualid_t rootVisual();
Resources *resources();

void openConnection(int fd);
void closeConnection();

bool isOurResource(quint32 id);

} // namespace Xcb

