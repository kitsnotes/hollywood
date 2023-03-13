// Hollywood Wayland Compositor
// (C) 2023 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include <QObject>
#include <QShortcut>
#include <QKeySequence>

class ShortcutManager : public QObject
{
    Q_OBJECT
public:
    explicit ShortcutManager(QObject *parent = nullptr);
    bool checkAndHandleCombo(const QKeyCombination &combo);
signals:

private slots:
    void brightnessUp();
    void brightnessDown();
private:
    QKeyCombination m_brightness_up;
    QKeyCombination m_brightness_down;

    QList<QKeyCombination> m_tracked;
};

#endif // SHORTCUTMANAGER_H
