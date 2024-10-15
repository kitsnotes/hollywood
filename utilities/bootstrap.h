#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QProcess>

#if defined(bsApp)
#undef bsApp
#endif
#define bsApp (HWBootstrap::instance())

class HWAboutDialog;
class MenuDialog;
class SetupAssistant;
class HWUtilities : public QApplication
{
public:
    HWUtilities(int &argc, char **argv);
    static HWUtilities* instance() { return static_cast<HWUtilities*>(QApplication::instance()); }

public slots:
   void showMenu();
   void aboutBootstrap();
   void launch();
   void doReinstall();
   void reinstallSecondStage();
private:
   void createGlobalMenu();
private:
   QMenuBar *m_menubar = nullptr;
   MenuDialog *m_menu = nullptr;
   HWAboutDialog *m_about = nullptr;
   QProcess *m_proc = nullptr;
};

#endif // BOOTSTRAP_H
