/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#pragma once

#include <QObject>
#include <QStringList>
#include <memory>

class StatusNotifierWidget;
class StatusNotifierWatcher;

class StatusNotifierProxy : public QObject
{
    Q_OBJECT

public:
    StatusNotifierProxy();
    ~StatusNotifierProxy();
    QStringList RegisteredStatusNotifierItems() const;
    static StatusNotifierProxy & registerLifetimeUsage(QObject * obj);

private:
    std::unique_ptr<StatusNotifierWatcher> mWatcher;
    int mUsersCount;

    void createWatcher();
    void registerUsage(QObject * obj);
    void unregisterUsage();

signals:
    void StatusNotifierItemRegistered(const QString &service);
    void StatusNotifierItemUnregistered(const QString &service);
};
