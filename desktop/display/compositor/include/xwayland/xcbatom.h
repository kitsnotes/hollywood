// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/QByteArray>

#include "xwayland.h"

namespace Xcb {

class Atom
{
    Q_DISABLE_COPY(Atom)
public:
    explicit Atom(const QByteArray &name, bool onlyIfExists = false);
    Atom();
    ~Atom();

    inline const QByteArray &name() const {
        return m_name;
    }

    bool isValid();

    operator xcb_atom_t() const;

    static QString nameFromAtom(xcb_atom_t atom);

private:
    QByteArray m_name;
    bool m_retrieved;
    xcb_intern_atom_cookie_t m_cookie;
    xcb_atom_t m_atom;

    void getReply();
};

} // namespace Xcb

