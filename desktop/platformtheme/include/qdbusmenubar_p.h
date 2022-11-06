/*
    SPDX-FileCopyrightText: 2016 Dmitry Shachnev <mitya57@gmail.com>
    Contact: https://www.qt.io/licensing/
    This file is part of the QtGui module of the Qt Toolkit.
    SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KFQF-Accepted-GPL OR LicenseRef-Qt-Commercial
*/

#ifndef QDBUSMENUBAR_P_H
#define QDBUSMENUBAR_P_H
#include "platformtheme.h"

#include <QHash>
#include <QString>
#include <QWindow>

#include "qdbusmenuadaptor_p.h"
#include "qdbusmenuconnection_p.h"
#include "qdbusmenuregistrarproxy_p.h"
#include "qdbusplatformmenu_p.h"

QT_BEGIN_NAMESPACE

class DBusMenuBar : public QPlatformMenuBar
{
    Q_OBJECT

public:
    DBusMenuBar(HollywoodPlatformTheme *platformTheme);
    ~DBusMenuBar() override;

    void insertMenu(QPlatformMenu *menu, QPlatformMenu *before) override;
    void removeMenu(QPlatformMenu *menu) override;
    void syncMenu(QPlatformMenu *menu) override;
    void handleReparent(QWindow *newParentWindow) override;
    QPlatformMenu *menuForTag(quintptr tag) const override;
    QPlatformMenu *createMenu() const override;
    static DBusMenuBar *globalMenuBar();
    static DBusMenuBar *menuBarForWindow(QWindow *window);

    QWindow *window() const
    {
        return m_window;
    }
    QString objectPath() const
    {
        return m_objectPath;
    }

Q_SIGNALS:
    void windowChanged(QWindow *newWindow, QWindow *oldWindow);

private:
    DBusPlatformMenu *m_menu = nullptr;
    DBusMenuAdaptor *m_menuAdaptor = nullptr;
    QHash<quintptr, DBusPlatformMenuItem *> m_menuItems;
    QPointer<QWindow> m_window;
    QString m_objectPath;
    bool m_initted = false;
    HollywoodPlatformTheme *m_platformTheme;
    static DBusMenuBar *s_globalMenuBar;
    static QHash<QWindow *, DBusMenuBar *> s_menuBars;

    DBusPlatformMenuItem *menuItemForMenu(QPlatformMenu *menu);
    static void updateMenuItem(DBusPlatformMenuItem *item, QPlatformMenu *menu);
    bool createDBusMenuBar();
    void uncreateDBusMenuBar();

    static void registerMenuBarX11(QWindow *window, const QString &objectPath);
    static void unregisterMenuBarX11(QWindow *window);
};

QT_END_NAMESPACE

#endif // QDBUSMENUBAR_P_H
