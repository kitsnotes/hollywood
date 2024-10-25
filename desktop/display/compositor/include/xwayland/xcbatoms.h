// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "xcbatom.h"

namespace Xcb {

class Atoms
{
public:
    Atoms();

    Atom wm_protocols;
    Atom wm_normal_hints;
    Atom wm_take_focus;
    Atom wm_delete_window;
    Atom wm_state;
    Atom wm_change_state;
    Atom wm_s0;
    Atom wm_client_machine;
    Atom net_wm_cm_s0;
    Atom net_wm_name;
    Atom net_wm_pid;
    Atom net_wm_icon;
    Atom net_wm_state;
    Atom net_wm_state_maximized_vert;
    Atom net_wm_state_maximized_horz;
    Atom net_wm_state_fullscreen;
    Atom net_wm_user_time;
    Atom net_wm_icon_name;
    Atom net_wm_desktop;
    Atom net_wm_window_type;
    Atom net_wm_window_type_desktop;
    Atom net_wm_window_type_dock;
    Atom net_wm_window_type_toolbar;
    Atom net_wm_window_type_menu;
    Atom net_wm_window_type_utility;
    Atom net_wm_window_type_splash;
    Atom net_wm_window_type_dialog;
    Atom net_wm_window_type_dropdown;
    Atom net_wm_window_type_popup;
    Atom net_wm_window_type_tooltip;
    Atom net_wm_window_type_notification;
    Atom net_wm_window_type_combo;
    Atom net_wm_window_type_dnd;
    Atom net_wm_window_type_normal;
    Atom net_wm_moveresize;
    Atom net_supporting_wm_check;
    Atom net_supported;
    Atom net_active_window;
    Atom motif_wm_hints;
    Atom clipboard;
    Atom clipboard_manager;
    Atom targets;
    Atom utf8_string;
    Atom wl_selection;
    Atom incr;
    Atom timestamp;
    Atom multiple;
    Atom compound_text;
    Atom text;
    Atom string;
    Atom window;
    Atom text_plain_utf8;
    Atom text_plain;
    Atom xdnd_selection;
    Atom xdnd_aware;
    Atom xdnd_enter;
    Atom xdnd_leave;
    Atom xdnd_drop;
    Atom xdnd_status;
    Atom xdnd_finished;
    Atom xdnd_type_list;
    Atom xdnd_action_copy;
    Atom wl_surface_id;
};

} // namespace Xcb

