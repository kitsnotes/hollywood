// Hollywood Stage
// (C) 2022-2024 Originull Software
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTIFIERHOST_H
#define NOTIFIERHOST_H

#include <QObject>
#include "statusnotifier/statusnotifierbutton.h"

class StageHost;
class NotifierHost : public QObject
{
    Q_OBJECT
public:
    explicit NotifierHost(QWidget *parent = nullptr);
signals:
    void buttonAdded(StatusNotifierButton *btn);
    void buttonRemoved(StatusNotifierButton *btn);
private slots:
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);
private:
    QHash<QString,StatusNotifierButton*> m_services;
    QWidget *m_parent;
};

#endif // NOTIFIERHOST_H
