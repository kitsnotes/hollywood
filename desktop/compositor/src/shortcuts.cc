// Hollywood Wayland Compositor
// (C) 2023 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#include "shortcuts.h"
#include <QDebug>

ShortcutManager::ShortcutManager(QObject *parent)
    : QObject(parent)
    , m_brightness_up(QKeyCombination(Qt::Key_Print))
{
    m_tracked.append(m_brightness_up);
}

bool ShortcutManager::checkAndHandleCombo(const QKeyCombination &combo)
{
    qDebug() << "checkAndHandleCombo" << combo;
    if(!m_tracked.contains(combo))
        return false;

    if(combo == m_brightness_up)
    {
        brightnessUp();
        return true;
    }

    return false;
}

void ShortcutManager::brightnessUp()
{
    qDebug() << "brightnessUp toggled";
}

void ShortcutManager::brightnessDown()
{

}
