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

#ifndef MENUREGISTRARIMPORTER_H
#define MENUREGISTRARIMPORTER_H

// Qt
#include <QDBusArgument>
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QObject>
#include <QWidget> // For WId
#include <QIcon>
#include <QMenu>
#include <QtDBus/QtDBus>

class QDBusObjectPath;
class QDBusServiceWatcher;

class MenuRegistrarImporter : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit MenuRegistrarImporter(QObject *);
    ~MenuRegistrarImporter() override;

    bool connectToBus();

    bool serviceExist(WId id)
    {
        return m_menuServices.contains(id);
    }
    QString serviceForWindow(WId id)
    {
        return m_menuServices.value(id);
    }

    bool pathExist(WId id)
    {
        return m_menuPaths.contains(id);
    }
    QString pathForWindow(WId id)
    {
        return m_menuPaths.value(id).path();
    }

    QList<WId> ids()
    {
        return m_menuServices.keys();
    }

Q_SIGNALS:
    void WindowRegistered(WId id, const QString &service, const QDBusObjectPath &);
    void WindowUnregistered(WId id);

public Q_SLOTS:
    Q_NOREPLY void RegisterWindow(WId id, const QDBusObjectPath &path);
    Q_NOREPLY void UnregisterWindow(WId id);
    QString GetMenuForWindow(WId id, QDBusObjectPath &path);

private Q_SLOTS:
    void slotServiceUnregistered(const QString &service);

private:
    QDBusServiceWatcher *m_serviceWatcher;
    QHash<WId, QString> m_menuServices;
    QHash<WId, QDBusObjectPath> m_menuPaths;
    QHash<WId, QString> m_windowClasses;
};

/*
 * Adaptor class for interface com.canonical.AppMenu.Registrar
 * * Command line was: qdbusxml2cpp -a ./include/menuimporteradaptor.h ./com.canonical.AppMenu.registrar.xml
 */
class RegistrarAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.AppMenu.Registrar")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.canonical.AppMenu.Registrar\">\n"
"    <method name=\"RegisterWindow\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
"      <arg direction=\"in\" type=\"o\" name=\"menuObjectPath\"/>\n"
"    </method>\n"
"    <method name=\"UnregisterWindow\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
"    </method>\n"
"    <method name=\"GetMenuForWindow\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"service\"/>\n"
"      <arg direction=\"out\" type=\"o\" name=\"menuObjectPath\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    RegistrarAdaptor(QObject *parent);
    virtual ~RegistrarAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    QString GetMenuForWindow(uint windowId, QDBusObjectPath &menuObjectPath);
    void RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath);
    void UnregisterWindow(uint windowId);
Q_SIGNALS: // SIGNALS
};


#endif /* MENUREGISTRARIMPORTER_H */
