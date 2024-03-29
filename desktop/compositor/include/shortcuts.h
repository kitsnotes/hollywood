// Hollywood Wayland Compositor
// (C) 2023 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QShortcut>
#include <QKeySequence>
#include <QHash>
#include <utility>

class KeyCombination {
public:
    QKeyCombination combo;
    std::function<void()> signal = nullptr;
    bool operator==(const KeyCombination &l) const;
};

class ShortcutManager : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutManager(QObject *parent = nullptr);
    bool checkAndHandleCombo(const QKeyCombination &combo);


signals:
    void monitorBrightnessUp();
    void monitorBrightnessDown();
    void keyboardBrightnessUp();
    void keyboardBrightnessDown();
    void printScreenRequested();
    void windowSwitchRequested();
    void volumeDownRequested();
    void volumeUpRequested();
    void volumeMuteRequested();
    void webBrowserRequested();
    void searchRequested();
public slots:
    void reloadConfig();
private:
    // Customizable Keys
    KeyCombination m_switch_windows;
    KeyCombination m_full_screenshot;

    KeyCombination m_mon_brightness_up;
    KeyCombination m_mon_brightness_down;
    KeyCombination m_kbd_brightness_up;
    KeyCombination m_kbd_brightness_down;

    // Multimedia Keyboard extras (always bound)
    KeyCombination m_mon_brightness_up_media;
    KeyCombination m_mon_brightness_down_media;
    KeyCombination m_kbd_brightness_up_media;
    KeyCombination m_kbd_brightness_down_media;

    KeyCombination m_vol_up_media;
    KeyCombination m_vol_down_media;
    KeyCombination m_vol_mute_media;

    KeyCombination m_launch_browser_media;
    KeyCombination m_start_search_media;


    // master list of global tracked combos
    QHash<QKeyCombination, KeyCombination> m_tracked;
};

#endif // SHORTCUTMANAGER_H
