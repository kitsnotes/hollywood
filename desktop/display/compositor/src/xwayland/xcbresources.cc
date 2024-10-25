// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "xcbwrapper.h"
#include "xcbresources.h"
#include "xwayland.h"

#include <xcb/xfixes.h>
#include <xcb/composite.h>

namespace Xcb {

Resources::Resources()
{
    xcb_prefetch_extension_data(connection(), &xcb_xfixes_id);
    xcb_prefetch_extension_data(connection(), &xcb_composite_id);

    xcb_render_query_pict_formats_cookie_t formatsCookie =
            xcb_render_query_pict_formats(connection());

    atoms = new Xcb::Atoms();

    xfixes = xcb_get_extension_data(connection(), &xcb_xfixes_id);
    if (!xfixes || !xfixes->present)
        qCWarning(gLcXwayland) << "xfixes not available";

    xcb_xfixes_query_version_cookie_t xfixesCookie =
            xcb_xfixes_query_version(connection(),
                                     XCB_XFIXES_MAJOR_VERSION,
                                     XCB_XFIXES_MINOR_VERSION);
    xcb_xfixes_query_version_reply_t *xfixesReply =
            xcb_xfixes_query_version_reply(connection(), xfixesCookie, nullptr);

    qCDebug(gLcXwayland, "xfixes version: %d.%d",
            xfixesReply->major_version,
            xfixesReply->minor_version);

    free(xfixesReply);

    xcb_render_query_pict_formats_reply_t *formatsReply =
            xcb_render_query_pict_formats_reply(connection(), formatsCookie, 0);
    if (formatsReply) {
        xcb_render_pictforminfo_t *formats =
                xcb_render_query_pict_formats_formats(formatsReply);
        for (quint32 i = 0; i < formatsReply->num_formats; i++) {
            if (formats[i].direct.red_mask != 0xff &&
                    formats[i].direct.red_shift != 16)
                continue;
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                    formats[i].depth == 24)
                formatRgb = formats[i];
            if (formats[i].type == XCB_RENDER_PICT_TYPE_DIRECT &&
                    formats[i].depth == 32 &&
                    formats[i].direct.alpha_mask == 0xff &&
                    formats[i].direct.alpha_shift == 24)
                formatRgba = formats[i];
        }

        free(formatsReply);
    }
}

Resources::~Resources()
{
    delete atoms;
}

} // namespace Xcb

