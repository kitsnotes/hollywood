/* Hollywood Operating System
 * System Settings - Date & Time Applet
 *
 * SPDX-FileCopyrightText: 2022 The Cat <cat@originull.org>
 * SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file was originally adopted from Calamares
 */

#ifndef TZIMAGE_H
#define TZIMAGE_H

#include <QImage>
#include <QList>

using TimeZoneImage = QImage;
class TimeZoneImageList : public QList< TimeZoneImage >
{
private:
    TimeZoneImageList();

public:
    static TimeZoneImageList fromQRC();
    static TimeZoneImageList fromDirectory( const QString& dirName );
    static QPoint getLocationPosition( double longitude, double latitude );
    int index( QPoint p ) const;
    int index( QPoint p, int& count ) const;
    QImage find( QPoint p ) const;
    static constexpr const int zoneCount = 37;
    static constexpr const QSize imageSize = QSize( 780, 340 );
};

#endif
