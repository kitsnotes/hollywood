// Hollywood Wayland Compositor
// Based on the lirios/aurora-compositor implementation
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-FileCopyrightText: 2019-2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>
#include <wayland-server.h>
#include "qwayland-server-wlr-screencopy-unstable-v1.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwScreenCopy)

class Compositor;
class Surface;
class Output;
class WlrScreencopyFrameV1;
class WlrScreencopyManagerV1 : public QWaylandCompositorExtensionTemplate<WlrScreencopyManagerV1>
        , public QtWaylandServer::zwlr_screencopy_manager_v1
{
    Q_OBJECT
public:
    WlrScreencopyManagerV1(QWaylandCompositor *compositor);
    void initialize() override;
signals:
    void frameCaptureRequest(WlrScreencopyFrameV1 *frame);
protected:
    friend class WlrScreencopyFrameV1;
    void destroyFrame(WlrScreencopyFrameV1 *frame);
    void zwlr_screencopy_manager_v1_capture_output(Resource *resource, uint32_t frame, int32_t overlay_cursor, struct ::wl_resource *output) override;
    void zwlr_screencopy_manager_v1_capture_output_region(Resource *resource, uint32_t frame, int32_t overlay_cursor, struct ::wl_resource *output, int32_t x, int32_t y, int32_t width, int32_t height) override;
private:
    QList<WlrScreencopyFrameV1*> m_frames;
};

class WlrScreencopyFrameV1 : public QWaylandCompositorExtensionTemplate<WlrScreencopyFrameV1>
        , public QtWaylandServer::zwlr_screencopy_frame_v1
{
    Q_OBJECT
public:
    enum Flag {
        YInvert = 1
    };
    Q_ENUM(Flag)
    //Q_DECLARE_FLAGS(Flags, flag)

    Output* output() { return m_output; }
    QRect region() const { return m_region; }
    /*Flags flags() const;
    void setFlags(Flags flags);*/

    void copy();
protected:
    friend class WlrScreencopyManagerV1;
    WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor, Output *output);
    WlrScreencopyFrameV1(WlrScreencopyManagerV1 *parent, uint32_t frame, int32_t overlay_cursor, int32_t x, int32_t y, int32_t width, int32_t height, Output *output);
    void zwlr_screencopy_frame_v1_copy(Resource *resource, struct ::wl_resource *buffer) override;
    void zwlr_screencopy_frame_v1_destroy(Resource *resource) override;
    void zwlr_screencopy_frame_v1_copy_with_damage(Resource *resource, struct ::wl_resource *buffer) override;
    void setup();
signals:
    void ready();
private:
    void initCopy(Resource *resource, struct ::wl_resource *buffer, bool handleDamage);
private:
    WlrScreencopyManagerV1 *m_parent = nullptr;
    Output *m_output = nullptr;
    uint32_t m_frame;
    int32_t m_overlayCursor;
    bool m_capRegion = false;
    QRect m_region;
    bool m_ready = false;

    uint32_t m_stride = 0;
    bool m_withDamage = false;
    QRect m_damageRect;
    wl_shm_format m_requestedBufferFormat = WL_SHM_FORMAT_ARGB8888;

    quint32 m_tv_sec_hi = 0, m_tv_sec_lo = 0;

    struct ::wl_shm_buffer *m_buffer = nullptr;
};
