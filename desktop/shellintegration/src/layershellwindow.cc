/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "layershellwindow.h"

#include <QPointer>

using namespace LayerShellQt;

class LayerShellQt::WindowPrivate
{
public:
    WindowPrivate(QWindow *window)
        : parentWindow(window)
    {
    }

    QWindow *parentWindow;
    QString scope = QStringLiteral("window");
    Window::Anchors anchors = {Window::AnchorTop | Window::AnchorBottom | Window::AnchorLeft | Window::AnchorRight};
    int32_t exclusionZone = 0;
    Window::KeyboardInteractivity keyboardInteractivity = Window::KeyboardInteractivityExclusive;
    Window::Layer layer = Window::LayerTop;
    QMargins margins;
    QPointer<QScreen> desiredOutput;
    QSize size;
};

static QHash<QWindow *, Window *> s_map;

Window::~Window()
{
    s_map.remove(d->parentWindow);
}

void Window::setAnchors(Anchors anchors)
{
    d->anchors = anchors;
    emit anchorsChanged();
}

Window::Anchors Window::anchors() const
{
    return d->anchors;
}

void Window::setExclusiveZone(int32_t zone)
{
    d->exclusionZone = zone;
    Q_EMIT exclusionZoneChanged();
}

int32_t Window::exclusionZone() const
{
    return d->exclusionZone;
}

void Window::setMargins(const QMargins &margins)
{
    d->margins = margins;
    emit marginsChanged();
}

QMargins Window::margins() const
{
    return d->margins;
}

void Window::setKeyboardInteractivity(KeyboardInteractivity interactivity)
{
    d->keyboardInteractivity = interactivity;
    emit keyboardInteractivityChanged();
}

Window::KeyboardInteractivity Window::keyboardInteractivity() const
{
    return d->keyboardInteractivity;
}

void Window::setLayer(Layer layer)
{
    d->layer = layer;
    emit layerChanged();
}

void Window::setScope(const QString &scope)
{
    d->scope = scope;
    // this is static and must be set before the platform window is created
}

QString Window::scope() const
{
    return d->scope;
}

Window::Layer Window::layer() const
{
    return d->layer;
}

void Window::setSize(QSize size)
{
    d->size = size;
    Q_EMIT sizeChanged();
}

QSize Window::size() const
{
    return d->size;
}

QScreen *Window::desiredOutput() const
{
    return d->desiredOutput;
}

void Window::setDesiredOutput(QScreen *output)
{
    d->desiredOutput = output;
}

Window::Window(QWindow *window)
    : QObject(window)
    , d(new WindowPrivate(window))
{
    s_map.insert(d->parentWindow, this);
}

Window *Window::get(QWindow *window)
{
    auto layerShellWindow = s_map.value(window);
    if (layerShellWindow) {
        return layerShellWindow;
    }
    return new Window(window);
}
