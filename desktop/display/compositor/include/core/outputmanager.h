// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#pragma once

#include <QObject>
#include <QScreen>
#include <QLoggingCategory>
#include "outputmanagement.h"

Q_DECLARE_LOGGING_CATEGORY(hwSeatManager)

class Compositor;
class Output;
class WlrOutputManagerV1;
class OutputManager : public QObject
{
    Q_OBJECT
public:
    explicit OutputManager(Compositor *parent, bool console = true);
    void present();
    void triggerRender();
    Output* primaryOutput();
    Output* outputAtPosition(const QPoint &pos);
    QList<Output*> outputs();
    WlrOutputManagerV1* wlrOutputManager();
protected:
    friend class Compositor;
    void startRender();
    void endRender();
private slots:
    void screenAdded(QScreen *screen);
    void screenRemoved(QScreen *screen);
private:
    void createForConsole();
    void recalculatePositioning();
    Output* outputForScreen(QScreen *screen);
private:
    bool m_console = true;
    bool m_created = false;
    bool m_use_global_config = true;
    QList<Output*> m_outputs;

    WlrOutputManagerV1 *m_wlr_output = nullptr;
};
