// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "xcbatoms.h"

#include <xcb/render.h>

namespace Xcb {

class Resources
{
public:
    Resources();
    ~Resources();

    const xcb_query_extension_reply_t *xfixes;
    Atoms *atoms;
    xcb_render_pictforminfo_t formatRgb;
    xcb_render_pictforminfo_t formatRgba;
};

} // namespace Xcb

