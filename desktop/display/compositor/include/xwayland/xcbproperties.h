// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <xcb/xcb.h>

namespace Xcb {

namespace Properties {

#if 0
void dump(xcb_atom_t property, xcb_get_property_reply_t *reply);
#endif
void dumpProperty(xcb_atom_t property, xcb_get_property_reply_t *reply);
void readAndDumpProperty(xcb_atom_t atom, xcb_window_t window);

} // namespace Cursors

} // namespace Xcb

