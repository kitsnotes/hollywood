// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GTKSHELL_H
#define GTKSHELL_H

#include <QWaylandCompositorExtensionTemplate>
//#include <QWaylandQuickExtension>
#include <QWaylandCompositor>
#include <QWaylandResource>

#include "qwayland-server-gtk.h"

class Compositor;
class Surface;
class GtkSurface;
class GtkShell : public QWaylandCompositorExtensionTemplate<GtkShell>
               , public QtWaylandServer::gtk_shell1
{
    Q_OBJECT
public:
    GtkShell(QWaylandCompositor *compositor);

signals:
    void surfaceRequested(QWaylandSurface *surface, const QWaylandResource &resource);
    void surfaceCreated(GtkSurface *gtk);
protected:
    void gtk_shell1_bind_resource(Resource *res) override;
    void gtk_shell1_get_gtk_surface(Resource *res, uint32_t id,
                                   wl_resource *sr)  override;
};

class GtkSurface : public QObject
                 , public QtWaylandServer::gtk_surface1
{
    Q_OBJECT
public:
    GtkSurface(GtkShell *shell, QWaylandSurface *surface,
               const QWaylandResource &resource,
               QObject *parent = nullptr);
    ~GtkSurface();

    Q_INVOKABLE void initialize(GtkShell *shell, QWaylandSurface *surface,
                                    const QWaylandResource &resource);

    QWaylandSurface *surface() const;
    GtkShell *shell() const;

    QString appId() const;
    QString appMenuPath() const;
    QString menuBarPath() const;
    QString windowObjectPath() const;
    QString appObjectPath() const;
    QString uniqueBusName() const;

    static GtkSurface *fromResource(wl_resource *res);
protected:
    void gtk_surface1_destroy_resource(Resource *res) override;

    void gtk_surface1_set_dbus_properties(Resource *res,
                                     const QString &application_id,
                                     const QString &app_menu_path,
                                     const QString &menubar_path,
                                     const QString &window_object_path,
                                     const QString &application_object_path,
                                     const QString &unique_bus_name) override;

    void gtk_surface1_set_modal(Resource *resource) override;
    void gtk_surface1_unset_modal(Resource *resource) override;
signals:
    void surfaceChanged();
    void shellChanged();

    void appIdChanged(const QString &appId);
    void appMenuPathChanged(const QString &appMenuPath);
    void menuBarPathChanged(const QString &menuBarPath);
    void windowObjectPathChanged(const QString &windowObjectPath);
    void appObjectPathChanged(const QString &appObjectPath);
    void uniqueBusNameChanged(const QString &uniqueBusName);

    void setModal();
    void unsetModal();
private:
    GtkShell *m_shell;
    QWaylandSurface *m_surface;

    QString m_appId;
    QString m_appMenuPath;
    QString m_menuBarPath;
    QString m_windowObjectPath;
    QString m_appObjectPath;
    QString m_uniqueBusName;
};

#endif // GTKSHELL_H
