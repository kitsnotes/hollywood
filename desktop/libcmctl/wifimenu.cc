// Hollywood Connman Control Library
// (C) 2021, 2022 Originull Software
// This class is based on Shadoware XINX QMenuView
// Copyright (C) 2007-2011 Ulrich Van Den Hekke
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wifimenu.h"
#include "private/wifimenu_p.h"
#include "connman.h"
#include "model.h"
#include "private/connman_p.h"

HWCM::WifiMenuPrivate::WifiMenuPrivate(ConnmanPrivate *conn, WifiMenu *parent)
    : QObject(parent)
    , m_q(parent)
{
    m_model = conn->m_wifiModel;
}

QAction *HWCM::WifiMenuPrivate::actionFromIndex(const QModelIndex &index)
{
    auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    auto title = index.data(Qt::DisplayRole).toString();

    auto act = new QAction(icon, title, this);
    QVariant idx;
    idx.setValue(index);
    act->setData(idx);

    return act;
}

void HWCM::WifiMenuPrivate::aboutToShow()
{
    auto menu = qobject_cast<QMenu*>(sender());
    if(menu)
    {
        QVariant idx = menu->menuAction()->data();
        if(idx.canConvert<QModelIndex>())
        {
            auto index = qvariant_cast<QModelIndex>(idx);
            m_q->createMenu(index, menu, menu);
            disconnect(menu, &WifiMenu::aboutToShow, this, &WifiMenuPrivate::aboutToShow);
        }
    }

    m_q->clear();

    m_q->createMenu(m_root, m_q, m_q);
    m_q->addSeparator();
    m_q->addAction(tr("Join other network..."));
}

void HWCM::WifiMenuPrivate::triggered(QAction *act)
{
    auto v = act->data();
    if(v.canConvert<QModelIndex>())
    {
        auto idx = qvariant_cast<QModelIndex>(v);
        Q_EMIT m_q->triggeredIndex(idx);
    }
}

void HWCM::WifiMenuPrivate::hovered(QAction *act)
{
    auto v = act->data();
    if(v.canConvert<QModelIndex>())
    {
        auto idx = qvariant_cast<QModelIndex>(v);
        auto str = idx.data(Qt::StatusTipRole).toString();
        Q_EMIT m_q->hoveredIndex(str);
    }
}

HWCM::WifiMenu::WifiMenu(ConnmanPrivate *parent)
    : QMenu(nullptr)
    , m_p(new WifiMenuPrivate(parent, this))
{
    connect(this, &WifiMenu::triggered, m_p.data(), &WifiMenuPrivate::triggered);
    connect(this, &WifiMenu::hovered, m_p.data(), &WifiMenuPrivate::hovered);
    connect(this, &WifiMenu::aboutToShow, m_p.data(), &WifiMenuPrivate::aboutToShow);
}

HWCM::WifiMenu::~WifiMenu() {}

void HWCM::WifiMenu::createMenu(const QModelIndex &idx, QMenu *parent, QMenu *menu)
{
    if(!menu)
    {
        auto ico = qvariant_cast<QIcon>(idx.data(Qt::DecorationRole));
        QVariant v;
        v.setValue(parent);

        menu = new QMenu(idx.data(Qt::DisplayRole).toString());
        menu->setIcon(ico);
        parent->addMenu(menu);
        menu->menuAction()->setData(v);
        menu->setEnabled(idx.flags().testFlag(Qt::ItemIsEnabled));

        connect(menu, &QMenu::aboutToShow, m_p.data(), &WifiMenuPrivate::aboutToShow);
        return;
    }

    int end = m_p->m_model->rowCount();
    for(int i = 0; i < end; ++i)
    {
        auto newidx = m_p->m_model->index(i, 0);
        menu->addAction(m_p->actionFromIndex(newidx));
    }
}
