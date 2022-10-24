#ifndef DECORATION_H
#define DECORATION_H

#include <QtGui/QCursor>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPalette>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainterPath>

#include <qpa/qwindowsysteminterface.h>

#include <QtWaylandClient/private/qwaylanddecorationplugin_p.h>
#include <QtWaylandClient/private/qwaylandabstractdecoration_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandabstractdecoration_p.h>
QT_BEGIN_NAMESPACE

namespace QtWaylandClient
{
#define BUTTON_SPACING 5
#define BUTTON_WIDTH 18
#define BUTTONS_RIGHT_MARGIN 8

enum Button
{
    None,
    Close,
    Maximize,
    Minimize
};

class Q_WAYLAND_CLIENT_EXPORT HollywoodDecoration : public QWaylandAbstractDecoration
{
public:
    HollywoodDecoration();
protected:
    QMargins margins(MarginsType marginsType = Full) const override;
    void paint(QPaintDevice *device) override;
    bool handleMouse(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global,Qt::MouseButtons b,Qt::KeyboardModifiers mods) override;
    bool handleTouch(QWaylandInputDevice *inputDevice, const QPointF &local, const QPointF &global, QEventPoint::State state, Qt::KeyboardModifiers mods) override;
private:
    void processMouseTop(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods);
    void processMouseBottom(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods);
    void processMouseLeft(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods);
    void processMouseRight(QWaylandInputDevice *inputDevice, const QPointF &local, Qt::MouseButtons b,Qt::KeyboardModifiers mods);
    bool clickButton(Qt::MouseButtons b, Button btn);

    QRectF closeButtonRect() const;
    QRectF maximizeButtonRect() const;
    QRectF minimizeButtonRect() const;

    QColor m_foregroundColor;
    QColor m_foregroundInactiveColor;
    QColor m_backgroundColor;
    QStaticText m_windowTitle;
    Button m_clicking = None;
};

class HollywoodDecorationPlugin : public QWaylandDecorationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandDecorationFactoryInterface_iid FILE "hollywood.json")
public:
    QWaylandAbstractDecoration *create(const QString&, const QStringList&) override;
};


}

#endif // DECORATION_H
