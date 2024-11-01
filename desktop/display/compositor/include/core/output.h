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
#include <qpa/qplatformscreen.h>

Q_DECLARE_LOGGING_CATEGORY(hwOutput)

class Surface;
class OutputWindow;
class Output : public QWaylandOutput
{
    Q_OBJECT
public:
    explicit Output(QScreen *s, QWindow *output, bool defaultScreen = false);
    ~Output();
    OutputWindow* hwWindow();
    //QWaylandOutput* wlOutput() { return m_wlOutput; }
    QScreen* screen() { return m_screen; }
    QSize size() const;
    bool isPrimary() const { return m_primary; }
    void setPrimary(bool primary) { m_primary = primary; }
    bool reserveLayerShellRegion(Surface *surface);
    bool removeLayerShellReservation(Surface *surface);
    void modesetFromConfig();
    void sleepDisplay();
    void wakeDisplay();
    void setConfigPosition(const QPoint &point);
    QPoint configPosition() const;
    void modeset(const QSize &size, const unsigned int refresh);
    uint defaultScaleFactor();
    QList<QPlatformScreen::Mode> availableModes();
signals:
    void reservedRegionsChanged();
    void availableGeometryChanged(const QRect &geometry);
private slots:
    void modeChanged();
private:
    QString persistentSettingName() const;
    void updateUsableGeometry();
private:
    friend class CompositorApp;
    friend class OutputWindow;
    bool m_virtual;
    bool m_primary = false;
    QScreen *m_screen = nullptr;
    QList<Surface*> m_reserved;
    QWaylandXdgOutputV1 *m_xdg_output = nullptr;
    QPoint m_config_position;
    QList<QPlatformScreen::Mode> m_available_modes;
};
