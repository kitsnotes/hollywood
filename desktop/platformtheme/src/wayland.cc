/*  Originated from KDE
    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "wayland.h"

#include <QExposeEvent>
#include <QGuiApplication>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
#include <qtwaylandclientversion.h>
#include "qdbusmenubar_p.h"

static const QByteArray s_schemePropertyName = QByteArrayLiteral("KDE_COLOR_SCHEME_PATH");
static const QByteArray s_blurBehindPropertyName = QByteArrayLiteral("ENABLE_BLUR_BEHIND_HINT");


using AppMenu = QtWayland::org_kde_kwin_appmenu;
//using ServerSideDecorationPalette = QtWayland::org_kde_kwin_server_decoration_palette;

Q_DECLARE_METATYPE(AppMenu *);
//Q_DECLARE_METATYPE(ServerSideDecorationPalette *);

WaylandIntegration::WaylandIntegration(HollywoodPlatformTheme *platformTheme)
    : QObject()
    , m_appMenuManager(new AppMenuManager)
    //, m_paletteManager(new ServerSideDecorationPaletteManager)
    , m_platformTheme(platformTheme)
{
    qDebug() << "QPlatformTheme: Created WaylandIntegration";
    QCoreApplication::instance()->installEventFilter(this);
}

WaylandIntegration::~WaylandIntegration() = default;

bool WaylandIntegration::isRelevantTopLevel(QWindow *w)
{
    if (!w || w->parent()) {
        return false;
    }

    // ignore  windows that map to XdgPopup
    if (w->type() == Qt::ToolTip || w->type() == Qt::Popup) {
        return false;
    }
    return true;
}

bool WaylandIntegration::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        auto ee = static_cast<QPaintEvent *>(event);
        if (ee->region().isNull()) {
            return false;
        }
        QWindow *w = qobject_cast<QWindow *>(watched);
        if (!isRelevantTopLevel(w)) {
            return false;
        }
        if (!w->isVisible()) {
            return false;
        }

        if (w->property("org.kde.plasma.integration.shellSurfaceCreated").isNull()) {
            shellSurfaceCreated(w);
        }
    } else if (event->type() == QEvent::Hide) {
        QWindow *w = qobject_cast<QWindow *>(watched);
        if (!isRelevantTopLevel(w)) {
            return false;
        }
        shellSurfaceDestroyed(w);
    } else if (event->type() == QEvent::ApplicationPaletteChange) {
        const auto topLevelWindows = QGuiApplication::topLevelWindows();
        for (QWindow *w : topLevelWindows) {
            if (isRelevantTopLevel(w)) {
                installColorScheme(w);
            }
        }
    } else if (event->type() == QEvent::PlatformSurface) {
        if (QWindow *w = qobject_cast<QWindow *>(watched)) {
            QPlatformSurfaceEvent *pe = static_cast<QPlatformSurfaceEvent *>(event);
            if (!w->flags().testFlag(Qt::ForeignWindow)) {
                if (pe->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
                    m_platformTheme->windowCreated(w);
                }
            }
        }
    }

    return false;
}

void WaylandIntegration::shellSurfaceCreated(QWindow *w)
{
    qDebug() << "org.kde.plasma.integration.shellSurfaceCreated";
    // set colorscheme hint
    /* if (qApp->property(s_schemePropertyName.constData()).isValid())
        installColorScheme(w);

    const auto blurBehindProperty = w->property(s_blurBehindPropertyName.constData());
    if (blurBehindProperty.isValid())
        KWindowEffects::enableBlurBehind(w, blurBehindProperty.toBool());
    */
    // create deco
    wl_surface *s = surfaceFromWindow(w);
    if (!s) {
        return;
    }

    w->setProperty("org.kde.plasma.integration.shellSurfaceCreated", true);

    if (m_appMenuManager->isActive()) {
        auto menu = new AppMenu(m_appMenuManager->create(s));
        w->setProperty("org.kde.plasma.integration.appmenu", QVariant::fromValue(menu));
        auto menuBar = DBusMenuBar::menuBarForWindow(w);
        if (!menuBar) {
            menuBar = DBusMenuBar::globalMenuBar();
        }
        if (menuBar) {
            menu->set_address(QDBusConnection::sessionBus().baseService(), menuBar->objectPath());
        }
    }
}

void WaylandIntegration::shellSurfaceDestroyed(QWindow *w)
{
    w->setProperty("org.kde.plasma.integration.shellSurfaceCreated", QVariant());

    auto appMenu = w->property("org.kde.plasma.integration.appmenu").value<AppMenu *>();
    if (appMenu) {
        appMenu->release();
        delete appMenu;
    }
    w->setProperty("org.kde.plasma.integration.appmenu", QVariant());

    /* auto decoPallete = w->property("org.kde.plasma.integration.palette").value<ServerSideDecorationPalette *>();
    if (decoPallete) {
        decoPallete->release();
        delete decoPallete;
    }*/
    w->setProperty("org.kde.plasma.integration.palette", QVariant());
}

void WaylandIntegration::installColorScheme(QWindow *w)
{
    Q_UNUSED(w);
    /*if (!m_paletteManager->isActive()) {
        return;
    }
    auto palette = w->property("org.kde.plasma.integration.palette").value<ServerSideDecorationPalette *>();
    if (!palette) {
        auto s = surfaceFromWindow(w);
        if (!s) {
            return;
        }
        palette = new ServerSideDecorationPalette(m_paletteManager->create(s));
        w->setProperty("org.kde.plasma.integration.palette", QVariant::fromValue(palette));
    }
    if (palette) {
        palette->set_palette(qApp->property(s_schemePropertyName.constData()).toString());
    }*/
}

void WaylandIntegration::setAppMenu(QWindow *window, const QString &serviceName, const QString &objectPath)
{
    auto menu = window->property("org.kde.plasma.integration.appmenu").value<AppMenu *>();
    if (menu) {
        menu->set_address(serviceName, objectPath);
    }
}

wl_surface *WaylandIntegration::surfaceFromWindow(QWindow *window)
{
    QPlatformNativeInterface *nativeInterface = qGuiApp->platformNativeInterface();
    if (!nativeInterface) {
        return nullptr;
    }
    return static_cast<wl_surface *>(nativeInterface->nativeResourceForWindow("surface", window));
}
