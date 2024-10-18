// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTSHELL_H
#define QTSHELL_H

#include <QWaylandCompositorExtensionTemplate>
//#include <QWaylandQuickExtension>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-qt-shell-unstable-v1.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(hwQtShell)

class Compositor;
class Surface;
class QtSurface;
class QtShell : public QWaylandCompositorExtensionTemplate<QtShell>
               , public QtWaylandServer::zqt_shell_v1
{
    Q_OBJECT
public:
    QtShell(QWaylandCompositor *compositor);

signals:
    void surfaceRequested(QWaylandSurface *surface, const QWaylandResource &resource);
    void surfaceCreated(QtSurface *qtsurface);
protected:
    void zqt_shell_v1_surface_create(Resource *resource, struct ::wl_resource *surface, uint32_t id);
};

class QWaylandSurfaceRole;
class QtSurface : public QObject
                 , public QtWaylandServer::zqt_shell_surface_v1
{
    Q_OBJECT
public:
    QtSurface(QtShell *shell, QWaylandSurface *surface,
               const QWaylandResource &resource,
               QObject *parent = nullptr);
    ~QtSurface();

    Q_INVOKABLE void initialize(QtShell *shell, QWaylandSurface *surface,
                                    const QWaylandResource &resource);

    QWaylandSurface *surface() const;
    QtShell *shell() const;

    static QtSurface *fromResource(wl_resource *res);
    static QWaylandSurfaceRole *role();
protected:
    void zqt_shell_surface_v1_reposition(Resource *resource, int32_t x, int32_t y) override;
    void zqt_shell_surface_v1_request_activate(Resource *resource) override;
    void zqt_shell_surface_v1_set_size(Resource *resource, int32_t width, int32_t height) override;
    void zqt_shell_surface_v1_set_minimum_size(Resource *resource, int32_t width, int32_t height) override;
    void zqt_shell_surface_v1_set_maximum_size(Resource *resource, int32_t width, int32_t height) override;
    void zqt_shell_surface_v1_set_window_title(Resource *resource, const QString &window_title) override;
    void zqt_shell_surface_v1_set_window_flags(Resource *resource, uint32_t flags) override;
    void zqt_shell_surface_v1_start_system_resize(Resource *resource, uint32_t serial, uint32_t edge) override;
    void zqt_shell_surface_v1_start_system_move(Resource *resource, uint32_t serial) override;
    void zqt_shell_surface_v1_change_window_state(Resource *resource, uint32_t state) override;
    void zqt_shell_surface_v1_raise(Resource *resource) override;
    void zqt_shell_surface_v1_lower(Resource *resource) override;
    void zqt_shell_surface_v1_ack_configure(Resource *resource, uint32_t serial) override;
signals:
    void surfaceChanged();
    void shellChanged();
    void windowTitleChanged(const QString &title);
    void reposition(const QPoint &pos);
    void requestActivate();
    void setSize(const QSize &size);
    void requestRaise();
    void requestLower();
    void windowFlagsChanged(const Qt::WindowFlags &f);
private:
    QtShell *m_shell;
    QWaylandSurface *m_surface;
    static QWaylandSurfaceRole s_role;

};

#endif // QTSHELL_H
