/*

Copyright 2020 vit9696

This file is part of qpdfview.

qpdfview is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

qpdfview is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qpdfview.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QEvent>
#include <QFileOpenEvent>

#include "application.h"
#include "mainwindow.h"

#include <hollywood/hollywood.h>

namespace qpdfview {

Application::Application(int& argc, char** argv) : QApplication(argc, argv),
    m_mainWindow()
{
    setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    setApplicationName("Popular");

    setApplicationVersion(APPLICATION_VERSION);

    setAttribute(Qt::AA_DontShowIconsInMenus);
    setWindowIcon(QIcon::fromTheme("kuiviewer"));
}

void Application::setMainWindow(MainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
}

bool Application::event(QEvent* event)
{
    if(event->type() == QEvent::FileOpen && !m_mainWindow.isNull())
    {
        QFileOpenEvent* openEvent = static_cast< QFileOpenEvent* >(event);

        m_mainWindow->jumpToPageOrOpenInNewTab(openEvent->file(), -1, true);
    }

    return QApplication::event(event);
}

} // qpdfview
