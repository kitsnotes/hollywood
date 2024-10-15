#include "bootstrap.h"
#include "menu.h"
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QMessageBox>

#include <hollywood/aboutdialog.h>

#define VERSTR "1.0.67"

HWUtilities::HWUtilities(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setOrganizationName("Originull Software");
    setApplicationName("Rescue Utilities");
    setApplicationVersion(VERSTR);

    setWindowIcon(QIcon::fromTheme("hollywood-logo"));
    createGlobalMenu();
}

void HWUtilities::showMenu()
{
    if(!m_menu)
        m_menu = new MenuDialog(0);

    m_menu->show();
}

void HWUtilities::aboutBootstrap()
{
    if(m_about != nullptr)
        return;

    m_about = new HWAboutDialog(nullptr);
    connect(m_about, &HWAboutDialog::finished, [this](int r) {
        Q_UNUSED(r);
        m_about->deleteLater();
        m_about = nullptr;
    });
    m_about->show();
}


void HWUtilities::createGlobalMenu()
{
    m_menubar = new QMenuBar(nullptr);
    // this is a vanity menu
    auto edit = m_menubar->addMenu(tr("&Edit"));
    auto undo = edit->addAction(tr("&Undo"));
    auto redo = edit->addAction(tr("&Redo"));
    edit->addSeparator();
    auto cut = edit->addAction(tr("&Cut"));
    auto copy = edit->addAction(tr("C&opy"));
    auto paste = edit->addAction(tr("&Paste"));
    auto sa = edit->addAction(tr("&Select All"));

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);
    cut->setShortcut(QKeySequence::Cut);
    copy->setShortcut(QKeySequence::Copy);
    paste->setShortcut(QKeySequence::Paste);
    sa->setShortcut(QKeySequence::SelectAll);

    undo->setEnabled(false);
    redo->setEnabled(false);
    cut->setEnabled(false);
    copy->setEnabled(false);
    paste->setEnabled(false);
    sa->setEnabled(false);

    auto tools = m_menubar->addMenu(tr("&Tools"));
    auto hwtest = tools->addAction(tr("Check &Hardware Requirements"));
    hwtest->setDisabled(true);
    auto wifitool = tools->addAction(tr("&Wireless Firmware Transfer"));
    auto help = m_menubar->addMenu(tr("&Help"));
    auto about = help->addAction(tr("&About Setup Assistant"));
    about->setMenuRole(QAction::AboutRole);
    connect(about, &QAction::triggered, this, &HWUtilities::aboutBootstrap);
}


void HWUtilities::launch()
{
    auto btn = qobject_cast<QCommandLinkButton*>(sender());
    if(!btn)
        return;

    // special handling
    if(btn == m_menu->m_reinstall)
        return doReinstall();

    auto launchCmd = QString();
    if(btn == m_menu->m_browser)
        launchCmd = QString("/usr/bin/quickweb");
    if(btn == m_menu->m_terminull)
        launchCmd = QString("/usr/bin/terminull");
    if(btn == m_menu->m_rescue)
        launchCmd = QString("sudo -E /usr/bin/hwrescue");

    m_proc = new QProcess(this);
    m_proc->setProgram(launchCmd);
    connect(m_proc, &QProcess::finished, [this, btn](int) {
        btn->clearFocus();
        m_menu->clearFocus();
        m_menu->show();
    });
    m_menu->hide();
    m_proc->start();
}

void HWUtilities::doReinstall()
{
    // start the hollywood-setup
    m_proc = new QProcess(this);
    m_proc->setProgram("sudo");
    m_proc->setArguments(QStringList() << "-E" << "/usr/bin/hollywood-setup");
    connect(m_proc, &QProcess::finished, [this](int ret) {
        if(ret == 0)
        {
            qDebug() <<"finished!";
            // completed successfully - lets start the runner
            this->reinstallSecondStage();
        }
        else
        {
            m_menu->show();
            m_menu->activateWindow();
        }
    });
    m_menu->hide();
    m_proc->start();
}

void HWUtilities::reinstallSecondStage()
{

}

int main(int argc, char *argv[])
{
    HWUtilities a(argc, argv);
    a.showMenu();
    return a.exec();
}
