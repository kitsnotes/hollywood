/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
 * SPDX-FileCopyrightText: 2017-2019 Adriaan de Groot <groot@kde.org>
 * SPDX-FileCopyrightText: 2014-2015 Teo Mrnjavac <teo@kde.org>
 * SPDX-FileCopyrightText: 2007 Free Software Foundation, Inc.
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file was originally adopted from Calamares
 * Originally from the Manjaro Installation Framework
 * by Roland Singer <roland@manjaro.org>
 * Copyright (C) 2007 Free Software Foundation, Inc.
 */

#ifndef TZWIDGET_H
#define TZWIDGET_H

#include "tzimage.h"
#include "timezone.h"

#include <QFont>
#include <QWidget>

class TimeZoneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimeZoneWidget( const ZonesModel* zones, QWidget* parent = nullptr );

public Q_SLOTS:
    void setCurrentLocation( const TimeZoneData* location );

signals:
    void locationChanged( const TimeZoneData* location );

private:
    QFont font;
    QImage background, pin, currentZoneImage;
    TimeZoneImageList timeZoneImages;

    const ZonesModel* m_zonesData;
    const TimeZoneData* m_currentLocation = nullptr;  // Not owned by me

    void paintEvent( QPaintEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
};

#endif  // TZWIDGET_H
