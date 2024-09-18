/*
 * main.cc - Implementation of the UI.Perform interface
 * horizon-run-qt5, the Qt 5 executor user interface for
 * Project Horizon
 *
 * Copyright (c) 2019 Adélie Linux and contributors.  All rights reserved.
 * This code is licensed under the AGPL 3.0 license, as noted in the
 * LICENSE-code file in the root directory of this repository.
 *
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLibraryInfo>
#include <QTranslator>
#include <QMenuBar>

#include <hollywood/aboutdialog.h>

#include "executorwizard.hh"
#include "main.hh"

HorizonRunnerApplication::HorizonRunnerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_menubar(new QMenuBar(nullptr))
    , m_wizard(new ExecutorWizard(nullptr, false))
{
    setOrganizationName("Originull Software");
    setApplicationName("Setup Assistant");
    setApplicationVersion(VERSTR);

    setWindowIcon(QIcon::fromTheme("system-software-install"));

    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    QTranslator *translator = new QTranslator(this);
    if(translator->load(translatorFileName,
                         QLibraryInfo::path(
                             QLibraryInfo::TranslationsPath
                             ))) {
        installTranslator(translator);
    }

    auto file = m_menubar->addMenu(tr("&File"));
    auto back = file->addAction(tr("Go &Back"));
    back->setShortcut(QKeySequence(QKeySequence::Back));
    connect(back, &QAction::triggered, this, [this] () {
        m_wizard->back();
    });
    auto fwd = file->addAction(tr("Co&ntinue"));
    fwd->setShortcut(QKeySequence(QKeySequence::Forward));
    connect(fwd, &QAction::triggered, this, [this] () {
        m_wizard->next();
    });
    file->addSeparator();
    auto save = file->addAction(tr("&Save Installer Log..."));
    save->setShortcut(QKeySequence(QKeySequence::SaveAs));
    save->setEnabled(false);
    auto quit = file->addAction(tr("&Quit"));
    quit->setShortcut(QKeySequence(QKeySequence::Quit));
    connect(quit, &QAction::triggered, this, &HorizonRunnerApplication::quit);
    auto edit = m_menubar->addMenu(tr("&Edit"));
    auto undo = edit->addAction(tr("&Undo"));
    undo->setShortcut(QKeySequence(QKeySequence::Undo));
    undo->setEnabled(false);
    auto redo = edit->addAction(tr("&Redo"));
    redo->setShortcut(QKeySequence(QKeySequence::Redo));
    redo->setEnabled(false);
    edit->addSeparator();
    auto cut = edit->addAction(tr("&Cut"));
    cut->setShortcut(QKeySequence(QKeySequence::Cut));
    cut->setEnabled(false);
    auto copy = edit->addAction(tr("C&opy"));
    copy->setShortcut(QKeySequence(QKeySequence::Copy));
    copy->setEnabled(false);
    auto paste = edit->addAction(tr("Paste"));
    paste->setShortcut(QKeySequence(QKeySequence::Paste));
    paste->setEnabled(false);

    auto window = m_menubar->addMenu(tr("&Window"));
    auto sa_wnd = window->addAction(tr("Setup Assistant"));
    sa_wnd->setCheckable(true);
    sa_wnd->setChecked(true);
    auto log = window->addAction(tr("Installer &Log"));
    log->setEnabled(false);
    auto help = m_menubar->addMenu(tr("&Help"));
    auto gethelp = help->addAction(tr("Setup Assistant &Help"));
    gethelp->setShortcut(QKeySequence(QKeySequence::HelpContents));
    connect(gethelp, &QAction::triggered, this, [this] () {
        m_wizard->helpRequested();
    });

    auto about = help->addAction(tr("&About Setup Assistant"));
    connect(about, &QAction::triggered, this, &HorizonRunnerApplication::about);
    m_menubar->setVisible(false);
}

void HorizonRunnerApplication::showWizard()
{
    m_wizard->show();
}

void HorizonRunnerApplication::about()
{
    if(m_about != nullptr)
        return;

    m_about = new HWAboutDialog(nullptr);
    m_about->setAppDescription(tr("Based on Project Horizon\n"
                                  "(C) 2019-2022\nAdélie Linux and contributors"));
    connect(m_about, &HWAboutDialog::finished, [this](int r) {
        Q_UNUSED(r);
        m_about->deleteLater();
        m_about = nullptr;
    });
    m_about->show();
}

int main(int argc, char *argv[]) {
    HorizonRunnerApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(app.tr("Executes a HorizonScript with graphical progress indication."));
    QCommandLineOption help = parser.addHelpOption();
    QCommandLineOption version = parser.addVersionOption();

    if(!parser.parse(app.arguments())) {
        parser.showHelp(1);
        return 1;
    }

    if(parser.isSet(help)) {
        parser.showHelp();
        return 0;
    }

    if(parser.isSet(version)) {
        parser.showVersion();
        return 0;
    }

    app.showWizard();
    return app.exec();
}
