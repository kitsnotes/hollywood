#ifndef ACPRIVATEPROTOCOL_H
#define ACPRIVATEPROTOCOL_H

#include <QGuiApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include <QtGui/QWindow>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qwindowsysteminterface.h>
#include <QtWaylandClient/private/qwayland-wayland.h>
#include "qwayland-arion-private-ui.h"

class QWaylandDisplay;
class QWaylandIntegration;

class AIPrivateWaylandProtocol : public QWaylandClientExtensionTemplate<AIPrivateWaylandProtocol>,
        public QtWayland::arion_private_protocol
{
    Q_OBJECT
public:
    AIPrivateWaylandProtocol();
    void registerWindow(QWindow *window);
public slots:
    void setMenuServerForWindow(QWindow *window);

signals:
    //void eventReceived(const QString &text, uint value);
private:
    /* QWindow* windowForSurface(struct ::wl_surface *surface);
    void sendWindowRegistration(QWindow* window);
    //bool eventFilter(QObject *object, QEvent *event) override;
private:
    QList<QWindow*> m_windows;
    bool m_setup = false; */
    //void provision_menu_server(struct ::wl_surface *surface) override;


};

#endif // ACPRIVATEPROTOCOL_H
