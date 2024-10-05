// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-2.1

#pragma once

#include "libshell_int.h"

class LSActionManagerPrivate;
class LSActionManager : public QObject
{
    Q_OBJECT
public:
    explicit LSActionManager(QObject *parent = nullptr);
    QAction* shellAction(HWShell::ShellActions action);
    QActionGroup* groupViewMode();
    QActionGroup* groupViewColumn();
    QActionGroup* groupViewOrder();
    QMenu* openWithMenu();
    QMenu* newMenu();

signals:
private:
    void setupActions();
private:
    LSActionManagerPrivate *d;
};
