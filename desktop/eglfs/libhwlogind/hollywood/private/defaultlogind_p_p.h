// Hollywood logind interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from libliri
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Hollywood API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "logind.h"

namespace Originull {

class DefaultLogind : public Logind
{
    Q_OBJECT
public:
    explicit DefaultLogind(QObject *parent = nullptr);
};

} // namespace Originull
