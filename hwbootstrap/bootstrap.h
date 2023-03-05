#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <QApplication>
#include <QMenuBar>
#include <QMenu>

#if defined(bsApp)
#undef bsApp
#endif
#define bsApp (HWBootstrap::instance())

class MenuDialog;
class HWBootstrap : public QApplication
{
public:
    HWBootstrap(int &argc, char **argv);
    static HWBootstrap* instance() { return static_cast<HWBootstrap*>(QApplication::instance()); }

public slots:
   void showMenu();
   void aboutBootstrap();
   void initHardwareTest();
   void wifiFirmwareTransfer();
private:
   void createGlobalMenu();
private:
   QMenuBar *m_menubar = nullptr;
   MenuDialog *m_menu = nullptr;
};

#endif // BOOTSTRAP_H
