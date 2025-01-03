// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "outputmanager.h"
#include "compositor.h"
#include "output.h"
#include "outputwnd.h"

#include <QSettings>

Q_LOGGING_CATEGORY(hwSeatManager, "compositor.seatmanager")


OutputManager::OutputManager(Compositor *parent, bool console)
    : QObject(parent)
{
    m_use_global_config = true;
    if(console)
    {
        qCInfo(hwOutput, "Setting up console");
        createForConsole();
        m_wlr_output = new WlrOutputManagerV1(hwComp);
        connect(qApp, &QGuiApplication::screenAdded, this, &OutputManager::screenAdded);
        connect(qApp, &QGuiApplication::screenRemoved, this, &OutputManager::screenRemoved);
    }
}

void OutputManager::present()
{
    // show all output console windows
    for(auto output : m_outputs)
        output->window()->show();
}

void OutputManager::createForConsole()
{
    if(m_created)
        return;

    QSettings settings("/etc/hollywood/compositor.conf", QSettings::IniFormat);
    // query all screens that we have currently
    for(auto screen : qApp->screens())
    {
        auto def = (screen == qApp->primaryScreen());
        auto window = new OutputWindow();
        auto output = new Output(screen, window, def);
        window->setOutput(output);
        m_outputs.append(output);
        window->show();
        auto settingsGroup = QString("Display-%1").arg(screen->name());

        settings.beginGroup(settingsGroup);
        auto scale = settings.value("ScaleFactor",
                                    output->defaultScaleFactor()).toInt();
        if(output->scaleFactor() != scale)
            output->setScaleFactor(scale);

        auto rotation = settings.value("Rotation",0).toUInt();
        if(rotation != 0 && rotation != 90
                && rotation != 180 && rotation != 270)
            rotation = 0;

        auto width = settings.value("Width",0).toUInt();
        auto height = settings.value("Height",0).toUInt();
        auto refresh = settings.value("Refresh", 60).toUInt();
        if(height != 0 && width != 0)
            output->modeset(QSize(width, height), refresh);

        auto posx = settings.value("PositionX",0).toUInt();
        auto posy = settings.value("PositionY",0).toUInt();
        output->setConfigPosition(QPoint(posx,posy));
    }
    recalculatePositioning();
    m_created = true;
}

void OutputManager::recalculatePositioning()
{
    // if we only have one input lets make this easy
    if(m_outputs.count() == 1)
    {
        m_outputs.first()->setPosition(QPoint(0,0));
        return;
    }

    // if we have negative offsets, let's fix that
    // to move us into 0,0 positive space
    int minX = 0;
    int minY = 0;

    for(auto output : m_outputs)
    {
        if(output->configPosition().x() < minX)
            minX = output->configPosition().x();
        if(output->configPosition().y() < minY)
            minY = output->configPosition().y();
    }

    // we don't care if the only things are +0
    if(minX > 0)
        minX = 0;
    if(minY > 0)
        minY = 0;

    QMap<Output*,QPoint> positions;

    if(minX < 0 || minY < 0)
    {
        for(auto output : m_outputs)
        {
            // TODO: flip this for rotation
            auto pos = output->configPosition();
            pos.setX(pos.x()-minX);
            pos.setY(pos.y()-minY);
            positions.insert(output, pos);
        }
    }

    // items are now offset into 0,0
    // TODO: clean up any "dead space" (from bad config)?
    for(auto output : positions.keys())
    {
        auto pos = positions.value(output);
        output->setPosition(pos);
    }
}

void OutputManager::triggerRender()
{
    if(m_outputs.count() == 0)
        return;

    for(auto out : m_outputs)
        out->window()->requestUpdate();
}

Output *OutputManager::primaryOutput()
{
    for(auto out : m_outputs)
    {
        if(out->isPrimary())
            return out;
    }

    // fallback
    return m_outputs.first();
}

Output *OutputManager::outputAtPosition(const QPoint &pos)
{
    for(auto output : m_outputs)
    {
        if(output->geometry().contains(pos))
            return output;
    }
    return nullptr;
}

QList<Output *> OutputManager::outputs()
{
    return m_outputs;
}

WlrOutputManagerV1 *OutputManager::wlrOutputManager() { return m_wlr_output; }

void OutputManager::startRender()
{
    if(m_outputs.count() == 0)
        return;
    for(auto out : m_outputs)
    {
        if(out)
            out->frameStarted();
    }
}

void OutputManager::endRender()
{
    if(m_outputs.count() == 0)
        return;

    for(auto out : m_outputs)
    {
        if(out)
            out->sendFrameCallbacks();
    }
}

void OutputManager::screenAdded(QScreen *screen)
{
    auto output = outputForScreen(screen);
    Q_UNUSED(output);
    // check to see if output is higher on our primary list
    recalculatePositioning();
}

void OutputManager::screenRemoved(QScreen *screen)
{
    auto output = outputForScreen(screen);
    Q_UNUSED(output);
    // check to see if output is primary and elect a new one if so

    // move surfaces on this display to primary display

    recalculatePositioning();
}

Output *OutputManager::outputForScreen(QScreen *screen)
{
    for(auto output : m_outputs)
    {
        if(output->screen() == screen)
            return output;
    }

    return nullptr;
}
