// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-wlr-output-management-unstable-v1.h"

#include "output.h"
#include "outputmanager.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwWlrOutput)

class Compositor;
class Surface;
class WlrOutputHeadV1;
class WlrOutputModeV1;
class WlrOutputConfigurationV1;
class WlrOutputConfigurationHeadV1;

class WlrOutputManagerV1 : public QWaylandCompositorExtensionTemplate<WlrOutputManagerV1>
    , public QtWaylandServer::zwlr_output_manager_v1
{
    Q_OBJECT
public:
    WlrOutputManagerV1(QWaylandCompositor *compositor);
    void setup();
protected:
    void zwlr_output_manager_v1_bind_resource(Resource *resource) override;
    void zwlr_output_manager_v1_create_configuration(Resource *resource, uint32_t id, uint32_t serial) override;
    void zwlr_output_manager_v1_stop(Resource *resource) override;
private:
    QList<WlrOutputHeadV1*> m_heads;
};

class WlrOutputHeadV1 : public QWaylandCompositorExtensionTemplate<WlrOutputHeadV1>
    , public QtWaylandServer::zwlr_output_head_v1
{
    Q_OBJECT
public:
    WlrOutputHeadV1(Output *output);
    void blast(Resource *resource);
signals:
    void release();
protected:
    void zwlr_output_head_v1_release(Resource *resource) override;
private:
    WlrOutputModeV1 *findMode(const QWaylandOutputMode &mode);
    Output *m_output = nullptr;
    QList<WlrOutputModeV1*> m_modes;
    WlrOutputModeV1 *m_current;
    WlrOutputModeV1 *m_preferred;
};

class WlrOutputModeV1 : public QWaylandCompositorExtensionTemplate<WlrOutputModeV1>
    , public QtWaylandServer::zwlr_output_mode_v1
{
    Q_OBJECT
public:
    WlrOutputModeV1(QPlatformScreen::Mode mode);
    QSize size() const;
    qreal refresh() const;
protected:
    void zwlr_output_mode_v1_release(Resource *resource) override;
private:
    QSize m_size;
    qreal m_refresh;
};

class WlrOutputConfigurationV1 : public QWaylandCompositorExtensionTemplate<WlrOutputConfigurationV1>
    , public QtWaylandServer::zwlr_output_configuration_v1
{
    Q_OBJECT
public:
    WlrOutputConfigurationV1(WlrOutputManagerV1 *manager, const QWaylandResource &resource);
signals:
protected:
    void zwlr_output_configuration_v1_enable_head(Resource *resource, uint32_t id, struct ::wl_resource *head) override;
    void zwlr_output_configuration_v1_disable_head(Resource *resource, struct ::wl_resource *head) override;
    void zwlr_output_configuration_v1_apply(Resource *resource) override;
    void zwlr_output_configuration_v1_test(Resource *resource) override;
    void zwlr_output_configuration_v1_destroy(Resource *resource) override;
private:
    WlrOutputManagerV1 *m_manager = nullptr;
    WlrOutputConfigurationHeadV1 *m_head_config = nullptr;
};

class WlrOutputConfigurationHeadV1 : public QWaylandCompositorExtensionTemplate<WlrOutputConfigurationHeadV1>
    , public QtWaylandServer::zwlr_output_configuration_head_v1
{
    Q_OBJECT
public:
    WlrOutputConfigurationHeadV1(WlrOutputHeadV1 *head);
signals:
protected:
};
