// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-FileCopyrightText: 2012-2013 Martin Gräßlin
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QtCore/QPoint>
#include <QtCore/QVector>

#include "xcbwindow.h"

#include <stdint.h>

namespace Xcb {

typedef xcb_window_t WindowId;

// helper functions
static inline void moveResizeWindow(WindowId window, const QRect &geometry)
{
    const uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    const uint32_t values[] = {
        static_cast<uint32_t>(geometry.x()),
        static_cast<uint32_t>(geometry.y()),
        static_cast<uint32_t>(geometry.width()),
        static_cast<uint32_t>(geometry.height())
    };
    xcb_configure_window(connection(), window, mask, values);
}

static inline void moveWindow(xcb_window_t window, const QPoint &pos)
{
    Q_UNUSED(window)
    Q_UNUSED(pos)
    //moveWindow(window, pos.x(), pos.y());
}

static inline void moveWindow(xcb_window_t window, uint32_t x, uint32_t y)
{
    const uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
    const uint32_t values[] = { x, y };
    xcb_configure_window(connection(), window, mask, values);
}

static inline void lowerWindow(xcb_window_t window)
{
    const uint32_t values[] = { XCB_STACK_MODE_BELOW };
    xcb_configure_window(connection(), window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

static inline WindowId createInputWindow(const QRect &geometry, uint32_t mask, const uint32_t *values)
{
    WindowId window = xcb_generate_id(connection());
    xcb_create_window(connection(), 0, window, rootWindow(),
                      geometry.x(), geometry.y(), geometry.width(), geometry.height(),
                      0, XCB_WINDOW_CLASS_INPUT_ONLY,
                      XCB_COPY_FROM_PARENT, mask, values);
    return window;
}

static inline void restackWindows(const QVector<xcb_window_t> &windows)
{
    if (windows.count() < 2) {
        // only one window, nothing to do
        return;
    }
    for (int i=1; i<windows.count(); ++i) {
        const uint16_t mask = XCB_CONFIG_WINDOW_SIBLING | XCB_CONFIG_WINDOW_STACK_MODE;
        const uint32_t stackingValues[] = {
            windows.at(i-1),
            XCB_STACK_MODE_BELOW
        };
        xcb_configure_window(connection(), windows.at(i), mask, stackingValues);
    }
}

static inline void restackWindowsWithRaise(const QVector<xcb_window_t> &windows)
{
    if (windows.isEmpty())
        return;
    const uint32_t values[] = { XCB_STACK_MODE_ABOVE };
    xcb_configure_window(connection(), windows.first(), XCB_CONFIG_WINDOW_STACK_MODE, values);
    restackWindows(windows);
}

Window::Window(xcb_window_t window, bool destroy)
    : m_window(window)
    , m_destroy(destroy)
{
}

Window::Window(const QRect &geometry, uint32_t mask,
               const uint32_t *values, xcb_window_t parent)
    : m_window(doCreate(geometry, XCB_COPY_FROM_PARENT, mask, values, parent))
    , m_destroy(true)
{
}

Window::Window(const QRect &geometry, uint16_t windowClass,
               uint32_t mask, const uint32_t *values, xcb_window_t parent)
    : m_window(doCreate(geometry, windowClass, mask, values, parent))
    , m_destroy(true)
{
}

Window::~Window()
{
    destroy();
}

void Window::destroy()
{
    if (!isValid() || !m_destroy)
        return;
    xcb_destroy_window(connection(), m_window);
    m_window = XCB_WINDOW_NONE;
}

bool Window::isValid() const
{
    return m_window != XCB_WINDOW_NONE;
}

Window::operator xcb_window_t() const
{
    return m_window;
}

void Window::create(const QRect &geometry, uint16_t windowClass, uint32_t mask, const uint32_t *values, xcb_window_t parent)
{
    destroy();
    m_window = doCreate(geometry, windowClass, mask, values, parent);
}

void Window::create(const QRect &geometry, uint32_t mask, const uint32_t *values, xcb_window_t parent)
{
    create(geometry, XCB_COPY_FROM_PARENT, mask, values, parent);
}

xcb_window_t Window::doCreate(const QRect &geometry, uint16_t windowClass, uint32_t mask, const uint32_t *values, xcb_window_t parent)
{
    xcb_window_t w = xcb_generate_id(connection());
    xcb_create_window(connection(), XCB_COPY_FROM_PARENT, w, parent,
                      geometry.x(), geometry.y(), geometry.width(), geometry.height(),
                      0, windowClass, XCB_COPY_FROM_PARENT, mask, values);
    return w;
}

void Window::reset(xcb_window_t window, bool shouldDestroy)
{
    destroy();
    m_window = window;
    m_destroy = shouldDestroy;
}

void Window::setGeometry(const QRect &geometry)
{
    setGeometry(geometry.x(), geometry.y(), geometry.width(), geometry.height());
}

void Window::setGeometry(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!isValid())
        return;
    const uint16_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    const uint32_t values[] = { x, y, width, height };
    xcb_configure_window(connection(), m_window, mask, values);
}

void Window::move(const QPoint &pos)
{
    move(pos.x(), pos.y());
}

void Window::move(uint32_t x, uint32_t y)
{
    if (!isValid())
        return;
    moveWindow(m_window, x, y);
}

void Window::resize(const QSize &size)
{
    resize(size.width(), size.height());
}

void Window::resize(uint32_t width, uint32_t height)
{
    if (!isValid())
        return;
    const uint16_t mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    const uint32_t values[] = { width, height };
    xcb_configure_window(connection(), m_window, mask, values);
}

void Window::raise()
{
    const uint32_t values[] = { XCB_STACK_MODE_ABOVE };
    xcb_configure_window(connection(), m_window, XCB_CONFIG_WINDOW_STACK_MODE, values);
}

void Window::lower()
{
    lowerWindow(m_window);
}

void Window::map()
{
    if (!isValid())
        return;
    xcb_map_window(connection(), m_window);
}

void Window::unmap()
{
    if (!isValid())
        return;
    xcb_unmap_window(connection(), m_window);
}

void Window::reparent(xcb_window_t parent, int x, int y)
{
    if (!isValid())
        return;
    xcb_reparent_window(connection(), m_window, parent, x, y);
}

void Window::changeProperty(xcb_atom_t property, xcb_atom_t type, uint8_t format, uint32_t lenght, const void *data, uint8_t mode)
{
    if (!isValid())
        return;
    xcb_change_property(connection(), mode, m_window, property, type, format, lenght, data);
}

void Window::deleteProperty(xcb_atom_t property)
{
    if (!isValid())
        return;
    xcb_delete_property(connection(), m_window, property);
}

void Window::setBorderWidth(uint32_t width)
{
    if (!isValid())
        return;
    xcb_configure_window(connection(), m_window, XCB_CONFIG_WINDOW_BORDER_WIDTH, &width);
}

void Window::grabButton(uint8_t pointerMode, uint8_t keyboardmode, uint16_t modifiers,
                        uint8_t button, uint16_t eventMask, xcb_window_t confineTo,
                        xcb_cursor_t cursor, bool ownerEvents)
{
    if (!isValid())
        return;
    xcb_grab_button(connection(), ownerEvents, m_window, eventMask,
                    pointerMode, keyboardmode, confineTo, cursor, button, modifiers);
}

void Window::ungrabButton(uint16_t modifiers, uint8_t button)
{
    if (!isValid())
        return;
    xcb_ungrab_button(connection(), button, m_window, modifiers);
}

void Window::clear()
{
    if (!isValid())
        return;
    xcb_clear_area(connection(), false, m_window, 0, 0, 0, 0);
}

void Window::setBackgroundPixmap(xcb_pixmap_t pixmap)
{
    if (!isValid())
        return;
    const uint32_t values[] = {pixmap};
    xcb_change_window_attributes(connection(), m_window, XCB_CW_BACK_PIXMAP, values);
}

void Window::defineCursor(xcb_cursor_t cursor)
{
    //Xcb::defineCursor(m_window, cursor);
}

void Window::focus(uint8_t revertTo, xcb_timestamp_t time)
{
    //setInputFocus(m_window, revertTo, time);
}

void Window::selectInput(uint32_t events)
{
    //Xcb::selectInput(m_window, events);
}

void Window::kill()
{
    xcb_kill_client(connection(), m_window);
}

xcb_window_t Window::window() const
{
    return m_window;
}

} // namespace Xcb

