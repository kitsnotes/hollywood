/*  Originated from KDE
    SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H

#include <QHash>
#include <QObject>
#include <QDebug>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>
#include "qwayland-appmenu.h"

class QWindow;
class HollywoodPlatformTheme;
class AppMenuManager;
//class ServerSideDecorationPaletteManager;
class WaylandIntegration : public QObject
{
    Q_OBJECT
public:
    explicit WaylandIntegration(HollywoodPlatformTheme *platformTheme);
    ~WaylandIntegration() override;

    void setAppMenu(QWindow *window, const QString &serviceName, const QString &objectPath);
    //void setPalette(QWindow *window, const QString &paletteName);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    static bool isRelevantTopLevel(QWindow *w);
    static struct wl_surface *surfaceFromWindow(QWindow *w);
    void shellSurfaceCreated(QWindow *w);
    void shellSurfaceDestroyed(QWindow *w);

    void installColorScheme(QWindow *w);
    QScopedPointer<AppMenuManager> m_appMenuManager;
    //QScopedPointer<ServerSideDecorationPaletteManager> m_paletteManager;
    HollywoodPlatformTheme *m_platformTheme;
};

class AppMenuManager : public QWaylandClientExtensionTemplate<AppMenuManager>,
        public QtWayland::org_kde_kwin_appmenu_manager
{
    Q_OBJECT
public:
    AppMenuManager()
        : QWaylandClientExtensionTemplate<AppMenuManager>(1)
    {
        qDebug() <<"appMenuManager";
        initialize();
/*#else
        // QWaylandClientExtensionTemplate invokes this with a QueuedConnection
        QMetaObject::invokeMethod(this, "addRegistryListener");
#endif*/
    }
};

#endif // WAYLANDINTEGRATION_H
