// Hollywood Wayland Compositor
// (C) 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef APPMENU_H
#define APPMENU_H

#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandQuickExtension>
#include <QWaylandCompositor>

#include "qwayland-server-appmenu.h"

class Compositor;
class Surface;
class AppMenu;
class AppMenuManager : public QWaylandCompositorExtensionTemplate<AppMenuManager>
        , public QtWaylandServer::org_kde_kwin_appmenu_manager
{
    Q_OBJECT
public:
    AppMenuManager(QWaylandCompositor *compositor = nullptr);
    void initialize() override;
signals:
    void appMenuCreated(AppMenu *menu);
protected:
    void org_kde_kwin_appmenu_manager_create(Resource *resource, uint32_t id, struct ::wl_resource *surface) override;
private:
    Compositor *m_compositor;
    QVector<AppMenu *> m_appmenus;
};

class AppMenu : public QObject
        , public QtWaylandServer::org_kde_kwin_appmenu
{
    Q_OBJECT
public:
    QString serviceName() const;
    QString objectPath() const;
    Surface* surface() const;
signals:
    void addressChanged(const QString &service_name, const QString &object_path);
protected:
    void org_kde_kwin_appmenu_destroy_resource(Resource *resource) override;
    void org_kde_kwin_appmenu_set_address(Resource *resource, const QString &service_name, const QString &object_path) override;
    void org_kde_kwin_appmenu_release(Resource *resource) override;
private:
    friend class AppMenuManager;
    AppMenu(Surface *s, wl_resource *resource, AppMenuManager *parent);
    uint m_id;
    AppMenuManager *m_parent;
    Surface *m_surface;

    QString m_servicename;
    QString m_objectpath;
};

#endif // APPMENU_H
