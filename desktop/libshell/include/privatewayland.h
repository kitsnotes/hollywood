// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QGuiApplication>
#include <QtGui/QWindow>

#include <QtWaylandClient/QWaylandClientExtensionTemplate>
#include <wayland-client-protocol.h>

#include "libshell_int.h"
#include "qwayland-originull-privateapi.h"

class QWaylandDisplay;
class QWaylandIntegration;

class OriginullMenuServerClient;
class LIBSHELL_EXPORT HWPrivateWaylandProtocol
        : public QWaylandClientExtensionTemplate<HWPrivateWaylandProtocol>
        , public QtWayland::org_originull_privateapi
{
    Q_OBJECT
public:
    HWPrivateWaylandProtocol();
    OriginullMenuServerClient* createMenuServerResponder();
    void rotateWallpaper();
    void registerDesktopSurface(QWindow *window);
private:
    void windowForSurface();
};

class LIBSHELL_EXPORT OriginullMenuServerClient
        : public QWaylandClientExtensionTemplate<OriginullMenuServerClient>
        , public QtWayland::org_originull_menuserver
{
    Q_OBJECT
public:
    OriginullMenuServerClient(struct ::org_originull_menuserver *menu);
protected:
    void org_originull_menuserver_appmenu_top_level_window_changed(const QString &service_name, const QString &object_path) override;
signals:
    void menuChanged(const QString &serviceName, const QString &objectPath);
private:
    void windowForSurface();
};
