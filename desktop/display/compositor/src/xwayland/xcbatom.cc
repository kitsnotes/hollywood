// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "xcbatom.h"
#include "xcbwrapper.h"

namespace Xcb {

Atom::Atom(const QByteArray &name, bool onlyIfExists)
    : m_name(name)
    , m_retrieved(false)
    , m_cookie(xcb_intern_atom(connection(), onlyIfExists, name.length(), name.constData()))
    , m_atom(XCB_ATOM_NONE)
{
}

Atom::Atom()
    : m_retrieved(false)
    , m_atom(XCB_ATOM_NONE)
{
}

Atom::~Atom()
{
    if (!m_retrieved && m_cookie.sequence)
        xcb_discard_reply(connection(), m_cookie.sequence);
}

bool Atom::isValid()
{
    getReply();
    return m_atom != XCB_ATOM_NONE;
}

Atom::operator xcb_atom_t() const
{
    const_cast<Atom *>(this)->getReply();
    return m_atom;
}

QString Atom::nameFromAtom(xcb_atom_t atom)
{
    if (atom == XCB_ATOM_NONE)
        return QStringLiteral("None");

    QString name;

    xcb_generic_error_t *e;
    xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(connection(), atom);
    xcb_get_atom_name_reply_t *reply = xcb_get_atom_name_reply(connection(), cookie, &e);
    if (reply) {
        int length = xcb_get_atom_name_name_length(reply);
        char *n = xcb_get_atom_name_name(reply);
        name = name.asprintf("%.*s", length, n);
    } else {
        name = name.asprintf("(atom %u)", atom);
    }

    free(reply);

    return name;
}

void Atom::getReply()
{
    if (m_retrieved || !m_cookie.sequence)
        return;

    xcb_intern_atom_reply_t *reply =
            xcb_intern_atom_reply(connection(), m_cookie, nullptr);
    m_atom = reply->atom;
    free(reply);
    m_retrieved = true;
}

} // namespace Xcb
