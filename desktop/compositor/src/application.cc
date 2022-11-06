// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "application.h"
#include "outputwnd.h"
#include "compositor.h"
#include "output.h"

#include <hollywood/hollywood.h>

#include <QMessageBox>

void handleShutDownSignal(int /* signalId */)
{
    return;
}

CompositorApp::CompositorApp(int &argc, char **argv)
    :QGuiApplication(argc, argv),
     m_compositor(new Compositor())
{
    setApplicationName("Hollywood Compositor");
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    Originull::Platform::EglFSFunctions::setCursorTheme(QLatin1String("neutral"), 32);

    m_compositor->create();
}

Compositor* CompositorApp::compositor()
{
    return m_compositor;
}

QWaylandCompositor* CompositorApp::waylandCompositor()
{
     return static_cast<QWaylandCompositor*>(m_compositor);
}
void CompositorApp::doInit()
{
    if(platformName() == QLatin1String("hollywood") ||
       platformName() == QLatin1String("wayland"))
    {
        /* right now we don't support nested wayland so error out */
        QMessageBox::critical(0, tr("Unsupported Compositor Action"),
                              tr("Currently Hollywood does not support using a nested compositor.  Now Exiting."));
        exit(0);
    }
    if(platformName() == QLatin1String("eglfs") ||
       platformName() == QLatin1String("hollywood-eglfs"))
        createForDisplays();
    else
        createDummyDebugWindow();
}

void CompositorApp::createForDisplays()
{
    auto ps = primaryScreen();
    auto output = new Output();
    output->configureForScreen(ps);
    output->window()->show();
}

void CompositorApp::createDummyDebugWindow()
{
    Output *output = new Output();
    output->setPrimary(true);
    output->configureDebugWindow();
    output->window()->show();

    return;

    Output *output2 = new Output();
    output2->configureDebugWindow2();
    output2->window()->show();
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handleShutDownSignal;
    sigaction(SIGINT, &sa, NULL);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    CompositorApp app(argc, argv);
    app.doInit();

    return app.exec();
}
