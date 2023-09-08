
#include "application.h"
#include <hollywood/aboutdialog.h>

#ifdef Q_OS_MAC
extern void disableWindowTabbing();
extern void setupServicesMenu();
extern void setWindowMenuMac(QMenu *windowMenu);
extern EditorWidgetServicesResponder* installResponder();
extern void setupMacWindowMenu();
#endif

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv),
      m_menubar(NULL),
      m_prefs(NULL),
      m_spell_check(new SpellCheck(this)),
      m_current(NULL)
{
    setApplicationName(APPLICATION_NAME);
    //setApplicationDisplayName(APPLICATION_NAME);
    setOrganizationName(ORGANIZATION_NAME);
    setOrganizationDomain(ORGANIZATION_DOMAIN);
    setApplicationVersion(APPLICATION_VERSION);
    setAttribute(Qt::AA_DontShowIconsInMenus, true);
    setupEncodings();
    rebuildMRUInMemory();

    setWindowIcon(QIcon(":/AppIcon/PNG"));

#ifdef Q_WS_WIN
    QFont font("Segoe UI");
    font.setPointSize(9);
    setFont(font);
    createJumpList();
#endif
    createDefaultSettings();
    connect(this, SIGNAL(updatePreferences()),
            m_spell_check, SLOT(updatePreferences()));
    checkSpellCheck();

#ifdef Q_WS_MAC
    setupServicesMenu();
    //setupBareMenuBar();
    setQuitOnLastWindowClosed(false);
    disableWindowTabbing();
    m_dock_menu->setAsDockMenu();
    installMacResponder();
#endif
}

SpellCheck* Application::spellChecker()
{
    if(m_spell_check)
        return m_spell_check;

    return Q_NULLPTR;
}

