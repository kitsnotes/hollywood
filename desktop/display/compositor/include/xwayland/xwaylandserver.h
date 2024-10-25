// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/QString>
#include <QtCore/QProcess>

#include "compositor.h"

struct wl_client;

class ServerProcess;

class XWaylandServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Compositor *compositor READ compositor CONSTANT)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
public:
    XWaylandServer(Compositor *compositor, QObject *parent = nullptr);
    ~XWaylandServer();

    inline int wmFd() const {
        return m_wmPairFd[0];
    }

    inline wl_client *client() const {
        return m_client;
    }

    Compositor *compositor() const;
    QString displayName() const;

    bool start();
    void shutdown();

Q_SIGNALS:
    void displayNameChanged();
    void started(const QString &displayName);
    void failedToStart();

private:
    Compositor *m_compositor;

    int m_display;
    QString m_displayName;

    ServerProcess *m_process;
    int m_serverPairFd[2];
    int m_wmPairFd[2];

    wl_client *m_client;

private Q_SLOTS:
    void handleServerStarted();
};

