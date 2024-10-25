// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtCore/qglobal.h>

#include "xcbwrapper.h"
#include "xcbresources.h"

namespace Xcb {

static xcb_connection_t *s_connection = nullptr;
static xcb_screen_t *s_screen = nullptr;
Resources *s_resources = nullptr;

xcb_connection_t *connection()
{
    return s_connection;
}

xcb_screen_t *screen()
{
    return s_screen;
}

xcb_window_t rootWindow()
{
    if (!s_screen)
        return XCB_WINDOW_NONE;
    return s_screen->root;
}

xcb_visualid_t rootVisual()
{
    if (!s_screen)
        return 0;
    return s_screen->root_visual;
}

Resources *resources()
{
    if (!s_resources)
        s_resources = new Resources();
    return s_resources;
}

void openConnection(int fd)
{
    if (s_connection)
        return;
    s_connection = xcb_connect_to_fd(fd, nullptr);

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(s_connection));
    s_screen = iter.data;
}

void closeConnection()
{
    if (s_connection) {
        delete s_resources;
        xcb_disconnect(s_connection);
        s_connection = nullptr;
    }
}

bool isOurResource(quint32 id)
{
    if (!s_connection)
        return false;
    const xcb_setup_t *setup = xcb_get_setup(s_connection);
    return (id & ~setup->resource_id_mask) == setup->resource_id_base;
}

} // namespace Xcb

