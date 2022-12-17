#ifndef LSACTIONMANAGER_H
#define LSACTIONMANAGER_H

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

#endif // LSACTIONMANAGER_H
