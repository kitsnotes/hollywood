// Hollywood Wayland Compositor
// (C) 2023 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-xdg-activation-v1.h"

class Compositor;
class Surface;
class XdgActivationToken;
class XdgActivation : public QWaylandCompositorExtensionTemplate<XdgActivation>
    , public QtWaylandServer::xdg_activation_v1
{
    Q_OBJECT
public:
    XdgActivation(QWaylandCompositor *compositor);
signals:
    void surfaceActivated(QWaylandSurface *surface);
protected:
    friend class XdgActivationToken;
    void destroyToken(XdgActivationToken* token);
protected:
    void xdg_activation_v1_get_activation_token(Resource *resource, uint32_t id) override;
    void xdg_activation_v1_activate(Resource *resource, const QString &token, struct ::wl_resource *surface) override;
private:
    QVector<XdgActivationToken*> m_tokens;
};

class XdgActivationToken : public QWaylandCompositorExtensionTemplate<XdgActivationToken>
    , public QtWaylandServer::xdg_activation_token_v1
{
    Q_OBJECT
protected:
    void xdg_activation_token_v1_set_serial(Resource *resource, uint32_t serial, struct ::wl_resource *seat) override;
    void xdg_activation_token_v1_set_app_id(Resource *resource, const QString &app_id) override;
    void xdg_activation_token_v1_set_surface(Resource *resource, struct ::wl_resource *surface) override;
    void xdg_activation_token_v1_commit(Resource *resource) override;
    void xdg_activation_token_v1_destroy(Resource *resource) override;
private:
    friend class XdgActivation;
    QWaylandSurface *m_surface = nullptr;
    QString m_app_id;
    uint32_t m_serial;
    QWaylandSeat *m_seat = nullptr;
    QString m_token;

    bool m_valid = false;
    bool m_used = false;

    XdgActivation* m_parent = nullptr;
};

#endif // ACTIVATION_H
