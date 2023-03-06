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
class SetupAssistant;
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
   void startSideBySide();
private:
   void createGlobalMenu();
   bool runHardwareTest();
private:
   QMenuBar *m_menubar = nullptr;
   MenuDialog *m_menu = nullptr;
   SetupAssistant *m_setup = nullptr;
   QStringList m_hwfail;
   bool m_hwtest  = false;
};

#endif // BOOTSTRAP_H
