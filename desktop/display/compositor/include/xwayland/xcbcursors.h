// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <xcb/xcb.h>

namespace Xcb {

namespace Cursors {

xcb_cursor_t *createCursors();
void destroyCursors(xcb_cursor_t *c);

} // namespace Cursors

} // namespace Xcb

