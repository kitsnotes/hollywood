// Hollywood Wayland Compositor
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef OUTPUT_H
#define OUTPUT_H

#include <QObject>
#include <QWaylandOutput>
#include <QWaylandOutputMode>
#include <QScreen>

class OutputWindow;
class Output : public QObject
{
    Q_OBJECT
public:
    explicit Output(QObject *parent = nullptr);
    OutputWindow* window() { return m_window; }
    QWaylandOutput* wlOutput() { return m_wlOutput; }
    QScreen* screen() { return m_screen; }
    QSize size() const;
    bool primary() const { return m_primary; }
    void setPrimary(bool primary) { m_primary = primary; };
signals:
private:
    void configureForScreen(QScreen *s, bool defaultScreen = false);
    void configureDebugWindow();
    void configureDebugWindow2();
private:
    friend class CompositorApp;
    friend class OutputWindow;
    bool m_virtual;
    bool m_primary = false;
    QScreen *m_screen;
    OutputWindow *m_window;
    QWaylandOutput *m_wlOutput;
};

#endif // OUTPUT_H
