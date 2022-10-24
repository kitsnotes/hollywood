#include "client/privateprotocol.h"
#include "client/privateprotocol_p.h"

AIPrivateWaylandProtocol::AIPrivateWaylandProtocol()
    :QWaylandClientExtensionTemplate<AIPrivateWaylandProtocol>(1),
     d_ptr(new AIPrivateWaylandProtocolObject) {}

AIPrivateWaylandProtocol::~AIPrivateWaylandProtocol()
{
    delete d_ptr;
}

void AIPrivateWaylandProtocol::registerWindow(QWindow *window)
{
    Q_UNUSED(window);
    return;
}

void AIPrivateWaylandProtocol::init(wl_registry *registry, int id, int version)
{
    d_ptr->init(registry, id, version);
}

const struct wl_interface *AIPrivateWaylandProtocol::interface()
{
    return AIPrivateWaylandProtocolObject::interface();
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
        d_ptr->provision_menu_server(nativeSurface);
    }
}

void AIPrivateWaylandProtocolObject::arion_private_protocol_appmenu_top_level_window_changed(struct ::wl_surface *surface, const QString &service_name, const QString &object_path)
{
    Q_UNUSED(surface);
    qDebug() << "arion_private_protocol_appmenu_top_level_window_changed" << service_name << object_path;
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

static AIPrivateWaylandProtocol *g_private = new AIPrivateWaylandProtocol();
