// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>
#include <QObject>
#include <QWaylandCompositor>

#include <hollywood/eglfsfunctions.h>

#if defined(wcApp)
#undef wcApp
#endif
#define wcApp (CompositorApp::instance())

class Compositor;
class CompositorApp : public QGuiApplication
{
public:
    CompositorApp(bool is_sddm, int &argc, char **argv);
    static CompositorApp* instance() { return static_cast<CompositorApp*>(QGuiApplication::instance()); }
    static Compositor* compositorInstance() { return static_cast<CompositorApp*>(QGuiApplication::instance())->compositor(); }
    Compositor* compositor();
    QWaylandCompositor* waylandCompositor();

    void doInit();
private:
    void createForDisplays();
    void createDummyDebugWindow();
private:
    Compositor *m_compositor;
};

#endif // ACAPPLICATION_H
