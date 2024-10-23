// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Originated from kwin_wayland_proxy
// SPDX-FileCopyrightText: 2020 <davidedmundson@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QtEnvironmentVariables>

#include <signal.h>

#include "wl-socket.h"

/*#include "xauthority.h"
#include "xwaylandsocket.h"*/

using namespace std::chrono_literals;

class HWLWrapper : public QObject
{
    Q_OBJECT
public:
    HWLWrapper(QObject *parent);
    ~HWLWrapper();

    void run();
    void restart();
    void terminate(std::chrono::milliseconds timeout);

private:
    wl_socket *m_socket;

    int m_crashCount = 0;
    QProcess *m_HWLProcess = nullptr;

    const std::chrono::microseconds m_watchdogInterval;
    bool m_watchdogIntervalOk;

    /*std::unique_ptr<HWL::XwaylandSocket> m_xwlSocket;
    QTemporaryFile m_xauthorityFile;*/
};
