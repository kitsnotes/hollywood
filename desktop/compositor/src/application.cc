// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "application.h"
#include "outputwnd.h"
#include "compositor.h"
#include "output.h"
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <hollywood/hollywood.h>
#include <stdlib.h>

#include <QMessageBox>

void handleShutDownSignal(int /* signalId */)
{
    return;
}

CompositorApp::CompositorApp(bool use_sddm, int &argc, char **argv)
    :QGuiApplication(argc, argv),
     m_compositor(new Compositor(use_sddm))
{
    setApplicationName("Hollywood Compositor");
    setApplicationVersion(HOLLYWOOD_OS_VERSION);
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    Originull::Platform::EglFSFunctions::setCursorTheme(QLatin1String("neutral"), 32);
    Originull::Platform::ScreenChange chg;
    chg.screen = QGuiApplication::primaryScreen();
    chg.position = QPoint(0,0);
    chg.enabled = true;
    chg.refreshRate = 60;
    chg.scale = 1.0;
    chg.resolution = QSize(1680,1050);
    QVector<Originull::Platform::ScreenChange> changes;
    changes << chg;
    Originull::Platform::EglFSFunctions::applyScreenChanges(changes);

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
    const char *name = "sddm";
    struct passwd *p;
    bool is_sddm = false;
    p = getpwnam(name);
    if(p != NULL)
    {
        if(getuid() == p->pw_uid)
            is_sddm = true;
    }

    if(!is_sddm)
    {
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = handleShutDownSignal;
        sigaction(SIGINT, &sa, NULL);
    }
    else
    {
        QString xdg = QString("/run/user/%1").arg(getuid());
        qputenv("XDG_RUNTIME_DIR", xdg.toUtf8().data());
        qputenv("QT_QPA_PLATFORM", "hollywood-eglfs");
    }

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    CompositorApp app(is_sddm, argc, argv);
    app.doInit();

    return app.exec();
}
