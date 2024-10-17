// Hollywood Wayland Compositor
// (C) 2021-2024 Originull Software
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#pragma once
#include "xdgshell.h"
#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/private/qwaylandcompositorextension_p.h>
#include <QtWaylandCompositor/private/qwayland-server-xdg-dialog-v1.h>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwXdgDialog)

class HWWaylandXdgDialogV1Global
    : public QWaylandCompositorExtensionTemplate<HWWaylandXdgDialogV1Global>
    , public QtWaylandServer::xdg_wm_dialog_v1
{
    Q_OBJECT
public:
    HWWaylandXdgDialogV1Global(QWaylandCompositor *parent = nullptr);

    void initialize() override;

protected:
    void xdg_wm_dialog_v1_get_xdg_dialog(Resource *resource, uint32_t id, wl_resource *toplevelResource) override;
};

class  HWWaylandXdgDialogV1
    : public QtWaylandServer::xdg_dialog_v1
{
public:
    HWWaylandXdgDialogV1(HWWaylandXdgToplevel  *toplevel, wl_client *client, int id);

protected:
    void xdg_dialog_v1_destroy_resource(Resource *resource) override;
    void xdg_dialog_v1_destroy(Resource *resource) override;

    void xdg_dialog_v1_set_modal(Resource *resource) override;
    void xdg_dialog_v1_unset_modal(Resource *resource) override;

private:
    QPointer<HWWaylandXdgToplevel > m_topLevel;
};
