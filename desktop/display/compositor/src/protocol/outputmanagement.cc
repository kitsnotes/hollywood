// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "outputmanagement.h"
#include "compositor.h"

#define WLR_OUTPUT_MANAGER_VERSION 1

Q_LOGGING_CATEGORY(hwWlrOutput, "compositor.wlroutput")

WlrOutputManagerV1::WlrOutputManagerV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
    , QtWaylandServer::zwlr_output_manager_v1(compositor->display(), WLR_OUTPUT_MANAGER_VERSION)
{
    qCInfo(hwWlrOutput, "Supporting wlr_output_manager (protocol version %i)", WLR_OUTPUT_MANAGER_VERSION);
}

void WlrOutputManagerV1::setup()
{
    for(auto output : hwComp->outputManager()->outputs())
    {
        auto wlrhead = new WlrOutputHeadV1(output);
        m_heads.append(wlrhead);
        connect(wlrhead, &WlrOutputHeadV1::destroyed, this, [this,wlrhead]() {
            m_heads.removeOne(wlrhead);
        });
    }
}

void WlrOutputManagerV1::zwlr_output_manager_v1_bind_resource(Resource *resource)
{
    // send all the heads
    for(auto *head : m_heads)
    {
        auto *hr = head->add(resource->client(), interfaceVersion());
        send_head(resource->handle, hr->handle);
        head->blast(hr);
    }
    if(m_heads.count() > 0)
        send_done(resource->handle, hwComp->nextSerial());
}

void WlrOutputManagerV1::zwlr_output_manager_v1_create_configuration(Resource *resource, uint32_t id, uint32_t serial)
{
    Q_UNUSED(id)
    Q_UNUSED(serial)

    //auto config = new WlrOutputConfigurationV1(this, resource);
}

void WlrOutputManagerV1::zwlr_output_manager_v1_stop(Resource *resource)
{
    Q_UNUSED(resource)
}

WlrOutputHeadV1::WlrOutputHeadV1(Output *output)
    :m_output(output)
{
    for(auto mode : output->availableModes())
    {
        auto wlmode = new WlrOutputModeV1(mode);
        m_modes.append(wlmode);
        if(mode.size == output->currentMode().size() &&
            mode.refreshRate == output->currentMode().refreshRate())
            m_current = wlmode;
    }

    connect(output, &Output::positionChanged, this, [this, output]() {
        for(auto res : resourceMap())
            send_position(res->handle, output->position().x(), output->position().y());
    });
    connect(output, &Output::scaleFactorChanged, this, [this, output]() {
        for(auto res : resourceMap())
            send_scale(res->handle, output->scaleFactor());
    });
    connect(output, &Output::currentModeChanged, this, [this, output]() {
        auto mode = findMode(output->currentMode());
        if(mode)
        {
            for(auto res : resourceMap())
                send_current_mode(res->handle, mode->resource()->handle);
        }
    });
}

void WlrOutputHeadV1::blast(Resource *resource)
{
    send_name(resource->handle, m_output->screen()->name());
    send_description(resource->handle,
        QString("%1 %2").arg(m_output->manufacturer(), m_output->model()));
    send_make(resource->handle, m_output->screen()->manufacturer());
    send_model(resource->handle, m_output->screen()->model());
    send_serial_number(resource->handle, m_output->screen()->serialNumber());
    if(m_output->screen()->physicalSize().width() > 0 &&
        m_output->screen()->physicalSize().height() > 0)
        send_physical_size(resource->handle, m_output->screen()->physicalSize().width(),
                           m_output->screen()->physicalSize().height());

    bool enabled = true;
    send_enabled(resource->handle, enabled);
    if(enabled)
    {
        send_scale(resource->handle, m_output->scaleFactor());
        send_transform(resource->handle, 0 /*m_output->transform()*/);
        send_position(resource->handle, m_output->position().x(), m_output->position().y());
    }

    for(auto mode : m_modes)
    {
        auto mr = mode->add(resource->client(), mode->interfaceVersion());
        send_mode(resource->handle, mr->handle);
        mode->send_size(mr->handle, mode->size().width(), mode->size().height());
        mode->send_refresh(mr->handle, mode->refresh());
        if(m_current == mode)
            send_current_mode(resource->handle, mr->handle);

        // TODO: preferred
        mode->send_finished(mr->handle);
    }
    send_finished(resource->handle);
}

void WlrOutputHeadV1::zwlr_output_head_v1_release(Resource *)
{

}

WlrOutputModeV1 *WlrOutputHeadV1::findMode(const QWaylandOutputMode &mode)
{
    for(auto test : m_modes)
    {
        if(test->size() == mode.size() &&
            test->refresh() == mode.refreshRate())
            return test;
    }

    return nullptr;
}

WlrOutputModeV1::WlrOutputModeV1(QPlatformScreen::Mode mode)
    : m_size(mode.size)
    , m_refresh(mode.refreshRate)
{}

QSize WlrOutputModeV1::size() const
{
    return m_size;
}

qreal WlrOutputModeV1::refresh() const
{
    return m_refresh;
}

void WlrOutputModeV1::zwlr_output_mode_v1_release(Resource *resource)
{

}

WlrOutputConfigurationV1::WlrOutputConfigurationV1(WlrOutputManagerV1 *manager, const QWaylandResource &resource)
    : m_manager(manager)
{

}

void WlrOutputConfigurationV1::zwlr_output_configuration_v1_enable_head(Resource *resource, uint32_t id, wl_resource *head)
{
    /*auto head = findHead();
    m_head_config = new WlrOutputConfigurationHeadV1(output);*/
}

void WlrOutputConfigurationV1::zwlr_output_configuration_v1_disable_head(Resource *resource, wl_resource *head)
{

}

void WlrOutputConfigurationV1::zwlr_output_configuration_v1_apply(Resource *resource)
{

}

void WlrOutputConfigurationV1::zwlr_output_configuration_v1_test(Resource *resource)
{

}

void WlrOutputConfigurationV1::zwlr_output_configuration_v1_destroy(Resource *resource)
{

}


WlrOutputConfigurationHeadV1::WlrOutputConfigurationHeadV1(WlrOutputHeadV1 *head)
{

}

