// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "output.h"
#include "outputwnd.h"
#include "application.h"
#include "compositor.h"

Output::Output(QObject *parent)
    : QObject(parent)
    , m_window(new OutputWindow(this))
{
    m_wlOutput = new QWaylandOutput(wcApp->waylandCompositor(), m_window);
    wcApp->compositor()->addOutput(this);
}

QSize Output::size() const
{
    return m_window->size();
}

void Output::configureForScreen(QScreen *s, bool defaultScreen)
{
    m_screen = s;
    QWaylandOutputMode mode(s->size(), s->refreshRate());
    qDebug() << "refresh rate:" << s->refreshRate();
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setManufacturer(s->manufacturer());
    m_wlOutput->setModel(s->model());
    if(defaultScreen)
        wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);

    if(defaultScreen)
        m_wlOutput->setPosition(QPoint(0,0));
    m_wlOutput->setSizeFollowsWindow(true);
    m_window->resize(s->size());
    m_window->show();
}

void Output::configureDebugWindow()
{
    QWaylandOutputMode mode(QSize(1024,768), 60000);
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setManufacturer(tr("Originull"));
    m_wlOutput->setModel(tr("Debug Wnd 1"));

    wcApp->waylandCompositor()->setDefaultOutput(m_wlOutput);
    m_wlOutput->setPosition(QPoint(0,0));
    m_wlOutput->setSizeFollowsWindow(true);
    m_window->resize(1024,768);
    m_window->show();
    if(m_primary)
        m_window->setTitle(tr("Primary Display"));

}

void Output::configureDebugWindow2()
{
    QWaylandOutputMode mode(QSize(1024,768), 60000);
    m_wlOutput->addMode(mode, true);
    m_wlOutput->setCurrentMode(mode);
    m_wlOutput->setPosition(QPoint(1025,0));
    m_wlOutput->setSizeFollowsWindow(true);
    m_wlOutput->setManufacturer(tr("Originull"));
    m_wlOutput->setModel(tr("Debug Wnd 2"));
    m_window->resize(1024,768);
    m_window->show();
    if(m_primary)
        m_window->setTitle(tr("Secondary Display"));

}
