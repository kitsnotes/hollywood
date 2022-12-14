// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "screencopy.h"
#include "hwc.h"

#define SCREENCOPY_VERSION  1
WlrScreencopyManagerV1::WlrScreencopyManagerV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<WlrScreencopyManagerV1>(compositor)
    , QtWaylandServer::zwlr_screencopy_manager_v1(compositor->display(), SCREENCOPY_VERSION)
{}

void WlrScreencopyManagerV1::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), SCREENCOPY_VERSION);
}

void WlrScreencopyManagerV1::destroyFrame(WlrScreencopyFrameV1 *frame)
{
    m_frames.removeOne(frame);
    frame->deleteLater();
}

void WlrScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output(Resource *resource, uint32_t frame, int32_t overlay_cursor, wl_resource *output)
{
    Q_UNUSED(resource);
    auto qwl = QWaylandOutput::fromResource(output);
    auto hwl = hwComp->outputFor(qwl);
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, hwl);
    m_frames.append(nf);
    emit(frameCaptureRequest(nf));
}

void WlrScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output_region(Resource *resource, uint32_t frame, int32_t overlay_cursor, wl_resource *output, int32_t x, int32_t y, int32_t width, int32_t height)
{
    Q_UNUSED(resource);
    auto qwl = QWaylandOutput::fromResource(output);
    auto hwl = hwComp->outputFor(qwl);
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, hwl);
    m_frames.append(nf);
    emit(frameCaptureRequest(nf));
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor, Output *output)
    : QtWaylandServer::zwlr_screencopy_frame_v1()
    , m_parent(parent)
    , m_output(output)
    , m_frame(frame)
    , m_overlayCursor(overlay_cursor)
{
    m_capRegion = false;
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor, int32_t x, int32_t y, int32_t width, int32_t height, Output *output)
    : QtWaylandServer::zwlr_screencopy_frame_v1()
    , m_parent(parent)
    , m_output(output)
    , m_frame(frame)
    , m_overlayCursor(overlay_cursor)
{
    m_region = QRect(x,y,width,height);
    m_capRegion = true;
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_copy(Resource *resource, wl_resource *buffer)
{
    handleCopy(resource, buffer, false);
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_destroy(Resource *resource)
{
    Q_UNUSED(resource);
    m_parent->destroyFrame(this);
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_copy_with_damage(Resource *resource, wl_resource *buffer)
{
    handleCopy(resource, buffer, true);
}

void WlrScreencopyFrameV1::handleCopy(Resource *resource, wl_resource *buffer, bool handleDamage)
{
    // TODO: fix this to handle damage regions
    Q_UNUSED(handleDamage);

    auto *shm = wl_shm_buffer_get(buffer);
    if(!shm)
    {
        //wl_resource_post_error(resource, error_invalid_buffer, "invalid buffer type specified by client");
        return;
    }

    uint32_t format = wl_shm_buffer_get_format(shm);
    int32_t width = wl_shm_buffer_get_width(shm);
    int32_t height = wl_shm_buffer_get_height(shm);
    int32_t stride = wl_shm_buffer_get_stride(shm);


}
