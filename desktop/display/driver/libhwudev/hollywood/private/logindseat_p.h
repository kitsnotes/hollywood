// Hollywood udev interface library
// (C) 2022 Cat Stevenson <cat@originull.org>
// This library originated from Liri OS QtUdev
// (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LOGINDSEAT_P_H
#define LOGINDSEAT_P_H

#include <QDBusArgument>
#include <QVector>

class DBusUserSession
{
public:
    QString id;
    QDBusObjectPath objectPath;
};
Q_DECLARE_METATYPE(DBusUserSession)

typedef QVector<DBusUserSession> DBusUserSessionVector;
Q_DECLARE_METATYPE(DBusUserSessionVector)

class DBusSeat
{
public:
    QString id;
    QDBusObjectPath objectPath;
};
Q_DECLARE_METATYPE(DBusSeat)

QDBusArgument &operator<<(QDBusArgument &argument, const DBusUserSession &userSession);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusUserSession &userSession);

QDBusArgument &operator<<(QDBusArgument &argument, const DBusSeat &seat);
const QDBusArgument &operator>>(const QDBusArgument &argument, DBusSeat &seat);

class LogindSeat
{
public:
    LogindSeat();

    QString id() const;

private:
    QString m_id = QStringLiteral("seat0");

    bool findActiveSession(DBusUserSession &userSession) const;
    QString getSessionType(const QString &sessionId, const QString &sessionPath) const;
    QString getSessionState(const QString &sessionId, const QString &sessionPath) const;
    QString getSeatId(const QString &sessionPath) const;
};

#endif // LOGINDSEAT_P_H
