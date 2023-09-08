/****************************************************************************
** Arion Calculator
** Copyright (C) 2022 The Cat and Contributors to Arion
** spdx-license-identifier: bsd-3-clause
****************************************************************************/

#include <QApplication>
#include <QMenuBar>
#include <QActionGroup>
#include <QSettings>
#include <QClipboard>
#include <QVBoxLayout>
#include <hollywood/aboutdialog.h>

#include "window.h"
#include "calculator.h"

#include <hollywood/hollywood.h>

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QMainWindow(parent),
      m_calc(new Calculator(this)),
      m_tape(new QPlainTextEdit(this))
{
    QSettings settings;
    auto host = new QWidget(this);

    setWindowTitle(tr("Calculator"));
    setWindowIcon(QIcon::fromTheme("accessories-calculator"));

    connect(qApp->clipboard(), &QClipboard::dataChanged, this, &CalculatorWindow::clipboardChanged);
    connect(m_calc, &Calculator::resizeRequest, this, &CalculatorWindow::requestResize);
    auto menu = new QMenuBar(this);
    auto file = menu->addMenu(tr("&File"));
    auto save = file->addAction(tr("&Save Tape..."));
    save->setShortcut(QKeySequence(QKeySequence::Save));
    connect(save, &QAction::triggered, this, &CalculatorWindow::saveTape);
    auto print = file->addAction(tr("&Print Tape..."));
    print->setShortcut(QKeySequence(QKeySequence::Print));
    connect(print, &QAction::triggered, this, &CalculatorWindow::printTape);
    print->setDisabled(true);
    file->addSeparator();
    auto quit = file->addAction(tr("&Quit"));
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    quit->setShortcut(QKeySequence(QKeySequence::Quit));

    auto edit = menu->addMenu(tr("&Edit"));
    auto copy = edit->addAction(tr("&Copy Value"));
    copy->setShortcut(QKeySequence(QKeySequence::Copy));
    connect(copy, &QAction::triggered, this, &CalculatorWindow::copyValue);
    auto copytape = edit->addAction(tr("Copy Paper &Tape"));
    copytape->setShortcut(QKeySequence("Ctrl+Shift+C"));
    copytape->setDisabled(true);
    m_paste = edit->addAction(tr("&Paste"));
    m_paste->setShortcut(QKeySequence(QKeySequence::Paste));
    clipboardChanged();
    connect(m_paste, &QAction::triggered, this, &CalculatorWindow::pasteValue);

    auto view = menu->addMenu(tr("&View"));
    auto basic = view->addAction(tr("&Basic"));
    basic->setObjectName("Basic");
    auto sci = view->addAction(tr("&Scientific"));
    sci->setObjectName("Scientific");
    auto graph = view->addAction(tr("&Graphing"));
    auto dev = view->addAction(tr("&Developer"));
    basic->setCheckable(true);
    sci->setCheckable(true);
    graph->setCheckable(true);
    dev->setCheckable(true);

    basic->setShortcut(QKeySequence("Ctrl+1"));
    sci->setShortcut(QKeySequence("Ctrl+2"));
    graph->setShortcut(QKeySequence("Ctrl+3"));
    dev->setShortcut(QKeySequence("Ctrl+4"));

    auto ag = new QActionGroup(this);
    ag->addAction(basic);
    ag->addAction(sci);
    ag->addAction(graph);
    ag->addAction(dev);

    connect(basic, &QAction::triggered, this, &CalculatorWindow::changeOperationMode);
    connect(sci, &QAction::triggered, this, &CalculatorWindow::changeOperationMode);

    graph->setDisabled(true);
    dev->setDisabled(true);

    view->addSeparator();
    auto tape = view->addAction(tr("Show Paper &Tape"));
    tape->setCheckable(true);
    connect(tape, &QAction::toggled, this, &CalculatorWindow::togglePaperTape);
    tape->setShortcut(QKeySequence("Ctrl+T"));

    auto help = menu->addMenu(tr("&Help"));
    auto about = help->addAction(tr("&About Calculator"));
    connect(about, &QAction::triggered, this, &CalculatorWindow::about);
    setMenuBar(menu);

    m_tape->setUndoRedoEnabled(false);
    m_tape->setReadOnly(true);
    auto layout = new QVBoxLayout(host);
    layout->addWidget(m_tape);
    layout->addWidget(m_calc);
    layout->setContentsMargins(0,0,0,0);
    host->setLayout(layout);
    setCentralWidget(host);
    m_tape->setMaximumWidth(m_calc->layout()->minimumSize().width());
    bool stape = settings.value("ShowPaperTape", false).toBool();
    m_tape->setVisible(stape);
    tape->setChecked(stape);

    int mode = settings.value("LastMode", Calculator::Basic).toInt();
    if(mode < Calculator::Basic || mode > Calculator::Scientific)
        mode = Calculator::Basic;

    m_calc->setOperationMode((Calculator::OperationMode)mode);
    if(mode == Calculator::Basic)
        basic->setChecked(true);
    if(mode == Calculator::Scientific)
        sci->setChecked(true);
}

