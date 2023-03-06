#include "bootstrap.h"
#include "menu.h"
#include "assistant.h"
#include <style.h>
#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdio.h>
#endif
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QMessageBox>


HWBootstrap::HWBootstrap(int &argc, char **argv)
    : QApplication(argc, argv)
{
    auto s = new ArionStyle();
    setStyle(s);
#ifdef Q_OS_MAC
    createGlobalMenu();
#endif
#ifdef Q_OS_WIN
    auto p = palette();
    p.setColor(QPalette::Window, QColor::fromRgb(239,239,239));
    setPalette(p);
#endif
}

void HWBootstrap::showMenu()
{
    if(!m_menu)
        m_menu = new MenuDialog(0);

    m_menu->show();
}

void HWBootstrap::aboutBootstrap()
{
    QMessageBox::about(0, tr("About Hollywood Setup Assistant"),
                       tr("Hollywood Setup Assistant 1.0\n(C) 2023 Originull Software"));
}

void HWBootstrap::initHardwareTest()
{
    m_hwfail.clear();

    if(!m_hwtest)
    {

    }
}

void HWBootstrap::wifiFirmwareTransfer()
{
#ifdef Q_OS_MAC
    QFile list(":/FirmwareList");
    QList<QByteArray> models;
    if(list.open(QIODevice::ReadOnly))
    {
        models = list.readAll().split('\n');
        list.close();
    }
    char buffer[30];
    size_t bufferlen = 30;
    sysctlbyname("hw.model",&buffer,&bufferlen,NULL,0);
    QByteArray model(buffer);

    if(models.contains(model))
    {
        QDir resourceDir = QDir(QApplication::applicationDirPath());
        resourceDir.cdUp();
        resourceDir.cd("Resources");
        QProcess proc;
        proc.startDetached("bash",
            QStringList() << "-c" <<
            QString("SUDO_ASKPASS='%1' sudo --askpass bash %2").arg(resourceDir.absoluteFilePath("askpass-macos.js"), resourceDir.absoluteFilePath("t2-firmware-dump.sh")));

        CFOptionFlags cfRes;
        CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, NULL, NULL, NULL,
                                       CFSTR("Firmware transfer complete."),
                                       CFSTR("Wireless firmware has been successfully staged for Hollywood installation."),
                                       NULL, NULL, NULL, &cfRes);
    }
    else
    {
        CFOptionFlags cfRes;
        CFUserNotificationDisplayAlert(0, kCFUserNotificationNoteAlertLevel, NULL, NULL, NULL,
                                       CFSTR("Firmware transfer not required."),
                                       CFSTR("This system does not require a transfer of wireless firmware."),
                                       NULL, NULL, NULL, &cfRes);
    }
#endif
}

void HWBootstrap::startSideBySide()
{
    if(!m_setup)
        m_setup = new SetupAssistant(0);

    m_setup->show();
}

void HWBootstrap::createGlobalMenu()
{
    m_menubar = new QMenuBar(nullptr);
#ifdef Q_OS_MAC
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
#endif

    auto tools = m_menubar->addMenu(tr("&Tools"));
    auto hwtest = tools->addAction(tr("Check &Hardware Requirements"));
    hwtest->setDisabled(true);
    auto wifitool = tools->addAction(tr("&Wireless Firmware Transfer"));
    connect(wifitool, &QAction::triggered, this, &HWBootstrap::wifiFirmwareTransfer);
    auto help = m_menubar->addMenu(tr("&Help"));
    auto about = help->addAction(tr("&About Setup Assistant"));
    about->setMenuRole(QAction::AboutRole);
    connect(about, &QAction::triggered, this, &HWBootstrap::aboutBootstrap);
}

bool HWBootstrap::runHardwareTest()
{
#ifdef Q_OS_MAC
    // See if we're on 64-bit EFI
#endif
}

int main(int argc, char *argv[])
{
    HWBootstrap a(argc, argv);
    a.showMenu();
    return a.exec();
}
