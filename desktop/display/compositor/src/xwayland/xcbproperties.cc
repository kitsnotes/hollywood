// Hollywood Wayland Compositor
// SPDX-FileCopyrightText: 2021-2024 Originull Software
// Xwayland support originated from liri
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "xcbproperties.h"
#include "xcbresources.h"
#include "xcbwrapper.h"
#include "xwayland.h"

namespace Xcb {

namespace Properties {

#if 0
void dump(xcb_atom_t property, xcb_get_property_reply_t *reply)
{
    if (!reply)
        return;

    qint32 *incrValue;
    const char *textValue, *name;
    int len;
    xcb_atom_t *atomValue;

    if (reply->type == Xcb::resources()->atoms->incr) {
        incrValue = static_cast<qint32 *>(xcb_get_property_value(reply));
    } else if (reply->type == Xcb::resources()->atoms->utf8_string || reply->type == Xcb::resources()->atoms->string) {
        if (reply->value_len > 40)
            len = 40;
        else
            len = reply->value_len;
    } else if (reply->type == XCB_ATOM_ATOM) {
        atomValue = reinterpret_cast<xcb_atom_t *>(xcb_get_property_value(reply));
        for (int i = 0; i < reply->value_len; i++)
            name = strdup(Xcb::Atom::nameFromAtom(atomValue[i]).toUtf8().constData());
    }
}
#endif

void dumpProperty(xcb_atom_t property, xcb_get_property_reply_t *reply)
{
    QString buffer = QStringLiteral("\t%1: ").arg(Xcb::Atom::nameFromAtom(property));

    if (!reply) {
        buffer += QLatin1String("(no reply)");
        qCDebug(gLcXwaylandTrace) << qPrintable(buffer);
        return;
    }

    buffer += QString::asprintf("%s/%d, length %d (value_len %d): ",
                                qPrintable(Xcb::Atom::nameFromAtom(reply->type)),
                                reply->format,
                                xcb_get_property_value_length(reply),
                                reply->value_len);

    if (reply->type == Xcb::resources()->atoms->incr) {
        qint32 *v = (qint32 *)xcb_get_property_value(reply);
        buffer += QString::asprintf("%d", *v);
    } else if (reply->type == Xcb::resources()->atoms->utf8_string ||
               reply->type == Xcb::resources()->atoms->string) {
        const char *v = (const char *)xcb_get_property_value(reply);
        int len = reply->value_len > 40 ? 40 : reply->value_len;
        buffer += QString::asprintf("\"%.*s\"", len, v);
    } else if (reply->type == XCB_ATOM_ATOM) {
        xcb_atom_t *v = (xcb_atom_t *)xcb_get_property_value(reply);

        for (quint32 i = 0; i < reply->value_len; i++) {
            QString name = Xcb::Atom::nameFromAtom(v[i]);
            if (i > 0)
                buffer += QLatin1String(", ");
            buffer += name;
        }
    } else {
        buffer += QStringLiteral("huh?");
    }

    qCDebug(gLcXwaylandTrace) << qPrintable(buffer);
}

void readAndDumpProperty(xcb_atom_t atom, xcb_window_t window)
{
    xcb_get_property_cookie_t cookie =
            xcb_get_property(Xcb::connection(), 0, window,
                             atom, XCB_ATOM_ANY, 0, 2048);
    xcb_get_property_reply_t *reply =
            xcb_get_property_reply(Xcb::connection(), cookie, nullptr);

    dumpProperty(atom, reply);

    free(reply);
}

} // namespace Cursors

} // namespace Xcb
