// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef WNDMGMT_H
#define WNDMGMT_H

#include <QUuid>

#include <QWaylandCompositorExtensionTemplate>
#include <QWaylandQuickExtension>
#include <QWaylandCompositor>
#include <QIcon>

#include "qwayland-server-plasma-window-management.h"

class Compositor;
class Surface;
class PlasmaWindowControl;
class PlasmaWindowManagement : public QWaylandCompositorExtensionTemplate<PlasmaWindowManagement>
        , public QtWaylandServer::org_kde_plasma_window_management
{
    Q_OBJECT
public:
    PlasmaWindowManagement(QWaylandCompositor *compositor = nullptr);
    void initialize() override;
    PlasmaWindowControl* createWindowControl(Surface *parent);
    void updateZOrder(const QString &uuids);
protected:
    void org_kde_plasma_window_management_bind_resource(Resource *resource) override;
    void org_kde_plasma_window_management_destroy_resource(Resource *resource) override;
    void org_kde_plasma_window_management_get_window(Resource *resource, uint32_t id, uint32_t internal_window_id) override;
    void org_kde_plasma_window_management_get_window_by_uuid(Resource *resource, uint32_t id, const QString &internal_window_uuid) override;

private:
    bool m_shellbound = false;
    Compositor *m_compositor = nullptr;
    friend class Compositor;
    QList<PlasmaWindowControl*> m_windows;
};


class PlasmaWindowControl : public QWaylandCompositorExtensionTemplate<PlasmaWindowControl>
        , public QtWaylandServer::org_kde_plasma_window
{
    Q_OBJECT
public:
    QUuid uuid() const;
public slots:
    void destroy();
    void setActive(bool set);
    void setMinimized(bool set);
    void setMaximized(bool set);
    void setMinimizable(bool set);
    void setMaximizable(bool set);
    void setCloseable(bool set);
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    void setThemedIcon(const QString &title);
signals:
    void setStateRequest(uint flags, uint state);
    void getIconRequest(int fd);
    void titleChanged(const QString &title);
    void themedIconChanged(const QString &icon);
    void closeRequested();
    void moveRequested();
    void resizeRequested();
    void activeRequested(bool set);
    void minimizedRequested(bool set);
    void maximizedRequested(bool set);
    void fullscreenRequested(bool set);
protected:
    friend class PlasmaWindowManagement;
    friend class Surface;
    friend class Compositor;
    PlasmaWindowControl(Surface *parent, const QUuid &uuid);
    ~PlasmaWindowControl();
    void org_kde_plasma_window_bind_resource(Resource *resource) override;
    void org_kde_plasma_window_set_state(Resource *resource, uint32_t flags, uint32_t state) override;
    void org_kde_plasma_window_set_virtual_desktop(Resource *resource, uint32_t number) override;
    void org_kde_plasma_window_get_icon(Resource *resource, int32_t fd) override;
    void org_kde_plasma_window_destroy(Resource *resource) override;
private:
    void unmap();
    void setState(org_kde_plasma_window_management_state flag, bool set);
private:
    Surface *m_parent = nullptr;
    QUuid m_uuid;
    QIcon m_icon;
    QString m_themedicon;
    QString m_title;
    quint32 m_state = 0;
    bool m_mapped = true;
};

#endif // WNDMGMT_H
