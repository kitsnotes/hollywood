// Hollywood Wayland Compositor
// (C) 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandCompositor>
#include <QWaylandResource>
#include <QWaylandPointer>

#include "qwayland-server-relative-pointer-unstable-v1.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwRelativePointer)

class Compositor;
class Surface;
class RelativePointerManagerV1;

class RelativePointerV1 : public QObject, public QtWaylandServer::zwp_relative_pointer_v1
{
    Q_OBJECT
public:
    RelativePointerV1(QWaylandPointer *pointer, wl_resource *resource, RelativePointerManagerV1 *parent);
    ~RelativePointerV1();
    void sendRelativeMotion(int dx, int dy, int dx_unaccel, int dy_unaccel, quint64 ts);
protected:
    void zwp_relative_pointer_v1_destroy(Resource *resource) override;
private:
    friend class RelativePointerManagerV1;
    RelativePointerManagerV1 *m_parent = nullptr;
    QWaylandPointer *m_pointer = nullptr;
};

class RelativePointerManagerV1 : public QWaylandCompositorExtensionTemplate<RelativePointerManagerV1>
    , public QtWaylandServer::zwp_relative_pointer_manager_v1
{
    Q_OBJECT
public:
    RelativePointerManagerV1(QWaylandCompositor *compositor);
    void initialize() override;
    QList<RelativePointerV1*> relativePointerForWaylandPointer(QWaylandPointer *pointer);
protected:
    void removePointer(RelativePointerV1 *pointer);
    void zwp_relative_pointer_manager_v1_get_relative_pointer(Resource *resource, uint32_t id, struct ::wl_resource *pointer) override;
private:
    friend class RelativePointerV1;
    Compositor *m_compositor = nullptr;
    QList<RelativePointerV1 *> m_relativePointers;
};
