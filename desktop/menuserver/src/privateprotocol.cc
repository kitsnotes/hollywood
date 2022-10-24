#include "privateprotocol.h"


AIPrivateWaylandProtocol::AIPrivateWaylandProtocol()
    :QWaylandClientExtensionTemplate(1)
{
    //connect(this, &AIPrivateWaylandProtocol::activeChanged,
    //        this, &AIPrivateWaylandProtocol::handleExtensionActive);
}

static inline struct ::wl_surface *getWlSurface(QWindow *window)
{
    void *surf = QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window);
    return static_cast<struct ::wl_surface *>(surf);
}

void AIPrivateWaylandProtocol::setMenuServerForWindow(QWindow *window)
{
    if(!isActive())
        return;
    if (window->handle())
    {
        wl_surface *nativeSurface = getWlSurface(window);
        QtWayland::arion_private_protocol::provision_menu_server(nativeSurface);
    }
}

/* QWindow *AIPrivateWaylandProtocol::windowForSurface(struct ::wl_surface *surface)
{
    for (QWindow *w : qAsConst(m_windows)) {
        if (getWlSurface(w) == surface)
            return w;
    }
    return nullptr;
}

bool AIPrivateWaylandProtocol::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::PlatformSurface
            && static_cast<QPlatformSurfaceEvent*>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
        QWindow *window = qobject_cast<QWindow*>(object);
        Q_ASSERT(window);
        window->removeEventFilter(this);
        QtWayland::arion_private_protocol::register_surface(getWlSurface(window));
    }
    return false;
}

void AIPrivateWaylandProtocol::sendWindowRegistration(QWindow *window)
{
    if (window->handle())
        QtWayland::arion_private_protocol::register_surface(getWlSurface(window));
    else
        window->installEventFilter(this); // register when created
}

void AIPrivateWaylandProtocol::registerWindow(QWindow *window)
{
    m_windows << window;
    if (isActive())
        sendWindowRegistration(window);
} */
