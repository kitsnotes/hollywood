// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "screencopy.h"
#include "hwc.h"
#include <wayland-server.h>

#define SCREENCOPY_VERSION  1

static inline QImage::Format fromWaylandShmFormat(wl_shm_format format)
{
    switch (format) {
    case WL_SHM_FORMAT_XRGB8888: return QImage::Format_RGB32;
    case WL_SHM_FORMAT_ARGB8888: return QImage::Format_ARGB32_Premultiplied;
    case WL_SHM_FORMAT_RGB565: return QImage::Format_RGB16;
    case WL_SHM_FORMAT_XRGB1555: return QImage::Format_RGB555;
    case WL_SHM_FORMAT_RGB888: return QImage::Format_RGB888;
    case WL_SHM_FORMAT_XRGB4444: return QImage::Format_RGB444;
    case WL_SHM_FORMAT_ARGB4444: return QImage::Format_ARGB4444_Premultiplied;
    case WL_SHM_FORMAT_XBGR8888: return QImage::Format_RGBX8888;
    case WL_SHM_FORMAT_ABGR8888: return QImage::Format_RGBA8888_Premultiplied;
    case WL_SHM_FORMAT_XBGR2101010: return QImage::Format_BGR30;
    case WL_SHM_FORMAT_ABGR2101010: return QImage::Format_A2BGR30_Premultiplied;
    case WL_SHM_FORMAT_XRGB2101010: return QImage::Format_RGB30;
    case WL_SHM_FORMAT_ARGB2101010: return QImage::Format_A2RGB30_Premultiplied;
    case WL_SHM_FORMAT_C8: return QImage::Format_Alpha8;
    default: return QImage::Format_Invalid;
    }
}

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

void WlrScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output(Resource *resource, uint32_t frame,
                            int32_t overlay_cursor, wl_resource *output)
{
    auto qwl = QWaylandOutput::fromResource(output);
    if(!qwl)
    {
        auto id = wl_resource_get_id(output);
        qWarning() << QString("Resource wl_output@%1 doesn't exist").arg(id);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_output@%d doesn't exist", id);
        return;
    }
    auto hwl = hwComp->outputFor(qwl);
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, hwl);
    m_frames.append(nf);
    emit frameCaptureRequest(nf);
}

void WlrScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output_region(Resource *resource, uint32_t frame,
              int32_t overlay_cursor, wl_resource *output, int32_t x, int32_t y, int32_t width, int32_t height)
{
    auto qwl = QWaylandOutput::fromResource(output);
    auto hwl = hwComp->outputFor(qwl);
    if(!qwl)
    {
        auto id = wl_resource_get_id(output);
        qWarning() << QString("Resource wl_output@%1 doesn't exist").arg(id);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_output@%d doesn't exist", id);
        return;
    }
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, hwl);
    m_frames.append(nf);
    emit  frameCaptureRequest(nf);
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor,
                                           Output *output)
    : QtWaylandServer::zwlr_screencopy_frame_v1()
    , m_parent(parent)
    , m_output(output)
    , m_frame(frame)
    , m_overlayCursor(overlay_cursor)
{
    m_capRegion = false;
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor,
                                           int32_t x, int32_t y, int32_t width, int32_t height, Output *output)
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

    if (format != m_requestedBufferFormat || width != m_region.width() ||
        height != m_region.height() || stride != stride) {
        qWarning("WlrScreencopyFrameV1: Invalid buffer attributes");
        wl_resource_post_error(resource->handle, error_invalid_buffer,
                               "invalid buffer attributes");
        return;
    }

    if (m_withDamage)
        send_damage(m_damageRect.x(), m_damageRect.y(), m_damageRect.width(), m_damageRect.height());

    m_ready = true;
    m_buffer = shm;
    emit ready();
}
