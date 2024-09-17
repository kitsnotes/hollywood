#pragma once

#include <QApplication>

class QMenuBar;
class HWAboutDialog;
class HorizonWizard;
class HorizonSetupApplication : public QApplication
{
    Q_OBJECT
public:
    HorizonSetupApplication(int &argc, char **argv);
    static HorizonSetupApplication* instance() { return static_cast<HorizonSetupApplication*>(QApplication::instance()); }
    void showWizard();
private slots:
    void about();
private:
    QMenuBar *m_menubar = nullptr;
    HWAboutDialog *m_about = nullptr;
    HorizonWizard *m_wizard = nullptr;
};
