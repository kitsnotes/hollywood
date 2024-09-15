/*
  Copyright (c) 2011 Lionel Chauvin <megabigbug@yahoo.fr>
  Copyright (c) 2011,2012 Cédric Bellegarde <gnumdk@gmail.com>
  Copyright (c) 2016 Kai Uwe Broulik <kde@privat.broulik.de>

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#include "menuregistrarimporter.h"
#include <dbusmenuimporter.h>

#include <QDBusMessage>
#include <QDBusServiceWatcher>

static const char *DBUS_SERVICE = "com.canonical.AppMenu.Registrar";
static const char *DBUS_OBJECT_PATH = "/com/canonical/AppMenu/Registrar";

RegistrarAdaptor::RegistrarAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

RegistrarAdaptor::~RegistrarAdaptor()
{
    // destructor
}

QString RegistrarAdaptor::GetMenuForWindow(uint windowId, QDBusObjectPath &menuObjectPath)
{
    // handle method call com.canonical.AppMenu.Registrar.GetMenuForWindow
    //return static_cast<YourObjectType *>(parent())->GetMenuForWindow(windowId, menuObjectPath);
    QMetaObject::invokeMethod(parent(), "GetMenuForWindow", Q_ARG(WId, windowId), Q_ARG(QDBusObjectPath, menuObjectPath));
}

void RegistrarAdaptor::RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath)
{
    // handle method call com.canonical.AppMenu.Registrar.RegisterWindow
    QMetaObject::invokeMethod(parent(), "RegisterWindow", Q_ARG(WId, windowId), Q_ARG(QDBusObjectPath, menuObjectPath));
}

void RegistrarAdaptor::UnregisterWindow(uint windowId)
{
    // handle method call com.canonical.AppMenu.Registrar.UnregisterWindow
    QMetaObject::invokeMethod(parent(), "UnregisterWindow", Q_ARG(WId, windowId));
}

MenuRegistrarImporter::MenuRegistrarImporter(QObject *parent)
    : QObject(parent)
    , m_serviceWatcher(new QDBusServiceWatcher(this))
{
    //qDBusRegisterMetaType<DBusMenuLayoutItem>();
    m_serviceWatcher->setConnection(QDBusConnection::sessionBus());
    m_serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &MenuRegistrarImporter::slotServiceUnregistered);
}

MenuRegistrarImporter::~MenuRegistrarImporter()
{
    QDBusConnection::sessionBus().unregisterService(DBUS_SERVICE);
}

bool MenuRegistrarImporter::connectToBus()
{
    if (!QDBusConnection::sessionBus().registerService(DBUS_SERVICE)) {
        return false;
    }
    //ComCanonicalAppMenuRegistrarInterface;
    new RegistrarAdaptor(this);
    QDBusConnection::sessionBus().registerObject(DBUS_OBJECT_PATH, this);

    return true;
}

void MenuRegistrarImporter::RegisterWindow(WId id, const QDBusObjectPath &path)
{
    if (path.path().isEmpty()) // prevent bad dbusmenu usage
        return;

    QString service = message().service();

    // TODO: FIX ME
    QString classClass = "TEST"; //info.windowClassClass();
    m_windowClasses.insert(id, classClass);
    m_menuServices.insert(id, service);
    m_menuPaths.insert(id, path);

    if (!m_serviceWatcher->watchedServices().contains(service)) {
        m_serviceWatcher->addWatchedService(service);
    }

    emit WindowRegistered(id, service, path);
}

void MenuRegistrarImporter::UnregisterWindow(WId id)
{
    m_menuServices.remove(id);
    m_menuPaths.remove(id);
    m_windowClasses.remove(id);

    emit WindowUnregistered(id);
}

QString MenuRegistrarImporter::GetMenuForWindow(WId id, QDBusObjectPath &path)
{
    path = m_menuPaths.value(id);
    return m_menuServices.value(id);
}

void MenuRegistrarImporter::slotServiceUnregistered(const QString &service)
{
    WId id = m_menuServices.key(service);
    m_menuServices.remove(id);
    m_menuPaths.remove(id);
    m_windowClasses.remove(id);
    emit WindowUnregistered(id);
    m_serviceWatcher->removeWatchedService(service);
}

