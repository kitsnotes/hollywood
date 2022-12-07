/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2011-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QApplication>
#include <glib-object.h>
#include <QCommandLineParser>

#include "agent.h"

int main(int argc, char *argv[])
{
#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Elevator");
    app.setDesktopFileName("org.originull.elevator.desktop");
    app.setWindowIcon(QIcon::fromTheme("preferences-security"));
    app.setQuitOnLastWindowClosed(false);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Hollywood Elevator"));
    /* const QString VERINFO = QStringLiteral(LXQT_POLKITAGENT_VERSION
                                           "\nliblxqt   " LXQT_VERSION
                                           "\nQt        " QT_VERSION_STR);
    app.setApplicationVersion(VERINFO);*/
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    PolicykitAgent agent;
    if(!agent.registered())
    {
        QMessageBox msg(QMessageBox::Critical, QApplication::translate("main", "Elevator Registration Failed"), QApplication::translate("main", "Failed to register Hollywood Elevator as polkit agent."), QMessageBox::Ok, nullptr);
        msg.setInformativeText(QApplication::translate("main", "There is already a polkit agent running on the system or there is an underlying polkit issue. Check system logs for further details."));
        msg.exec();
        app.exit();
        return 0;
    }
    return app.exec();
}
