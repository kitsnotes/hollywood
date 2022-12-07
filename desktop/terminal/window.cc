#include "window.h"
#include "tabhost.h"
#include "preferences.h"
#include "application.h"
#include "terminal.h"

#include <QMenuBar>
#include <QLayout>
#include <QMimeData>
#include <QFileDialog>
#include <QClipboard>
#include <QMessageBox>
#if QT_VERSION >= 0x060000
#include <qtermwidget6/qtermwidget.h>
#else
#include <qtermwidget5/qtermwidget.h>
#endif

#include <aboutdialog.h>
#include <QSettings>
TerminalWindow::TerminalWindow(QWidget *parent)
    : QMainWindow(parent),
      m_tabs(new TabHost(this))
{
    setWindowIcon(QIcon::fromTheme("utilities-terminal"));
    setupActions();
    setCentralWidget(m_tabs);
    connect(TerminalApplication::instance(), &TerminalApplication::profileRemoved,
            m_tabs, &TabHost::profileDeleted);
    newTab();

    QSettings settings("originull", "terminull");
    if(settings.contains("Cache/LastWindowPos"))
        restoreGeometry(settings.value("Cache/LastWindowPos").toByteArray());
    else
    {
        // fallback defaults
        resize(560,440);
        move(screen()->geometry().center() - frameGeometry().center());
    }

    connect(TerminalApplication::instance()->clipboard(), &QClipboard::dataChanged,
            this, &TerminalWindow::clipboardDataChanged);
    connect(m_tabs, &TabHost::requestWindowClose, this, &TerminalWindow::close);
    connect(m_tabs, &TabHost::customContextMenuRequested, this, &TerminalWindow::contextMenuRequested);
}

TerminalWindow::~TerminalWindow()
{
}

void TerminalWindow::setupActions()
{
    auto app = TerminalApplication::instance();
    auto menubar = new QMenuBar(this);
    setMenuBar(menubar);

    auto shell = menubar->addMenu(tr("&Shell"));
    auto newwnd = shell->addAction(tr("New &Window"));
    newwnd->setObjectName("NewWindow");
    newwnd->setShortcut(QKeySequence("Ctrl+Shift+N"));
    connect(newwnd, &QAction::triggered, app, &TerminalApplication::newWindow);

    auto newtab = shell->addAction(tr("New &Tab"));
    newtab->setShortcut(QKeySequence("Ctrl+Shift+T"));
    newtab->setObjectName("NewTab");
    connect(newtab, &QAction::triggered, this, &TerminalWindow::newTab);
    shell->addSeparator();

    auto savebuffer = shell->addAction(tr("&Save Buffer..."));
    savebuffer->setObjectName("SaveBuffer");
    connect(savebuffer, &QAction::triggered, this, &TerminalWindow::saveToFile);

    m_saveSelection = shell->addAction(tr("Save S&election..."));
    m_saveSelection->setObjectName("SaveSelection");
    connect(m_saveSelection, &QAction::triggered, this, &TerminalWindow::saveToFile);

    auto quit = shell->addAction(tr("&Quit Terminal"));
    quit->setObjectName("QuitTerminal");
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    auto edit = menubar->addMenu(tr("&Edit"));

    // basically just for show to have symmetry with other apps
    // i don't know if this is a good idea yet or not --cat
    auto cut = edit->addAction(tr("&Cut"));
    cut->setShortcut(QKeySequence("Ctrl+Shift+X"));
    cut->setDisabled(true);
    // real options
    m_copy = edit->addAction(tr("C&opy"));
    m_copy->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(m_copy, &QAction::triggered, m_tabs, &TabHost::copySelection);

    m_pasteClipboard = edit->addAction(tr("&Paste"));
    m_pasteClipboard->setShortcut(QKeySequence("Ctrl+Shift+V"));
    connect(m_pasteClipboard, &QAction::triggered, m_tabs, &TabHost::pasteClipboard);

    m_pasteSelection = edit->addAction(tr("Paste &Selection"));
    m_pasteSelection->setShortcut(QKeySequence("Shift+Ins"));
    connect(m_pasteSelection, &QAction::triggered, m_tabs, &TabHost::pasteSelection);

    edit->addSeparator();
    auto clear = edit->addAction(tr("C&lear"));
    connect(clear, &QAction::triggered, m_tabs, &TabHost::clear);

    edit->addSeparator();
    auto find = edit->addAction(tr("&Find..."));
    find->setShortcut(QKeySequence("Ctrl+Shift+F"));
    auto view = menubar->addMenu(tr("&View"));
    auto zoomin = view->addAction(tr("Make Text &Bigger"));
    zoomin->setShortcut(QKeySequence("Ctrl+Shift++"));
    connect(zoomin, &QAction::triggered, m_tabs, &TabHost::zoomIn);

    auto zoomout = view->addAction(tr("Make Text &Smaller"));
    zoomout->setShortcut(QKeySequence("Ctrl+Shift+-"));
    connect(zoomout, &QAction::triggered, m_tabs, &TabHost::zoomOut);

    auto zoomnormal = view->addAction(tr("&Normal Text Size"));
    zoomnormal->setShortcut(QKeySequence("Ctrl+Shift+0"));
    connect(zoomnormal, &QAction::triggered, m_tabs, &TabHost::zoomNormal);

    view->addSeparator();
    auto prefs = view->addAction(tr("&Preferences"));
    connect(prefs, &QAction::triggered, this, &TerminalWindow::showPreferences);

    auto window = menubar->addMenu(tr("&Window"));
    window->setDisabled(true);
    auto help = menubar->addMenu(tr("&Help"));
    auto about = help->addAction(tr("&About Terminal"));
    connect(about, &QAction::triggered, this, &TerminalWindow::aboutTerminal);
}

