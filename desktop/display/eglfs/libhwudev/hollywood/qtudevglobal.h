// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTUDEVGLOBAL_H
#define QTUDEVGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIRI_BUILD_QTUDEV_LIB)
#  define QTUDEV_EXPORT Q_DECL_EXPORT
#else
#  define QTUDEV_EXPORT Q_DECL_IMPORT
#endif
#define QTUDEV_NO_EXPORT Q_DECL_HIDDEN

#endif // QTUDEVGLOBAL_H
