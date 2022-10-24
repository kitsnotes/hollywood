/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>

class ASAnalogClock : public QWidget
{
    Q_OBJECT
public:
    explicit ASAnalogClock(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ANALOGCLOCK_H
