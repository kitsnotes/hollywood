#pragma once

#include <QApplication>

class QMenuBar;
class HWAboutDialog;
class ExecutorWizard;
class HorizonRunnerApplication : public QApplication
{
    Q_OBJECT
public:
    HorizonRunnerApplication(int &argc, char **argv);
    static HorizonRunnerApplication* instance() { return static_cast<HorizonRunnerApplication*>(QApplication::instance()); }
    void showWizard();
private slots:
    void about();
private:
    QMenuBar *m_menubar = nullptr;
    HWAboutDialog *m_about = nullptr;
    ExecutorWizard *m_wizard = nullptr;
};
