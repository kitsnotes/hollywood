// Hollywood Wayland Compositor
// Based on the lirios/aurora-compositor implementation
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "compositor.h"
#include "screencopy.h"
#include "output.h"
#include "outputwnd.h"

#include <wayland-server.h>

#define SCREENCOPY_VERSION  1

Q_LOGGING_CATEGORY(hwScreenCopy, "compositor.screencopy")

WlrScreencopyManagerV1::WlrScreencopyManagerV1(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<WlrScreencopyManagerV1>(compositor)
    , QtWaylandServer::zwlr_screencopy_manager_v1(compositor->display(), SCREENCOPY_VERSION)
{
    qCInfo(hwScreenCopy, "Supporting wlr_screencopy_manager_v1 (protocol version %i)", SCREENCOPY_VERSION);
}

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
        qCWarning(hwScreenCopy, "Resource wl_output@%d doesn't exist", id);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_output@%d doesn't exist", id);
        return;
    }
    auto hwl = hwComp->outputFor(qwl);
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, hwl);
    nf->init(resource->client(), frame, resource->version());
    m_frames.append(nf);
    hwl->hwWindow()->setupScreenCopyFrame(nf);

    emit frameCaptureRequest(nf);
    nf->setup();
}

void WlrScreencopyManagerV1::zwlr_screencopy_manager_v1_capture_output_region(Resource *resource, uint32_t frame,
              int32_t overlay_cursor, wl_resource *output, int32_t x, int32_t y, int32_t width, int32_t height)
{
    auto qwl = QWaylandOutput::fromResource(output);
    auto hwl = hwComp->outputFor(qwl);
    if(!qwl)
    {
        auto id = wl_resource_get_id(output);
        qCWarning(hwScreenCopy, "Resource wl_output@%d doesn't exist", id);
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "resource wl_output@%d doesn't exist", id);
        return;
    }
    WlrScreencopyFrameV1 *nf = new WlrScreencopyFrameV1(this, frame, overlay_cursor, x, y, width, height, hwl);
    nf->init(resource->client(), frame, resource->version());
    m_frames.append(nf);
    hwl->hwWindow()->setupScreenCopyFrame(nf);

    emit frameCaptureRequest(nf);
    nf->setup();
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor,
                                           Output *output)
    : QtWaylandServer::zwlr_screencopy_frame_v1()
    , m_parent(parent)
    , m_output(output)
    , m_frame(frame)
    , m_overlayCursor(overlay_cursor)
{
    qCInfo(hwScreenCopy, "Setup new WlrScreencopyFrameV1 for full output");
    m_region = m_output->window()->geometry();
    m_capRegion = false;
    m_stride = 4 * m_output->window()->geometry().width();
}

WlrScreencopyFrameV1::WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor,
                                           int32_t x, int32_t y, int32_t width, int32_t height, Output *output)
    : QtWaylandServer::zwlr_screencopy_frame_v1()
    , m_parent(parent)
    , m_output(output)
    , m_frame(frame)
    , m_overlayCursor(overlay_cursor)
{
    qCInfo(hwScreenCopy, "Setup new WlrScreencopyFrameV1 for region");
    m_region = QRect(x,y,width,height);
    m_capRegion = true;
    m_stride = 4 * m_output->window()->geometry().width();
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_copy(Resource *resource, wl_resource *buffer)
{
    initCopy(resource, buffer, false);
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_destroy(Resource *resource)
{
    Q_UNUSED(resource);
    m_parent->destroyFrame(this);
}

void WlrScreencopyFrameV1::zwlr_screencopy_frame_v1_copy_with_damage(Resource *resource, wl_resource *buffer)
{
    initCopy(resource, buffer, true);
}

void WlrScreencopyFrameV1::setup()
{
    send_buffer(m_requestedBufferFormat, m_region.width(), m_region.height(), m_stride);
    QDateTime dateTime(QDateTime::currentDateTimeUtc());
    qint64 secs = dateTime.toSecsSinceEpoch();
    m_tv_sec_hi = secs >> 32;
    m_tv_sec_lo = secs & 0xffffffff;
}

void WlrScreencopyFrameV1::initCopy(Resource *resource, wl_resource *buffer, bool handleDamage)
{
    Q_UNUSED(handleDamage)
    auto *shm = wl_shm_buffer_get(buffer);
    if(!shm)
    {
        qCWarning(hwScreenCopy, "initCopy: invalid buffer type specified by client");
        //wl_resource_post_error(resource, error_invalid_buffer, "invalid buffer type specified by client");
        return;
    }

    uint32_t format = wl_shm_buffer_get_format(shm);
    int32_t width = wl_shm_buffer_get_width(shm);
    int32_t height = wl_shm_buffer_get_height(shm);
    int32_t stride = wl_shm_buffer_get_stride(shm);

    if (format != m_requestedBufferFormat || width != m_region.width() ||
        height != m_region.height() || (uint32_t)stride != m_stride) {
        qCWarning(hwScreenCopy, "initCopy: Invalid buffer attributes format:%i width:%i height:%i stride:%i expecting %i %i %i %i",
                                format, width, height, stride, m_requestedBufferFormat, m_region.width(), m_region.height(), m_stride);
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


void WlrScreencopyFrameV1::copy()
{
    // needs to be called in the GL pipeline
    if(m_ready)
    {
        wl_shm_buffer_begin_access(m_buffer);
        void *data = wl_shm_buffer_get_data(m_buffer);

        QRect rect = m_region.translated(m_output->position());

        qCDebug(hwScreenCopy, "reading data from compositor start (%ix%i) w/h (%ix%i)",
                rect.x(), rect.y(), rect.width(), rect.height());

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        QImage img(rect.size(), QImage::Format_RGBA8888_Premultiplied);
        glReadPixels(rect.x(), rect.y(), rect.width(), rect.height(),
                     GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        img.convertTo(QImage::Format_ARGB32_Premultiplied);
        memcpy(data, img.bits(), img.sizeInBytes());
        wl_shm_buffer_end_access(m_buffer);
        send_flags(static_cast<uint32_t>(1));
        send_ready(m_tv_sec_hi, m_tv_sec_lo, 0);
    }
}
