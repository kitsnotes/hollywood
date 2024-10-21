// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QWaylandOutput>
#include <QWaylandOutputMode>
#include <QWaylandXdgOutputManagerV1>
#include <QScreen>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(hwOutput)

class Surface;
class OutputWindow;
class Output : public QObject
{
    Q_OBJECT
public:
    explicit Output(QScreen *s, bool defaultScreen = false);
    OutputWindow* window() { return m_window; }
    QWaylandOutput* wlOutput() { return m_wlOutput; }
    QScreen* screen() { return m_screen; }
    QSize size() const;
    bool primary() const { return m_primary; }
    void setPrimary(bool primary) { m_primary = primary; }
    bool reserveLayerShellRegion(Surface *surface);
    bool removeLayerShellReservation(Surface *surface);
    void modesetFromConfig();
signals:
    void reservedRegionsChanged();
    void availableGeometryChanged(const QRect &geometry);
private slots:
    void modeChanged();
private:
    QString persistentSettingName() const;
    void touchConfiguration();
    void reloadConfiguration();
    uint defaultScaleFactor();
    void updateUsableGeometry();
private:
    friend class CompositorApp;
    friend class OutputWindow;
    bool m_virtual;
    bool m_primary = false;
    OutputWindow *m_window;
    QScreen *m_screen;
    QWaylandOutput *m_wlOutput;
    QList<Surface*> m_reserved;

    QWaylandXdgOutputV1 *m_xdgOutput = nullptr;
};