void CalculatorWindow::copyTape()
{

}

void CalculatorWindow::copyValue()
{

}

void CalculatorWindow::pasteValue()
{

}

void CalculatorWindow::clipboardChanged()
{
    bool canPaste = false;
    QString text = qApp->clipboard()->text();
    if(!text.isEmpty())
        text.toDouble(&canPaste);

    m_paste->setEnabled(canPaste);
}

void CalculatorWindow::togglePaperTape(bool showTape)
{
    QSettings settings;
    m_tape->setVisible(showTape);
    m_showtape = showTape;
    requestResize();
    settings.setValue("ShowPaperTape", m_showtape);
}

void CalculatorWindow::requestResize()
{
    setMaximumWidth(m_calc->layout()->minimumSize().width());
    if(!m_showtape)
    {
        layout()->setSizeConstraint(QLayout::SetFixedSize);
        setMaximumHeight(m_calc->layout()->minimumSize().height());
        resize(m_calc->layout()->minimumSize());
    }
    else
    {
        layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
        setMaximumHeight(m_calc->layout()->minimumSize().height()+1000);
        QSize size(m_calc->layout()->minimumSize());
        size.setHeight(size.height() + m_tape->height());
        resize(size);
    }
}

void CalculatorWindow::changeOperationMode()
{
    auto senderAction = qobject_cast<QAction*>(sender());
    Q_ASSERT(senderAction);

    QSettings settings;
    if(senderAction->objectName() == "Basic")
    {
        m_calc->setOperationMode(Calculator::Basic);
        settings.setValue("LastMode", Calculator::Basic);
    }

    if(senderAction->objectName() == "Scientific")
    {
        settings.setValue("LastMode", Calculator::Scientific);
        m_calc->setOperationMode(Calculator::Scientific);
    }

    m_tape->setMaximumWidth(m_calc->layout()->minimumSize().width());
}

void CalculatorWindow::saveTape()
{

}

void CalculatorWindow::printTape()
{

}

void CalculatorWindow::about()
{
    auto about = new HWAboutDialog(this);
    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon::fromTheme("accessories-calculator"));
#ifdef HW_BUILD_VER
    a.setApplicationVersion(QString("%1.%2").arg(HOLLYWOOD_OS_VERSION, QString::number(HW_BUILD_VER)));
#else
    a.setApplicationVersion(HOLLYWOOD_OS_VERSION);
#endif
    a.setApplicationName("Calculator");
    a.setOrganizationDomain(HOLLYWOOD_OS_DOMAIN);
    a.setOrganizationName(HOLLYWOOD_OS_ORGNAME);
    CalculatorWindow calc;
    calc.show();
    return a.exec();
}
