// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on Shadoware XINX QMenuView
// Copyright (C) 2007-2011 Ulrich Van Den Hekke
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIFIMENU_H
#define WIFIMENU_H

#include <QMenu>

namespace HWCM
{
class ConnmanPrivate;
class WifiMenuPrivate;
class WifiMenu : public QMenu
{
    Q_OBJECT
public:
    ~WifiMenu();
protected:
    explicit WifiMenu(ConnmanPrivate *parent);
    void createMenu(const QModelIndex &idx, QMenu *parent = nullptr, QMenu *menu = nullptr);
signals:
    void hoveredIndex(const QString &text);
    void triggeredIndex(const QModelIndex &idx);
private:
    friend class ConnmanPrivate;
    friend class WifiMenuPrivate;
    QScopedPointer<WifiMenuPrivate> m_p;
};

};
#endif // WIFIMENU_H
