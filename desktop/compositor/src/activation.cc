// Hollywood Wayland Compositor
// (C) 2023 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "activation.h"
#include "hwc.h"

#include <QUuid>

#define XDG_ACTIVATION_VERSION 1

XdgActivation::XdgActivation(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<XdgActivation>(compositor)
    , QtWaylandServer::xdg_activation_v1(compositor->display(), XDG_ACTIVATION_VERSION)
{}

void XdgActivation::destroyToken(XdgActivationToken *token)
{
    m_tokens.removeOne(token);
    token->deleteLater();
}

void XdgActivation::xdg_activation_v1_get_activation_token(Resource *resource, uint32_t id)
{
    auto *token = new XdgActivationToken;
    token->init(resource->client(),  id, resource->version());
    token->m_parent = this;
    m_tokens.append(token);
}

void XdgActivation::xdg_activation_v1_activate(Resource *resource, const QString &token, wl_resource *surface)
{
    // find the item in m_tokens that matches token
    for(auto tk : m_tokens)
    {
        if(tk->m_token == token)
        {
            if(tk->m_used)
            {
                wl_resource_post_error(resource->handle,
                                       QtWaylandServer::xdg_activation_token_v1::error_already_used,
                                       "xdg_activation_v1_activate: token has already been used");

                return;
            }
            auto *qSurface = QWaylandSurface::fromResource(surface);
            if(qSurface)
            {
                if(tk->m_valid)
                    emit surfaceActivated(qSurface);

                tk->m_used = true;
            }
        }
    }
}

void XdgActivationToken::xdg_activation_token_v1_set_serial(Resource *resource, uint32_t serial, wl_resource *seat)
{
    // set the activating seat/serial (optional)
    Q_UNUSED(resource)
    m_serial = serial;
    m_seat = QWaylandSeat::fromSeatResource(seat);
}

void XdgActivationToken::xdg_activation_token_v1_set_app_id(Resource *resource, const QString &app_id)
{
    // set the activating app id (optional)
    Q_UNUSED(resource)
    m_app_id = app_id;
}

void XdgActivationToken::xdg_activation_token_v1_set_surface(Resource *resource, wl_resource *surface)
{
    // set the activating surface (optional)
    Q_UNUSED(resource)
    m_surface = QWaylandSurface::fromResource(surface);
}

void XdgActivationToken::xdg_activation_token_v1_commit(Resource *resource)
{
    // Generate a UUID token and generate it
    Q_UNUSED(resource)
    m_token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_valid = true;
    send_done(m_token);
}

void XdgActivationToken::xdg_activation_token_v1_destroy(Resource *resource)
{
    // Destroy this object
    wl_resource_destroy(resource->handle);
    m_parent->destroyToken(this);
}

