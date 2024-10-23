// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Originated from kwin_wayland_proxy
// SPDX-FileCopyrightText: 2020 <davidedmundson@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wlproxy.h"

HWLWrapper::HWLWrapper(QObject *parent)
    : QObject(parent)
    , m_HWLProcess(new QProcess(this))
    , m_watchdogInterval(std::chrono::microseconds(qEnvironmentVariableIntValue("WATCHDOG_USEC", &m_watchdogIntervalOk) / 2))
{
    m_socket = wl_socket_create();
    if (!m_socket) {
        qFatal("Could not create wayland socket");
    }

    /*if (qApp->arguments().contains(QLatin1String("--xwayland"))) {
        m_xwlSocket = std::make_unique<HWL::XwaylandSocket>(HWL::XwaylandSocket::OperationMode::TransferFdsOnExec);
        if (!m_xwlSocket->isValid()) {
            qCWarning(HWL_WRAPPER) << "Failed to create Xwayland connection sockets";
            m_xwlSocket.reset();
        }
        if (m_xwlSocket) {
            if (!qEnvironmentVariableIsSet("HWL_WAYLAND_NO_XAUTHORITY")) {
                if (!generateXauthorityFile(m_xwlSocket->display(), &m_xauthorityFile)) {
                    qCWarning(HWL_WRAPPER) << "Failed to create an Xauthority file";
                }
            }
        }
    }*/
}

HWLWrapper::~HWLWrapper()
{
    wl_socket_destroy(m_socket);
    terminate(30s);
}

void HWLWrapper::run()
{
    m_HWLProcess->setProgram("/usr/libexec/hollywood/compositor");
    qputenv("WAYLAND_SOCKET_FD", QString::number(wl_socket_get_fd(m_socket)).toUtf8());
    qputenv("WAYLAND_SOCKET_NAME", QString::fromUtf8(wl_socket_get_display_name(m_socket)).toUtf8());

    
    /*if (m_xwlSocket) {
        const auto xwaylandFileDescriptors = m_xwlSocket->fileDescriptors();
        for (const int &fileDescriptor : xwaylandFileDescriptors) {
            args << "--xwayland-fd" << QString::number(fileDescriptor);
        }
        args << "--xwayland-display" << m_xwlSocket->name();
        if (m_xauthorityFile.open()) {
            args << "--xwayland-xauthority" << m_xauthorityFile.fileName();
        }
    }*/

    m_HWLProcess->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_HWLProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0) {
            qApp->quit();
            return;
        } else if (exitCode == 133) {
            m_crashCount = 0;
        } else {
            m_crashCount++;
        }

        if (m_crashCount > 10) {
            qApp->quit();
            return;
        }
        qputenv("HWL_RESTART_COUNT", QByteArray::number(m_crashCount));
        // restart
        m_HWLProcess->start();
    });

    m_HWLProcess->start();

    QProcessEnvironment env;
    env.insert("WAYLAND_DISPLAY", QString::fromUtf8(wl_socket_get_display_name(m_socket)));
    /*if (m_xwlSocket) {
        env.insert("DISPLAY", m_xwlSocket->name());
        if (m_xauthorityFile.open()) {
            env.insert("XAUTHORITY", m_xauthorityFile.fileName());
        }
    }*/
}

void HWLWrapper::terminate(std::chrono::milliseconds timeout)
{
    if (m_HWLProcess) {
        disconnect(m_HWLProcess, nullptr, this, nullptr);
        m_HWLProcess->terminate();
        m_HWLProcess->waitForFinished(timeout.count() / 2);
        if (m_HWLProcess->state() != QProcess::NotRunning) {
            m_HWLProcess->kill();
            m_HWLProcess->waitForFinished(timeout.count() / 2);
        }
    }
}

void HWLWrapper::restart()
{
    terminate(m_watchdogIntervalOk ? std::chrono::duration_cast<std::chrono::milliseconds>(m_watchdogInterval) : 30000ms);
    m_HWLProcess->start();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    //app.setQuitLockEnabled(false); // don't exit when the first KJob finishes

    /*KSignalHandler::self()->watchSignal(SIGTERM);
    KSignalHandler::self()->watchSignal(SIGHUP);

    HWLWrapper wrapper(&app);
    wrapper.run();

    QObject::connect(KSignalHandler::self(), &KSignalHandler::signalReceived, &app, [&app, &wrapper](int signal) {
        if (signal == SIGTERM) {
            app.quit();
        } else if (signal == SIGHUP) { // The systemd service will issue SIGHUP when it's locked up so that we can restarted
            wrapper.restart();
        }
    });*/
    HWLWrapper wrapper(&app);
    wrapper.run();
    return app.exec();
}

