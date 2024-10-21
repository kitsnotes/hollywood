// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-only

#include "shortcuts.h"
#include <QSettings>

Q_LOGGING_CATEGORY(hwShortcut, "compositor.globalkey")

ShortcutManager::ShortcutManager(QObject *parent)
    : QObject(parent)
{
    // create keys for media keyboards
    m_mon_brightness_up_media.combo = QKeyCombination(Qt::Key_MonBrightnessUp);
    m_mon_brightness_up_media.signal = [this] { emit monitorBrightnessUp(); };

    m_mon_brightness_down_media.combo = QKeyCombination(Qt::Key_MonBrightnessDown);
    m_mon_brightness_down_media.signal = [this] { emit monitorBrightnessDown(); };

    m_kbd_brightness_up_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessUp);
    m_kbd_brightness_up_media.signal = [this] { emit keyboardBrightnessUp(); };

    m_kbd_brightness_down_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_kbd_brightness_down_media.signal = [this] { emit keyboardBrightnessDown(); };

    m_vol_up_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_vol_up_media.signal = [this] { emit volumeUpRequested(); };

    m_vol_down_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_vol_down_media.signal = [this] { emit volumeDownRequested(); };

    m_vol_mute_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_vol_mute_media.signal = [this] { emit volumeMuteRequested(); };

    m_launch_browser_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_launch_browser_media.signal = [this] { emit webBrowserRequested(); };

    m_start_search_media.combo = QKeyCombination(Qt::Key_KeyboardBrightnessDown);
    m_start_search_media.signal = [this] { emit searchRequested(); };

    reloadConfig();
}

bool ShortcutManager::checkAndHandleCombo(const QKeyCombination &combo)
{
    if(!m_tracked.contains(combo))
        return false;

    auto val = m_tracked.value(combo);
    qCDebug(hwShortcut, "global hotkey shortcut triggered: %i", combo.toCombined());

    val.signal();
    return true;
}

void ShortcutManager::reloadConfig()
{
    m_tracked.clear();
    m_tracked.insert(m_mon_brightness_up_media.combo,
                     m_mon_brightness_up_media);
    m_tracked.insert(m_mon_brightness_down_media.combo,
                     m_mon_brightness_down_media);
    m_tracked.insert(m_kbd_brightness_up_media.combo,
                     m_kbd_brightness_up_media);
    m_tracked.insert(m_kbd_brightness_down_media.combo,
                     m_kbd_brightness_down_media);
    m_tracked.insert(m_vol_up_media.combo,
                     m_vol_up_media);
    m_tracked.insert(m_vol_down_media.combo,
                     m_vol_down_media);
    m_tracked.insert(m_vol_mute_media.combo,
                     m_vol_mute_media);
    m_tracked.insert(m_launch_browser_media.combo,
                     m_launch_browser_media);
    m_tracked.insert(m_start_search_media.combo,
                     m_start_search_media);

    QSettings settings(QSettings::UserScope,
       QLatin1String("originull"), QLatin1String("hollywood"));

    settings.beginGroup(QLatin1String("GlobalShortcuts"));

    int scval = settings.value("TakeScreenshot", QKeyCombination(Qt::Key_Print).toCombined()).toInt();
    m_full_screenshot.combo = QKeyCombination::fromCombined(scval);
    m_full_screenshot.signal = [this] { emit printScreenRequested(); };
    m_tracked.insert(m_full_screenshot.combo,
                     m_full_screenshot);

    int sw = settings.value("SwitchWindow", QKeyCombination(Qt::AltModifier, Qt::Key_Tab).toCombined()).toInt();
    m_switch_windows.combo = QKeyCombination::fromCombined(sw);
    m_switch_windows.signal = [this] { emit windowSwitchRequested(); };
    m_tracked.insert(m_switch_windows.combo,
                     m_switch_windows);
}

bool KeyCombination::operator==(const KeyCombination &l) const
{
    if(l.combo == combo)
        return true;

    return false;
}
