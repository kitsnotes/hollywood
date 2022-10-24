#ifndef PRIVATEPROTOCOL_P_H
#define PRIVATEPROTOCOL_P_H

#include "privateprotocol.h"
#include "libcompositor_global.h"
#include <QGuiApplication>
#include <QtWaylandClient/QWaylandClientExtension>
#include <QtGui/QWindow>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qwindowsysteminterface.h>
#include <QtWaylandClient/private/qwayland-wayland.h>


class LIBCOMPOSITOR_EXPORT AIPrivateWaylandProtocolObject
        : public QObject

{
    Q_OBJECT
public:
    AIPrivateWaylandProtocolObject() = default;
private:

};
#endif // PRIVATEPROTOCOL_P_H
