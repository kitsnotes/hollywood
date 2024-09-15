#pragma once
#include <QObject>
#include <QWidget>
#include <QToolButton>
#include <QSharedPointer>
#include <QList>

class AbstractTaskViewItem : public QToolButton
{

};
typedef QList<AbstractTaskViewItem*> TaskItems;


class AbstractTaskView : public QObject
{
public:
    virtual TaskItems taskItems() = 0;
signals:
    virtual void itemCreated(AbstractTaskViewItem *btn, int position) = 0;
    virtual void itemRemoved(AbstractTaskViewItem *btn) = 0;
};
