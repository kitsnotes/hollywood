// Hollywood Notification Daemon
// (C) 2021, 2022 Cat Stevenson <cat@originull.org>
// SPDX-License-Identifier: GPL-2.0-or-later

// Command line was: qdbusxml2cpp -l NotificationDaaemon -i adaptor.h -a adaptor.cc ./org.freedesktop.Notifications.xml

#ifndef ADAPTOR_CC
#define ADAPTOR_CC

#include <QtCore/QObject>
#include <QtCore/QMetaObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "adaptor.h"
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "daemon.h"

/*
 * Adaptor class for interface org.freedesktop.Notifications
 */
class NotificationsAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.Notifications\">\n"
"    <signal name=\"NotificationClosed\">\n"
"      <arg direction=\"out\" type=\"u\" name=\"id\"/>\n"
"      <arg direction=\"out\" type=\"u\" name=\"reason\"/>\n"
"    </signal>\n"
"    <signal name=\"ActionInvoked\">\n"
"      <arg direction=\"out\" type=\"u\" name=\"id\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"action_key\"/>\n"
"    </signal>\n"
"    <method name=\"Notify\">\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.In6\"/>\n"
"      <arg direction=\"out\" type=\"u\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_name\"/>\n"
"      <arg direction=\"in\" type=\"u\" name=\"replaces_id\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"app_icon\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"summary\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"body\"/>\n"
"      <arg direction=\"in\" type=\"as\" name=\"actions\"/>\n"
"      <arg direction=\"in\" type=\"a{sv}\" name=\"hints\"/>\n"
"      <arg direction=\"in\" type=\"i\" name=\"timeout\"/>\n"
"    </method>\n"
"    <method name=\"CloseNotification\">\n"
"      <arg direction=\"in\" type=\"u\" name=\"id\"/>\n"
"    </method>\n"
"    <method name=\"GetCapabilities\">\n"
"      <arg direction=\"out\" type=\"as\" name=\"caps\"/>\n"
"    </method>\n"
"    <method name=\"GetServerInformation\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"name\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"vendor\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"version\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"spec_version\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    NotificationsAdaptor(NotificationDaemon *parent);
    virtual ~NotificationsAdaptor();

    inline NotificationDaemon *parent() const
    { return static_cast<NotificationDaemon *>(QObject::parent()); }

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void CloseNotification(uint id);
    QStringList GetCapabilities();
    QString GetServerInformation(QString &vendor, QString &version, QString &spec_version);
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantMap &hints, int timeout);
Q_SIGNALS: // SIGNALS
    void ActionInvoked(uint id, const QString &action_key);
    void NotificationClosed(uint id, uint reason);
};

#endif