void TerminalWindow::closeEvent(QCloseEvent *event)
{
    if(m_tabs->requestCloseAll())
    {
        event->accept();
        return;
    }

    QMessageBox question(this);
    question.setIcon(QMessageBox::Question);
    question.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    question.setText(tr("Do you want to terminate all running processes in this window?"));
    question.setInformativeText(tr("Process List Here"));
    int result = question.exec();
    if(result == QMessageBox::Yes)
    {
        event->accept();
        return;
    }

    event->ignore();
}

void TerminalWindow::newTab()
{
    m_tabs->newDefaultTab();
    disableSelectionRelevantActions();
}

void TerminalWindow::currentTabChanged()
{
    disableSelectionRelevantActions();
    auto tw = m_tabs->currentHost();
    if(!tw->widget()->selectedText().isEmpty())
        enableSelectionRelevantActions();
}

void TerminalWindow::showPreferences()
{
    auto prefs = new PreferencesDialog(this);
    connect(prefs, &PreferencesDialog::finished, prefs, &PreferencesDialog::deleteLater);
    prefs->show();
}

void TerminalWindow::aboutTerminal()
{
    auto about = new HWAboutDialog(this);
    about->setAppDescription(tr("Terminal emulator for Hollywood Linux\nAPI Version %1")
                             .arg(QLatin1String(QT_VERSION_STR)));
    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();

}

void TerminalWindow::clipboardDataChanged()
{
    auto pb = TerminalApplication::instance()->clipboard();
    if(pb->mimeData()->hasText() ||
       pb->mimeData()->hasUrls() ||
       pb->mimeData()->hasHtml())
    {
        m_pasteClipboard->setEnabled(true);
        return;
    }
    m_pasteClipboard->setEnabled(false);
}

void TerminalWindow::saveToFile()
{
    bool savesel = false;
    auto action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);

    if(action->objectName() == "SaveSelection")
        savesel = true;

    QFileDialog sd(this);
    sd.setAcceptMode(QFileDialog::AcceptSave);
    sd.setFileMode(QFileDialog::AnyFile);
    if(savesel)
        sd.setWindowTitle(tr("Save Selection"));
    else
        sd.setWindowTitle(tr("Save Buffer"));

    sd.setNameFilters({tr("Text File (*.txt)"), tr("All Files (*)")});
    auto result = sd.exec();
    if(result == QDialog::Accepted)
    {
        auto fn = sd.selectedFiles().first();
        QFile file(fn);
        if(file.open(QFile::WriteOnly))
        {
            if(savesel)
                file.write(m_tabs->currentHost()->widget()->selectedText().toUtf8());
            else
                m_tabs->currentHost()->widget()->saveHistory(&file);
            file.close();
        }
        else
        {
            QMessageBox msg(this);
            msg.setWindowTitle(tr("Save Failure"));
            msg.setText(tr("Unable to save buffer to %1.  Check permissions and try again, or save to a different file.").arg(fn));
            msg.setIcon(QMessageBox::Critical);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
    }
}

void TerminalWindow::contextMenuRequested(const QPoint &pos)
{
    qDebug() << "TerminalWindow::contextMenuRequested";
    QMenu menu;

    menu.addAction(m_copy);
    menu.addAction(m_pasteClipboard);
    menu.addAction(m_pasteSelection);
    auto act = menu.exec(mapToGlobal(pos));
    if(act == nullptr)
        return;

}

void TerminalWindow::disableSelectionRelevantActions()
{
    m_copy->setDisabled(true);
    m_pasteSelection->setDisabled(true);
    m_saveSelection->setDisabled(true);
}

void TerminalWindow::enableSelectionRelevantActions()
{
    m_copy->setDisabled(false);
    m_pasteSelection->setDisabled(false);
    m_saveSelection->setDisabled(false);
}