void Application::setupBareMenuBar()
{
    m_menubar = new QMenuBar(NULL);
    QAction *newdoc = new QAction(tr("&New"));
    newdoc->setObjectName("NewDocument");
    newdoc->setIcon(QIcon::fromTheme("document-new",
                                  QIcon(":/IconActions/NewDocument")));
    newdoc->setStatusTip(tr("Create a new document in a new tab."));
    newdoc->setToolTip(tr("New Document"));
    newdoc->setIconText(tr("New"));
    newdoc->setShortcut(QKeySequence::New);
    m_actions.insert("NewDocument", newdoc);

    connect(newdoc, SIGNAL(triggered(bool)), this, SLOT(newWindowSlot()));

    QAction *open = new QAction(tr("&Open..."));
    open->setObjectName("OpenAction");
    open->setIcon(QIcon::fromTheme("document-open",
                                  QIcon(":/IconActions/Open")));
    open->setToolTip(tr("Open Document"));
    open->setStatusTip(tr("Open a document from disk."));
    open->setIconText(tr("Open"));
    open->setShortcut(QKeySequence::Open);
    m_actions.insert("Open", open);

    QAction *quit = new QAction(tr("&Quit"));
    quit->setObjectName("QuitApp");
    quit->setToolTip(tr("Quit %1").arg(qApp->applicationDisplayName()));
    quit->setStatusTip(tr("Prompt to save all changes and quit %1.")
                       .arg(qApp->applicationDisplayName()));
    quit->setIconText(tr("Quit"));
    quit->setShortcut(QKeySequence::Quit);
    m_actions.insert("QuitApp", quit);
    connect(quit, SIGNAL(triggered(bool)), myApp, SLOT(quit()));

    QAction *prefs = new QAction(tr("&Preferences..."));
    prefs->setObjectName("Preferences");
    prefs->setToolTip(tr("Preferences"));
    prefs->setStatusTip(tr("Set application preferences."));
    prefs->setIconText(tr("Preferences"));
    m_actions.insert("Preferences", prefs);
    connect(prefs, SIGNAL(triggered(bool)), myApp, SLOT(showPreferencesDialog()));

    // window actions
    QAction *minimize = new QAction(tr("&Minimize"));
    minimize->setObjectName("Minimize");
    minimize->setToolTip(tr("Minimize Window"));
    minimize->setStatusTip(tr("Minimize this window."));
    minimize->setIconText(tr("Minimize"));
    m_actions.insert("Minimize", minimize);
    minimize->setDisabled(true);

    QAction *zoom = new QAction(tr("&Zoom"));
    zoom->setObjectName("Zoom");
    zoom->setToolTip(tr("Zoom Window"));
    zoom->setStatusTip(tr("Zoom this window to fill the screen."));
    zoom->setIconText(tr("Zoom"));
    m_actions.insert("Zoom", zoom);
    zoom->setDisabled(true);

    QAction *next_tab = new QAction(tr("Select &Next Tab"));
    next_tab->setObjectName("SelectNextTab");
    next_tab->setToolTip(tr("Next Tab"));
    next_tab->setStatusTip(tr("Select the next tab in the window."));
    next_tab->setIconText(tr("Next Tab"));
    m_actions.insert("SelectNextTab", next_tab);
    next_tab->setDisabled(true);

    QAction *prev_tab = new QAction(tr("Select &Previous Tab"));
    prev_tab->setObjectName("SelectPreviousTab");
    prev_tab->setToolTip(tr("Previous Tab"));
    prev_tab->setStatusTip(tr("Select the previous tab in the window."));
    prev_tab->setIconText(tr("Previous Tab"));
    m_actions.insert("SelectPreviousTab", prev_tab);
    prev_tab->setDisabled(true);

    QAction *help = new QAction(tr("%1 &Help").arg(qApp->applicationName()));
    help->setObjectName("Help");
    help->setToolTip(tr("Help Topics"));
    help->setStatusTip(tr("Show all help topics for the application."));
    help->setIconText(tr("Help"));
    help->setShortcut(QKeySequence::HelpContents);
    m_actions.insert("Help", help);

    QAction *webpage = new QAction(tr("&WWW Homepge"));
    webpage->setObjectName("Homepage");
    webpage->setToolTip(tr("WWW Homepage"));
    webpage->setStatusTip(
                tr("Open the %1 homepage on the world wide web in "
                   "your default browser.")
                .arg(qApp->applicationName()));
    webpage->setIconText(tr("Homepage"));
    m_actions.insert("Homepage", webpage);

    QAction *update = new QAction(tr("Check for &Updates..."));
    update->setObjectName("Preferences");
    update->setToolTip(tr("Preferences"));
    update->setStatusTip(tr("Set application preferences."));
    update->setIconText(tr("Preferences"));
    m_actions.insert("Update", update);

    QAction *about = new QAction(tr("&About %1").arg(qApp->applicationName()));
    about->setObjectName("About");
    about->setToolTip(tr("About %1").arg(qApp->applicationName()));
    about->setStatusTip(tr("Show information about the application."));
    about->setIconText(tr("About"));
    m_actions.insert("About", about);
    connect(about, SIGNAL(triggered(bool)), myApp, SLOT(aboutApplication()));

    m_menubar->setAccessibleName(tr("Menu Bar"));
    m_menubar->setAccessibleDescription(tr("%1 Main Menu Bar")
                                     .arg(qApp->applicationDisplayName()));

    QMenu *file = m_menubar->addMenu(tr("&File"));
    QMenu *view = m_menubar->addMenu(tr("&View"));
    QMenu *window = m_menubar->addMenu(tr("&Window"));
    QMenu *helpMenu = m_menubar->addMenu(tr("&Help"));

    file->setObjectName(QString("FileMenu"));
    view->setObjectName(QString("ViewMenu"));
    help->setObjectName(QString("HelpMenu"));

    file->addAction(findActionByName("NewDocument"));
    file->addAction(findActionByName("Open"));
    file->addSeparator();
    file->addAction(findActionByName("QuitApp"));

    view->addAction(findActionByName("Preferences"));

#ifdef Q_WS_MAC
    minimize->setShortcut(QKeySequence("Ctrl+M"));
    next_tab->setShortcut(QKeySequence("META+Tab"));
    prev_tab->setShortcut(QKeySequence("META+Shift+Tab"));
    setWindowMenuMac(window);
    window->addAction(findActionByName("Minimize"));
    window->addAction(findActionByName("Zoom"));
    window->addSeparator();
#endif
    window->addAction(findActionByName("SelectNextTab"));
    window->addAction(findActionByName("SelectPreviousTab"));
    window->addSeparator();

    helpMenu->addAction(findActionByName("Help"));
    helpMenu->addAction(findActionByName("Homepage"));
    helpMenu->addAction(findActionByName("Update"));
    helpMenu->addAction(findActionByName("About"));
}

EditorWindow* Application::newEditorWindow(QString filename)
{
    EditorWindow* w = new EditorWindow();

    if(!filename.isEmpty())
        w->tabWidget()->openFile(filename);
    else
        w->newDocument();

    w->show();
    m_windows.append(w);
    connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(windowDestroyed(QObject*)));
    return w;
}

