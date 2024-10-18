// Hollywood Wayland Compositor
// (C) 2021-2024 Originull Software
// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include "xdgdialog.h"
#include "qwaylandcompositor.h"

Q_LOGGING_CATEGORY(hwXdgDialog, "compositor.xdgdialog")

HWWaylandXdgDialogV1Global::HWWaylandXdgDialogV1Global(QWaylandCompositor *parent)
    : QWaylandCompositorExtensionTemplate<HWWaylandXdgDialogV1Global>(parent)
{
}

void HWWaylandXdgDialogV1Global::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (compositor)
        init(compositor->display(), 1);
}

void HWWaylandXdgDialogV1Global::xdg_wm_dialog_v1_get_xdg_dialog(Resource *resource, uint32_t id, wl_resource *toplevelResource)
{
    auto *toplevel = HWWaylandXdgToplevel::fromResource(toplevelResource);
    (void)new HWWaylandXdgDialogV1(toplevel, resource->client(), id);
}

HWWaylandXdgDialogV1::HWWaylandXdgDialogV1(HWWaylandXdgToplevel *toplevel, wl_client *client, int id)
    : QtWaylandServer::xdg_dialog_v1(client, id, 1), m_topLevel(toplevel)
{
}

void HWWaylandXdgDialogV1::xdg_dialog_v1_set_modal(Resource *resource)
{
    Q_UNUSED(resource);
    if (m_topLevel)
        m_topLevel->setModal(true);

}

void HWWaylandXdgDialogV1::xdg_dialog_v1_unset_modal(Resource *resource)
{
    Q_UNUSED(resource);
    if (m_topLevel)
        m_topLevel->setModal(false);
}

void HWWaylandXdgDialogV1::xdg_dialog_v1_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource);
    delete this;
}

void HWWaylandXdgDialogV1::xdg_dialog_v1_destroy(Resource *resource)
{
    if (m_topLevel)
        m_topLevel->setModal(false);
    wl_resource_destroy(resource->handle);
}

