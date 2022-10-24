// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on Shadoware XINX QMenuView
// Copyright (C) 2007-2011 Ulrich Van Den Hekke
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIFIMENU_P_H
#define WIFIMENU_P_H

#include "libcmctl_global.h"
#include <QAbstractItemModel>

#include "wifimenu.h"

namespace HWCM
{
class ConnmanPrivate;
class WifiModel;
class WifiMenuPrivate : public QObject
{
    Q_OBJECT
public:
    explicit WifiMenuPrivate(ConnmanPrivate *conn, WifiMenu *parent);
    ~WifiMenuPrivate() = default;
private:
    friend class WifiMenu;
    QAction *actionFromIndex(const QModelIndex &index);
private slots:
    void aboutToShow();
    void triggered(QAction *act);
    void hovered(QAction *act);
private:
    WifiMenu *m_q;
    WifiModel *m_model;
    QPersistentModelIndex m_root;
    int limitNetworks = 0;
};
}

#endif // WIFIMENU_P_H