void Application::newWindowSlot()
{
    newEditorWindow();
}

QAction* Application::findActionByName(QString name)
{
    return m_actions.value(name, nullptr);
}

QStringList Application::dialogNameFilters()
{
    QStringList filters;
    filters << "Text files (*.txt)"
            << "Log files (*.log)"
            << "HTML Documents (*.htm *.html)"
            << "CSS Stylesheets (*.css)"
            << "JavaScript Files (*.js)"
            << "C\\C++ Source Code (*.c *.cc *.cpp *.cxx)"
            << "Header Files (*.h *.hpp *.hxx)"
            << "Objective C\\C++ Source Code (*.m *.mm)"
            << "Batch Files (*.bat *.cmd)"
            << "Shell Scripts (*.sh)"
            << "Perl Scripts (*.pl)"
            << "PHP Scripts (*.php)"
            << "Python Scripts (*.py)"
            << "Any files (*)";

    return filters;
}

void Application::setupEncodings()
{
    if(!m_encodingCategories.isEmpty() &&
       !m_encodingNames.isEmpty())
            return;

    QStringList codecs;
    QList<int> mibs = QTextCodec::availableMibs();
    foreach(int mib, mibs)
    {
        //QTextCodec *c = QTextCodec::codecForMib(mib);
        QString codec = QString(QTextCodec::codecForMib(mib)->name());
        codecs.append(codec);
    }
    codecs.sort();


    foreach(QString codec, codecs)
    {
        QString cc = codec.toLower();

        if(cc == "big5")
        {
            m_encodingNames.insert(codec, tr("Traditional Chinese (Big5)"));
            m_encodingCategories.insert(codec, tr("Chinese"));
            continue;
        }

        if(cc == "big5-hkscs")
        {
            m_encodingNames.insert(codec, tr("Traditional Chinese (HK Supplementary Character Set) (Big5-HKSCS)"));
            m_encodingCategories.insert(codec, tr("Chinese"));
            continue;
        }

        if(cc == "euc-jp")
        {
            m_encodingNames.insert(codec, tr("Extended UNIX Code (EUC-JP)"));
            m_encodingCategories.insert(codec, tr("Japanese"));
            continue;
        }

        if(cc == "euc-kr")
        {
            m_encodingNames.insert(codec, tr("Extended UNIX Code (EUC-KR)"));
            m_encodingCategories.insert(codec, tr("Korean"));
            continue;
        }

        if(cc == "gb18030")
        {
            m_encodingNames.insert(codec, tr("National Standard (PRC, 2001) (GB18030)"));
            m_encodingCategories.insert(codec, tr("Chinese"));
            continue;
        }

        if(cc == "gb")
        {
            m_encodingNames.insert(codec, tr("National Standard (PRC, 1980) (GB)"));
            m_encodingCategories.insert(codec, tr("Chinese"));
            continue;
        }

        if(cc == "gbk")
        {
            m_encodingNames.insert(codec, tr("National Standard Extension (PRC, 1993) (GBK)"));
            m_encodingCategories.insert(codec, tr("Chinese"));
            continue;
        }

        if(cc == "koi8-r")
        {
            m_encodingNames.insert(codec, tr("Russian (KOI8-R)"));
            m_encodingCategories.insert(codec, tr("Cyrillic"));
            continue;
        }

        if(cc == "koi8-u")
        {
            m_encodingNames.insert(codec, tr("Ukrainian (KOI8-U)"));
            m_encodingCategories.insert(codec, tr("Cyrillic"));
            continue;
        }

        if(cc == "shift_jis")
        {
            m_encodingNames.insert(codec, tr("Shift Japanese Industrial Standards (Shift_JIS)"));
            m_encodingCategories.insert(codec, tr("Japanese"));
            continue;
        }

        if(cc == "tis-620")
        {
            m_encodingNames.insert(codec, tr("Thai (TIS-620)"));
            m_encodingCategories.insert(codec, tr("Thai"));
            continue;
        }

        if(cc == "tscii")
        {
            m_encodingNames.insert(codec, tr("Tamil (TSCII)"));
            m_encodingCategories.insert(codec, tr("Tamil"));
            continue;
        }

        if(cc == "winsami2")
        {
            m_encodingNames.insert(codec, tr("Northern Sami (winsami2)"));
            m_encodingCategories.insert(codec, tr("Nordic"));
            continue;
        }

        if(cc == "hp-roman8")
        {
            m_encodingNames.insert(codec, tr("Latin (hp-roman8)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }

        if(cc == "iscii-bng")
        {
            m_encodingNames.insert(codec, tr("Bengali (iscii-bng)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-dev")
        {
            m_encodingNames.insert(codec, tr("Devanagri (iscii-dev)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-gjr")
        {
            m_encodingNames.insert(codec, tr("Gujarati (iscii-gjr)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-knd")
        {
            m_encodingNames.insert(codec, tr("Kannada (iscii-knd)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-mlm")
        {
            m_encodingNames.insert(codec, tr("Malayalam (iscii-mlm)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-ori")
        {
            m_encodingNames.insert(codec, tr("Oriya (iscii-ori)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-pnj")
        {
            m_encodingNames.insert(codec, tr("Punjabi (iscii-pnj)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-tlg")
        {
            m_encodingNames.insert(codec, tr("Telgu (iscii-tlg)"));
            m_encodingCategories.insert(codec, tr("Indian"));
            continue;
        }

        if(cc == "iscii-tml")
        {
            m_encodingNames.insert(codec, tr("Tamil (iscii-tml)"));
            m_encodingCategories.insert(codec, tr("Tamil"));
            continue;
        }

        if(cc == "macintosh")
        {
            m_encodingNames.insert(codec, tr("Mac OS Roman (macintosh)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }

        if(cc == "utf-8")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-8)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }

        if(cc == "utf-16")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-16)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }
        if(cc == "utf-16be")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-16, Big Endian)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }

        if(cc == "utf-16le")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-16, Little Endian)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }

        if(cc == "utf-32")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-32)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }
        if(cc == "utf-32be")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-32, Big Endian)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }
        if(cc == "utf-32le")
        {
            m_encodingNames.insert(codec, tr("Unicode (UTF-32, Little Endian)"));
            m_encodingCategories.insert(codec, tr("Unicode"));
            continue;
        }

        if(cc == "windows-1250")
        {
            m_encodingNames.insert(codec, tr("Central/Eastern European (windows-1250)"));
            m_encodingCategories.insert(codec, tr("European"));
            continue;
        }

        if(cc == "windows-1251")
        {
            m_encodingNames.insert(codec, tr("Cyrillic (windows-1251)"));
            m_encodingCategories.insert(codec, tr("Cyrillic"));
            continue;
        }

        if(cc == "windows-1252")
        {
            m_encodingNames.insert(codec, tr("Western (windows-1252)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }
        if(cc == "windows-1253")
        {
            m_encodingNames.insert(codec, tr("Greek (windows-1253)"));
            m_encodingCategories.insert(codec, tr("Greek"));
            continue;
        }
        if(cc == "windows-1254")
        {
            m_encodingNames.insert(codec, tr("Turkish (windows-1254)"));
            m_encodingCategories.insert(codec, tr("Turkish"));
            continue;
        }
        if(cc == "windows-1255")
        {
            m_encodingNames.insert(codec, tr("Hebrew (windows-1255)"));
            m_encodingCategories.insert(codec, tr("Hebrew"));
            continue;
        }
        if(cc == "windows-1256")
        {
            m_encodingNames.insert(codec, tr("Arabic (windows-1256)"));
            m_encodingCategories.insert(codec, tr("Arabic"));
            continue;
        }
        if(cc == "windows-1257")
        {
            m_encodingNames.insert(codec, tr("Baltic (windows-1257)"));
            m_encodingCategories.insert(codec, tr("Baltic"));
            continue;
        }
        if(cc == "windows-1258")
        {
            m_encodingNames.insert(codec, tr("Vietnamese (windows-1258)"));
            m_encodingCategories.insert(codec, tr("Vietnamese"));
            continue;
        }
        if(cc == "windows-949")
        {
            m_encodingNames.insert(codec, tr("Unified Hangul Code (windows-949)"));
            m_encodingCategories.insert(codec, tr("Korean"));
            continue;
        }

        if(cc == "iso-8859-1")
        {
            m_encodingNames.insert(codec, tr("US-ASCII (iso-8859-1)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }

        if(cc == "iso-8859-2")
        {
            m_encodingNames.insert(codec, tr("European (iso-8859-2)"));
            m_encodingCategories.insert(codec, tr("European"));
            continue;
        }

        if(cc == "iso-8859-3")
        {
            m_encodingNames.insert(codec, tr("Turkish, Esperanto (iso-8859-3)"));
            m_encodingCategories.insert(codec, tr("Turkish"));
            continue;
        }
        if(cc == "iso-8859-4")
        {
            m_encodingNames.insert(codec, tr("Baltic (iso-8859-4)"));
            m_encodingCategories.insert(codec, tr("Baltic"));
            continue;
        }

        if(cc == "iso-8859-5")
        {
            m_encodingNames.insert(codec, tr("Cyrillic (iso-8859-5)"));
            m_encodingCategories.insert(codec, tr("Cyrillic"));
            continue;
        }

        if(cc == "iso-8859-6")
        {
            m_encodingNames.insert(codec, tr("Arabic (iso-8859-6)"));
            m_encodingCategories.insert(codec, tr("Arabic"));
            continue;
        }

        if(cc == "iso-8859-7")
        {
            m_encodingNames.insert(codec, tr("Greek (iso-8859-7)"));
            m_encodingCategories.insert(codec, tr("Greek"));
            continue;
        }

        if(cc == "iso-8859-8")
        {
            m_encodingNames.insert(codec, tr("Hebrew (iso-8859-8)"));
            m_encodingCategories.insert(codec, tr("Hebrew"));
            continue;
        }

        if(cc == "iso-8859-9")
        {
            m_encodingNames.insert(codec, tr("Turkish (iso-8859-9)"));
            m_encodingCategories.insert(codec, tr("Turkish"));
            continue;
        }

        if(cc == "iso-8859-10")
        {
            m_encodingNames.insert(codec, tr("Nordic (iso-8859-10)"));
            m_encodingCategories.insert(codec, tr("Nordic"));
            continue;
        }
        if(cc == "iso-8859-13")
        {
            m_encodingNames.insert(codec, tr("Baltic, Polish (iso-8859-13)"));
            m_encodingCategories.insert(codec, tr("Baltic"));
            continue;
        }
        if(cc == "iso-8859-14")
        {
            m_encodingNames.insert(codec, tr("Celtic (iso-8859-14)"));
            m_encodingCategories.insert(codec, tr("Celtic"));
            continue;
        }
        if(cc == "iso-8859-15")
        {
            m_encodingNames.insert(codec, tr("Western (iso-8859-15)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }
        if(cc == "iso-8859-16")
        {
            m_encodingNames.insert(codec, tr("SE European (iso-8859-16)"));
            m_encodingCategories.insert(codec, tr("European"));
            continue;
        }

        if(cc == "ibm850")
        {
            m_encodingNames.insert(codec, tr("CP 850 (ibm850)"));
            m_encodingCategories.insert(codec, tr("Western"));
            continue;
        }
        if(cc == "ibm866")
        {
            m_encodingNames.insert(codec, tr("CP 866 (ibm866)"));
            m_encodingCategories.insert(codec, tr("Cyrillic"));
            continue;
        }
        if(cc == "ibm874")
        {
            m_encodingNames.insert(codec, tr("CP 874 (ibm874)"));
            m_encodingCategories.insert(codec, tr("Thai"));
            continue;
        }
    }
}

QStringList Application::encodingCategories()
{
    auto setValues = m_encodingCategories.values();
    setValues.sort();

    return setValues;
}

QString Application::categoryForEncoding(QString encoding)
{
    QString enc =
            m_encodingCategories.value(encoding);

    if(enc.isEmpty())
        return tr("Unknown");

    return enc;
}

QMap<QString, QString> Application::encodings()
{
    return m_encodingNames;
}

void Application::requestPreferencesUpdate()
{
    emit updatePreferences();
}

void Application::createDefaultSettings()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    if(settings.value("DefaultsConfigured", false).toBool())
        return;

    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");

    // TODO: spelling
    settings.setValue("ForegroundColor", QColor(Qt::black).name());
    settings.setValue("BackgroundColor", QColor(Qt::white).name());
    settings.endGroup();
    settings.endGroup();

    settings.setValue("DefaultsConfigured", true);
}

void Application::aboutApplication(QWidget *parent)
{
#ifdef BUILD_HOLLYWOOD
    HWAboutDialog *about;
    if(parent != nullptr)
    {
        if(parent->objectName().startsWith("FMWindow"))
            about = new HWAboutDialog(parent);
        else
            about = new HWAboutDialog(0);
    }
    else
        about = new HWAboutDialog(0);

    about->setAppDescription(tr("An editor made for you!"));
    about->setAppCopyright("Copyright © 2019-2022\nOriginull Software");
    connect(about, SIGNAL(finished(int)), about, SLOT(deleteLater()));
    about->show();
#else
    QMessageBox *m = new QMessageBox(parent);
    QString compilier = tr("Unknown");
#ifdef Q_WS_WIN
#ifdef _MSC_VER

#if _MSC_VER > 1910 && _MSC_VER < 1920
    compilier = tr("Visual C++ 2017");
#elif _MSC_VER == 1900
    compilier = tr("Visual C++ 2015");
#elif _MSC_VER == 1800
    compilier = tr("Visual C++ 2013");
#elif _MSC_VER == 1700
    compilier = tr("Visual C++ 2012");
#elif _MSC_VER == 1600
    compilier = tr("Visual C++ 2010");
#else
    compilier = tr("Visual C++");
#endif
#elif __GNUC___
    compilier = tr("GCC %1.%2.%3")
            .arg(__GNUC__)
            .arg(__GNUC_MINOR__)
            .arg(__GNUC_PATCHLEVEL__);
#elif __clang__
    compilier = tr("Clang %1.%2.%3")
            .arg(__clang__)
            .arg(__clang_minor__)
            .arg(__clang_patchlevel__);
#endif
#endif

#ifdef Q_WS_MAC
#ifdef __clang__
    compilier = tr("Apple Clang %1.%2.%3")
            .arg(__clang__)
            .arg(__clang_minor__)
            .arg(__clang_patchlevel__);
#endif
#endif
    m->setText(myApp->applicationName());
    m->setIconPixmap(myApp->windowIcon().pixmap(48, 48));
    m->setInformativeText(tr("Version: %1 \n\nCopyright © %2 %3.\nAll Rights Reserved.")
                          .arg(myApp->applicationVersion())
                         .arg(COPYRIGHT_YEARS).arg(myApp->organizationName()));

#ifndef Q_WS_MAC
    m->setWindowTitle(tr("About %1").arg(myApp->applicationName()));
#endif

#ifdef Q_WS_WIN
    m->setText(QString("About %1").arg(myApp->applicationName()));
    QLabel *w = m->findChild<QLabel*>("qt_msgbox_label",
                               Qt::FindDirectChildrenOnly);
    QFont f("Seoge UI", 12);
    QPalette p = w->palette();
    p.setColor(QPalette::Foreground, QColor(0, 51, 153));
    w->setFont(f);
    w->setPalette(p);
#endif

    m->exec();

    m->deleteLater();
#endif
}

void Application::openFile(QString fileName)
{
    // This function exists to locate the file
    // in any current open windows/tabs

    foreach(EditorWindow *w, m_windows)
    {
        foreach(Document *d, w->documents())
        {
            if(!d)
                continue;

            if(d->fileName() == fileName)
                return w->focusDocument(d);
        }
    }

    // Not currently open, so let's open
    // the document in something new...

    if(m_windows.empty())
        newEditorWindow(fileName);
    else
    {
        // open up in a new tab
        currentWindow()->tabWidget()->openFile(fileName);
    }
}

void Application::showPreferencesDialog(QWidget *parent)
{
    m_prefs = new PreferencesDialog(parent);
    m_prefs->setParent(parent);
    connect(m_prefs, SIGNAL(finished(int)), m_prefs, SLOT(deleteLater()));

    if(m_prefs->exec())
    {

    }

    m_prefs->deleteLater();

    m_prefs = NULL;
}

EditorWindow* Application::currentWindow()
{
    return m_current;
}

void Application::windowDestroyed(QObject *w)
{
    EditorWindow *w2 = qobject_cast<EditorWindow*>(w);
    if(m_windows.contains(w2))
        m_windows.removeOne(w2);
}

void Application::setCurrentWindow(EditorWindow *w)
{
    m_current = w;
}

void Application::parseCommandLine()
{
    QCommandLineParser p;
    p.setApplicationDescription(applicationName());
    p.addVersionOption();
    p.addHelpOption();

    QStringList args = arguments();
    p.process(arguments());

    if(!p.positionalArguments().isEmpty())
    {
        foreach(QString file, p.positionalArguments())
        {
            QFile f(file);
            if(!f.exists())
            {
                QMessageBox msg(QMessageBox::Question, tr("Create new file?"),
                                tr("The file, %1, does not exist. Do you want to create it?"),
                                QMessageBox::Yes|QMessageBox::No);
                int result = msg.exec();

                if(result == QMessageBox::Yes)
                {
                    if(f.open(QIODevice::WriteOnly))
                    {
                        f.write("");
                        f.close();
                        openFile(file);
                    }
                }
            }
            else
                openFile(file);
        }
    }
    else
        newEditorWindow();
}

bool Application::checkSpellCheck(bool silent)
{
#ifdef Q_WS_WIN
    QString appPath = QString("%1/Dictionaries").arg(QCoreApplication::applicationDirPath());
#endif

#ifdef Q_WS_MAC
    CFURLRef url = (CFURLRef)CFAutorelease((CFURLRef)CFBundleCopyBundleURL(CFBundleGetMainBundle()));
    QString appPath = QString("%1/Contents/Resources/Dictionaries").arg(QUrl::fromCFURL(url).path());
#endif

#ifdef __linux__
    // This works for Debian/Ubuntu
    QString appPath = QString("/usr/share/hunspell");
#endif

    QStringList dicts =
            SpellCheck::availableDictionaries();

    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    QString prefDict = settings.value("SpellingDictionary", LUVEDIT_DEFAULT_DICT).toString();

#ifdef __linux__
    bool hasAff = QFile::exists(QString("/usr/share/hunspell/%1.aff").arg(prefDict));
    bool hasDic = QFile::exists(QString("/usr/share/hunspell/%1.dic").arg(prefDict));
#else
    bool hasAff = QFile::exists(QString("%1/%2/%3").arg(appPath).arg(prefDict).arg("index.aff"));
    bool hasDic = QFile::exists(QString("%1/%2/%3").arg(appPath).arg(prefDict).arg("index.dic"));
#endif

    if(!hasAff && !hasDic)
    {
        if(silent)
            return false;

        QMessageBox box;
        box.setWindowModality(Qt::ApplicationModal);
        box.setWindowTitle(tr("Missing Dictionaries"));
    #ifdef Q_WS_WIN
        QLabel *w = box.findChild<QLabel*>("qt_msgbox_label",
                                   Qt::FindDirectChildrenOnly);
        QFont f("Seoge UI", 12);
        QPalette p = w->palette();
        p.setColor(QPalette::Foreground, QColor(0, 51, 153));
        w->setFont(f);
        w->setPalette(p);
        box.setIcon(QMessageBox::Warning);
    #endif
        box.setText(tr("Missing Spelling Dictionary"));
        box.setInformativeText(tr("The spelling dictionary selected, %1, is unavailable. Re-install %2 to correct this problem.").arg(prefDict)
                               .arg(applicationDisplayName()));
        box.setStandardButtons(QMessageBox::Ok);
        box.exec();

        return false;
    }

    return true;
}

#ifdef Q_WS_MAC
void Application::installMacResponder()
{
    m_services_responder = installResponder();
}
#endif

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    a.parseCommandLine();

    return a.exec();
}

void Application::addToMRUFiles(const QString &fileName)
{
    QSettings settings("originull", "startext");
    settings.beginGroup("RecentFiles");

    // just looking for uniqueness, not perfection
    QString fileHash =
            QCryptographicHash::hash(fileName.toUtf8(),
                                     QCryptographicHash::Sha1).toHex();

#ifdef Q_WS_WIN
    QString newName = QDir::toNativeSeparators(fileName);
#else
    QString newName = fileName;
#endif

    if(!settings.childGroups().contains(fileHash))
    {
        // we don't have this file, so make a new entry!
        settings.beginGroup(fileHash);
        settings.setValue("FileName", newName);
        settings.setValue("LastAccess",
                          QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
        settings.endGroup();
    }
    else
    {
        // just bump the access time
        settings.beginGroup(fileHash);
        settings.setValue("LastAccess",
                          QDateTime::currentDateTimeUtc().toSecsSinceEpoch());
    }

    rebuildMRUInMemory();
}

void Application::cleanMRUList()
{
    // TODO: I think Mac has a system level definition for
    // MaxMRUEntries
    QMap<time_t, QString> files;
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    uint max_mru = settings.value("MaxMRUEntries", 10).toUInt();
    if(max_mru > MAX_MRU_LEN)
        max_mru = MAX_MRU_LEN;

    settings.endGroup();

    settings.beginGroup("RecentFiles");

    foreach(QString group, settings.childGroups())
    {
        settings.beginGroup(group);
        time_t last_access = settings.value("LastAccess").toUInt();
        files.insert(last_access, group);
        settings.endGroup();
    }

    QMap<time_t, QString> desired;

    QMapIterator<time_t, QString> i(files);
    i.toBack();
    uint proc = 0;
    while(i.hasPrevious())
    {
        if(proc == max_mru)
            break;

        i.previous();
        desired.insert(i.key(), i.value());
        ++proc;
    }

    foreach(QString group, settings.childGroups())
    {
        if(!desired.values().contains(group))
            settings.remove(group);
    }
}

void Application::rebuildMRUInMemory()
{
    cleanMRUList();
    m_mru_files.clear();
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    uint max_mru = settings.value("MaxMRUEntries", 10).toUInt();
    if(max_mru > MAX_MRU_LEN)
        max_mru = MAX_MRU_LEN;

    settings.endGroup();
    settings.beginGroup("RecentFiles");

    foreach(QString group, settings.childGroups())
    {
        settings.beginGroup(group);
        time_t last_access = settings.value("LastAccess").toUInt();
        QString file_name = settings.value("FileName").toString();
        m_mru_files.insert(last_access, file_name);
        settings.endGroup();
    }
}

MRUFiles Application::mruList() const
{
    return m_mru_files;
}

#ifdef Q_WS_WIN
QString Application::findDefaultBrowserWindows()
{
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\shell\\Associations\\UrlAssociations\\http\\UserChoice",
                       QSettings::NativeFormat);

    QString progid = settings.value("ProgId").toString();
    QSettings rootSettings(QString("HKEY_CLASSES_ROOT\\%1\\shell\\open\\command")
                           .arg(progid),
                       QSettings::NativeFormat);

    return rootSettings.value("Default").toString();
}

// TODO: move me out!!!
QImage luv_imageFromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = w * h * 4;

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    if (image.isNull())
        return image;

    // Get bitmap bits
    QScopedArrayPointer<uchar> data(new uchar [bmi.bmiHeader.biSizeImage]);
    if (!GetDIBits(hdc, bitmap, 0, h, data.data(), &bmi, DIB_RGB_COLORS)) {
        qErrnoWarning("%s: failed to get bitmap bits", __FUNCTION__);
        return QImage();
    }
    // Create image and copy data into image.
    for (int y = 0; y < h; ++y) {
        void *dest = (void *) image.scanLine(y);
        void *src = data.data() + y * image.bytesPerLine();
        memcpy(dest, src, image.bytesPerLine());
    }
    return image;
}

QPixmap luv_pixmapFromWinHICON(HICON icon)
{
    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    ICONINFO iconinfo;
    const bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
    if (!result) {
        qErrnoWarning("QPixmap::fromWinHICON(), failed to GetIconInfo()");
        return QPixmap();
    }

    const int w = iconinfo.xHotspot * 2;
    const int h = iconinfo.yHotspot * 2;

    BITMAPINFOHEADER bitmapInfo;
    bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biWidth       = w;
    bitmapInfo.biHeight      = h;
    bitmapInfo.biPlanes      = 1;
    bitmapInfo.biBitCount    = 32;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biSizeImage   = 0;
    bitmapInfo.biXPelsPerMeter = 0;
    bitmapInfo.biYPelsPerMeter = 0;
    bitmapInfo.biClrUsed       = 0;
    bitmapInfo.biClrImportant  = 0;
    DWORD* bits;

    HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
    DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);
    QImage image = luv_imageFromWinHBITMAP(hdc, winBitmap, w, h);

    for (int y = 0 ; y < h && !foundAlpha ; y++) {
        const QRgb *scanLine= reinterpret_cast<const QRgb *>(image.scanLine(y));
        for (int x = 0; x < w ; x++) {
            if (qAlpha(scanLine[x]) != 0) {
                foundAlpha = true;
                break;
            }
        }
    }
    if (!foundAlpha) {
        //If no alpha was found, we use the mask to set alpha values
        DrawIconEx( hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
        const QImage mask = luv_imageFromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0 ; y < h ; y++){
            QRgb *scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
            const QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<const QRgb *>(mask.scanLine(y));
            for (int x = 0; x < w ; x++){
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0; //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
            }
        }
    }
    //dispose resources created by iconinfo call
    DeleteObject(iconinfo.hbmMask);
    DeleteObject(iconinfo.hbmColor);

    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(image);
}
#endif
