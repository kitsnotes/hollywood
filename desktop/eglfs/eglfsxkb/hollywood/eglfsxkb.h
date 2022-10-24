/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL$
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1 or later as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPLv21 included in the packaging of
 * this file.  Please review the following information to ensure the
 * GNU Lesser General Public License version 2.1 requirements will be
 * met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 2.0 or later as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 included in the
 * packaging of this file.  Please review the following information to ensure
 * the GNU General Public License version 2.0 requirements will be
 * met: http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_EGLFSXKB_H
#define LIRI_EGLFSXKB_H

#include <Qt>

#include <xkbcommon/xkbcommon.h>

namespace Originull {

namespace Platform {

class EglFSXkb
{
public:
    static int keysymToQtKey(xkb_keysym_t keysym);
    static int keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers, const QString &text);
    static Qt::KeyboardModifiers modifiers(xkb_state *state);
};

} // namespace Platform

} // namespace Liri

#endif // LIRI_EGLFSXKB_H
