#include "editorwindow.h"

#ifdef Q_WS_MAC
extern void setQMainWindowTitlebarOff(QMainWindow *m);
extern void setTabBarAccessory(TabWidget *t);
extern void removeTabBarAccessory(TabWidget *t);
extern void setWindowMenuMac(QMenu *windowMenu);
#endif

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent),
      m_menubar(new QMenuBar),
#ifdef Q_WS_MAC
      m_toolbar(new MacToolBar("org.originull.startext.toolbar")),
#else
      m_toolbar(new QToolBar),
      m_toolbar_stretcher(new QWidget(this)),
      m_searchbox(new QLineEdit(this)),
#endif
      m_status(new QStatusBar),
      m_tabwidget(new TabWidget(this)),
      m_col_ln(new QLabel(this)),
      m_encoding(new QLabel(this)),
      m_lineendings(new QLabel(this)),
      m_language(new QLabel(this)),
      m_active_document(Q_NULLPTR)
{
    // Always create actions first, before the toolbar, menubar
    // anything that consumes actions
    createActions();
    setAccessibleName(tr("%1 Application Window").arg(qApp->applicationName()));
    setAccessibleDescription(tr("The main %1 application window")
                             .arg(qApp->applicationDisplayName()));

    createMainMenu();
    createToolBar();
    createStatusBar();

    QClipboard *cb = qApp->clipboard();
    connect(cb, SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    connect(m_tabwidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabIndexChanged(int)));
    connect(m_tabwidget, SIGNAL(currentBufferModificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));

    connect(m_tabwidget->tabBar(), SIGNAL(tabCloseRequested(int)),
            this, SLOT(tabCloseRequested(int)));


    setCentralWidget(m_tabwidget);
#ifdef Q_WS_MAC
    // mac only
    connect(windowHandle(), SIGNAL(windowStateChanged(Qt::WindowState)),
            this, SLOT(windowStateChanged(Qt::WindowState)));
#endif

    connect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));
    updateLineNumberAction();
    updateWhitespaceAction();
    myApp->setCurrentWindow(this);
    restorePosition();
}

EditorWindow::~EditorWindow()
{
    disconnect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));
    savePosition();
}

void EditorWindow::restorePosition()
{
    QSettings settings("originull", "persistence");
    settings.beginGroup("StarText");

    if(settings.contains("MainWindowPos"))
        restoreGeometry(settings.value("MainWindowPos").toByteArray());

    settings.endGroup();
}

void EditorWindow::savePosition()
{
    QSettings settings("originull", "persistence");
    settings.beginGroup("StarText");

    settings.setValue("MainWindowPos", saveGeometry());
    settings.endGroup();
}


QAction* EditorWindow::findActionByName(QString name)
{
    return m_actions.value(name, nullptr);
}

void EditorWindow::createActions()
{
    QAction *newdoc = new QAction(tr("&New"));
    newdoc->setObjectName("NewDocument");
    newdoc->setIcon(QIcon::fromTheme("document-new",
                                  QIcon(":/IconActions/NewDocument")));
    newdoc->setStatusTip(tr("Create a new document in a new tab."));
    newdoc->setToolTip(tr("New Document"));
    newdoc->setIconText(tr("New"));
    newdoc->setShortcut(QKeySequence::New);
    m_actions.insert("NewDocument", newdoc);

    connect(newdoc, SIGNAL(triggered(bool)), this, SLOT(newDocument()));

    QAction *open = new QAction(tr("&Open..."));
    open->setObjectName("OpenAction");
    open->setIcon(QIcon::fromTheme("document-open",
                                  QIcon(":/IconActions/Open")));
    open->setToolTip(tr("Open Document"));
    open->setStatusTip(tr("Open a document from disk."));
    open->setIconText(tr("Open"));
    open->setShortcut(QKeySequence::Open);
    m_actions.insert("Open", open);
    connect(open, SIGNAL(triggered(bool)), this, SLOT(showOpenFileDialog()));

    QAction *reveal = new QAction(tr("&Reveal in File Manager"));
    reveal->setObjectName("RevealFile");
    reveal->setToolTip(tr("Reveal File"));
    reveal->setStatusTip(
               tr("Reveal the file in the system File Manager."));
    reveal->setIconText(tr("Reveal"));
#ifdef Q_WS_MAC
    reveal->setText(tr("&Reveal in Finder"));
    reveal->setToolTip(tr("Reveal in Finder"));
    reveal->setStatusTip(
               tr("Reveal the file in the Macintosh Finder."));
#endif
#ifdef Q_WS_WIN
    reveal->setText(tr("&Reveal in File Explorer"));
    reveal->setToolTip(tr("Reveal in File Explorer"));
    reveal->setStatusTip(
               tr("Reveal the file in the Windows File Explorer."));
#endif
    reveal->setShortcut(QKeySequence("Ctrl+Shift+R"));
    m_actions.insert("RevealFile", reveal);
    connect(reveal, SIGNAL(triggered(bool)),
            this, SLOT(showInFileManager()));

    QAction *save = new QAction(tr("&Save"));
    save->setObjectName("SaveAction");
    save->setIcon(QIcon::fromTheme("document-save",
                                  QIcon(":/IconActions/Save")));
    save->setToolTip(tr("Save Document"));
    save->setStatusTip(tr("Save all changes to the document to disk."));
    save->setIconText(tr("Save"));
    save->setShortcut(QKeySequence::Save);
    m_actions.insert("Save", save);
    connect(save, SIGNAL(triggered(bool)), this, SLOT(saveFile()));

    QAction *saveas = new QAction(tr("Save &As"));
    saveas->setObjectName("SaveAsAction");
    saveas->setIcon(QIcon::fromTheme("document-save-as",
                                     QIcon(":/IconActions/SaveAs")));
    saveas->setToolTip(tr("Save Document As"));
    saveas->setStatusTip(
               tr("Save this document to a different location on disk."));
    saveas->setIconText(tr("Save As"));
    saveas->setShortcut(QKeySequence::SaveAs);
    m_actions.insert("SaveAs", saveas);
    connect(saveas, SIGNAL(triggered(bool)), this, SLOT(saveFileAs()));

    QAction *saveall = new QAction(tr("Sa&ve All"));
    saveall->setObjectName("SaveAsAction");
    saveall->setToolTip(tr("Save All Documents"));
    saveall->setStatusTip(
               tr("Save changes to all documents in this window."));
    saveall->setIconText(tr("Save All"));
    saveall->setShortcut(QKeySequence("Ctrl+Alt+S"));
    m_actions.insert("SaveAll", saveall);
    connect(saveall, SIGNAL(triggered(bool)), this, SLOT(unimplemented()));

    QAction *closeTab = new QAction(tr("&Close"));
    closeTab->setObjectName("CloseDocument");
    closeTab->setToolTip(tr("Close Document"));
    closeTab->setStatusTip(
               tr("Close the document and prompt to save the changes."));
    closeTab->setIconText(tr("Close"));
    closeTab->setShortcut(QKeySequence::Close);
    m_actions.insert("CloseDocument", closeTab);
    connect(closeTab, SIGNAL(triggered(bool)), this, SLOT(closeTab()));
    closeTab->setIcon(QIcon::fromTheme("document-close", QIcon(":/IconActions/CloseDocument")));

    QAction *closeAll = new QAction(tr("Close A&ll"));
    closeAll->setObjectName("CloseAllDocument");
    closeAll->setToolTip(tr("Close All Documents"));
    closeAll->setStatusTip(
               tr("Closes all documents and prompts to save the changes."));
    closeAll->setIconText(tr("Close All"));
#ifdef Q_WS_MAC
    closeAll->setText(tr("Close All in Window"));
#endif
    m_actions.insert("CloseAllDocument", closeAll);
    connect(closeAll, SIGNAL(triggered(bool)), this, SLOT(unimplemented()));
    closeAll->setIcon(QIcon(":/IconActions/CloseAll"));
    closeAll->setEnabled(false);
    QAction *closeOthers = new QAction(tr("Close Ot&hers"));
    closeOthers->setObjectName("CloseOthers");
    closeOthers->setToolTip(tr("Close Others"));
    closeOthers->setStatusTip(
               tr("Closes all documents besides the current and prompts to save the changes."));
    closeOthers->setIconText(tr("Close Others"));
#ifdef Q_WS_MAC
    closeOthers->setText(tr("Close All but Current Document"));
#endif
    m_actions.insert("CloseOthers", closeOthers);
    connect(closeOthers, SIGNAL(triggered(bool)), this, SLOT(unimplemented()));
    closeOthers->setEnabled(false);

    QAction *print = new QAction(tr("&Print..."));
    print->setObjectName("Print");
    print->setIcon(QIcon::fromTheme("document-print",
                                    QIcon(":/IconActions/Print")));
    print->setToolTip(tr("Save Document As"));
    print->setStatusTip(tr("Print this document."));
    print->setIconText(tr("Print"));
    print->setShortcut(QKeySequence::Print);
    m_actions.insert("Print", print);
    connect(print, SIGNAL(triggered(bool)), this, SLOT(print()));

    QAction *printprev = new QAction(tr("P&rint Preview"));
    printprev->setObjectName("PrintPreview");
    printprev->setIcon(QIcon::fromTheme("document-print-preview",
                                   QIcon(":/IconActions/PrintPreview")));
    printprev->setToolTip(tr("Print Preview"));
    printprev->setStatusTip(tr("Preview this document as a print out."));
    printprev->setIconText(tr("Print Preview"));
    m_actions.insert("PrintPreview", printprev);
    connect(printprev, SIGNAL(triggered(bool)), this, SLOT(printPreview()));

    QAction *pagesetup = new QAction(tr("Pa&ge Setup..."));
    pagesetup->setObjectName("PageSetup");
    pagesetup->setToolTip(tr("Configure Page Settings"));
    pagesetup->setStatusTip(
               tr("Save this document to a different location on disk."));
    pagesetup->setIconText(tr("Page Setup"));
    pagesetup->setIcon(QIcon::fromTheme("document-page-setup",
                                   QIcon(":/IconActions/PageSetup")));
    m_actions.insert("PageSetup", pagesetup);
    connect(pagesetup, SIGNAL(triggered(bool)), this, SLOT(pageSetup()));

    QAction *quit = new QAction(tr("&Quit"));
    quit->setObjectName("QuitApp");
    quit->setToolTip(tr("Quit %1").arg(qApp->applicationDisplayName()));
    quit->setStatusTip(tr("Prompt to save all changes and quit %1.")
                       .arg(qApp->applicationDisplayName()));
    quit->setIconText(tr("Quit"));
    quit->setShortcut(QKeySequence::Quit);

    if(QSysInfo::productType() == "windows")
    {
        quit->setText("&Exit");
        quit->setToolTip(tr("Exit %1").arg(qApp->applicationDisplayName()));
        quit->setStatusTip(tr("Prompt to save all changes and Exit %1.")
                          .arg(qApp->applicationDisplayName()));
        quit->setIconText(tr("Exit"));
        quit->setShortcut(QKeySequence("Alt+F4"));
    }

    m_actions.insert("QuitApp", quit);
    connect(quit, SIGNAL(triggered(bool)), myApp, SLOT(quit()));

    QAction *cut = new QAction(tr("&Cut"));
    cut->setObjectName("Cut");
    cut->setIcon(QIcon::fromTheme("edit-cut", QIcon(":/IconActions/Cut")));
    cut->setToolTip(tr("Cut Selection"));
    cut->setStatusTip(tr("Cut the current selection to the clipboard."));
    cut->setIconText(tr("Cut"));
    cut->setShortcut(QKeySequence::Cut);
    m_actions.insert("Cut", cut);
    connect(cut, SIGNAL(triggered(bool)),
            this, SLOT(cut()));

    QAction *copy = new QAction(tr("C&opy"));
    copy->setObjectName("Copy");
    copy->setIcon(QIcon::fromTheme("edit-copy", QIcon(":/IconActions/Copy")));
    copy->setToolTip(tr("Copy Selection"));
    copy->setStatusTip(tr("Copy the current selection to the clipboard."));
    copy->setIconText(tr("Copy"));
    copy->setShortcut(QKeySequence::Copy);
    m_actions.insert("Copy", copy);
    connect(copy, SIGNAL(triggered(bool)),
            this, SLOT(copy()));

    QAction *paste = new QAction(tr("&Paste"));
    paste->setObjectName("Paste");
    paste->setToolTip(tr("Paste Contents"));
    paste->setIcon(QIcon::fromTheme("edit-paste",
                                    QIcon(":/IconActions/Paste")));
    paste->setStatusTip(
            tr("Paste the contents of clipboard at the current position."));
    paste->setIconText(tr("Paste"));
    paste->setShortcut(QKeySequence::Paste);
    m_actions.insert("Paste", paste);
    paste->setEnabled(false);
    connect(paste, SIGNAL(triggered(bool)),
            this, SLOT(paste()));

    QAction *undo = new QAction(tr("&Undo"));
    undo->setObjectName("Undo");
    undo->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/IconActions/Undo")));
    undo->setToolTip(tr("Undo Last Action"));
    undo->setStatusTip(tr("Undo the last action."));
    undo->setIconText(tr("Undo"));
    undo->setShortcut(QKeySequence::Undo);
    m_actions.insert("Undo", undo);
    connect(undo, SIGNAL(triggered(bool)),
            this, SLOT(undo()));

    QAction *redo = new QAction(tr("&Redo"));
    redo->setObjectName("Redo");
    redo->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/IconActions/Redo")));
    redo->setToolTip(tr("Redo next action"));
    redo->setStatusTip(tr("Redo the next un-did action."));
    redo->setIconText(tr("Redo"));
    redo->setShortcut(QKeySequence::Redo);
    m_actions.insert("Redo", redo);
    connect(redo, SIGNAL(triggered(bool)),
            this, SLOT(redo()));

    QAction *deleteSelection = new QAction(tr("&Delete"));
    deleteSelection->setObjectName("Redo");
    deleteSelection->setIcon(QIcon::fromTheme("edit-redo",
                                              QIcon(":/IconActions/Redo")));
    deleteSelection->setToolTip(tr("Redo next action"));
    deleteSelection->setStatusTip(tr("Delete the selected text."));
    deleteSelection->setIconText(tr("Redo"));
    deleteSelection->setShortcut(QKeySequence::Delete);
    m_actions.insert("DeleteSelection", deleteSelection);
    connect(deleteSelection, SIGNAL(triggered(bool)), this, SLOT(deleteSelection()));

    QAction *select_all = new QAction(tr("Select &All"));
    select_all->setObjectName("SelectAll");
    select_all->setToolTip(tr("Select All"));
    select_all->setStatusTip(tr("Select all text in the buffer."));
    select_all->setIconText(tr("Select All"));
    select_all->setShortcut(QKeySequence::SelectAll);
    m_actions.insert("SelectAll", select_all);
    connect(select_all, SIGNAL(triggered(bool)),
            this, SLOT(selectAll()));
    select_all->setIcon(QIcon::fromTheme("edit-select-all",
                                   QIcon(":/IconActions/SelectAll")));

    QAction *select_word = new QAction(tr("Select &Word"));
    select_word->setObjectName("SelectWord");
    select_word->setToolTip(tr("Select Word"));
    select_word->setStatusTip(tr("Select the next word."));
    select_word->setIconText(tr("Select Word"));
    select_word->setShortcut(QKeySequence::SelectNextWord);
    m_actions.insert("SelectWord", select_word);
    connect(select_word, SIGNAL(triggered(bool)),
            this, SLOT(selectWord()));

    QAction *select_line = new QAction(tr("Select &Line"));
    select_line->setObjectName("SelectLine");
    select_line->setToolTip(tr("Select Line"));
    select_line->setStatusTip(tr("Select the current line."));
    select_line->setIconText(tr("Select Line"));
    select_line->setShortcut(QKeySequence::SelectNextLine);
    m_actions.insert("SelectLine", select_line);
    connect(select_line, SIGNAL(triggered(bool)),
            this, SLOT(selectLine()));

    QAction *insert_file = new QAction(tr("&File Contents..."));
    insert_file->setObjectName("InsertFileContents");
    insert_file->setToolTip(tr("Insert File"));
    insert_file->setStatusTip(tr("Insert the contents of a file at the current text cursor."));
    insert_file->setIconText(tr("Insert File"));
    m_actions.insert("InsertFile", insert_file);
    connect(insert_file, SIGNAL(triggered(bool)),
            this, SLOT(insertFileContents()));

    QAction *insert_dirlist = new QAction(tr("&Directory Listing..."));
    insert_dirlist->setObjectName("InsertDirectoryList");
    insert_dirlist->setToolTip(tr("Insert Directory List"));
    insert_dirlist->setStatusTip(tr("Insert a directory listing at the current text cursor."));
    insert_dirlist->setIconText(tr("Dir List"));
    m_actions.insert("InsertDirList", insert_dirlist);
    connect(insert_dirlist, SIGNAL(triggered(bool)),
            this, SLOT(insertDirectoryList()));

    QAction *insert_pgbreak = new QAction(tr("&Page Break"));
    insert_pgbreak->setObjectName("InsertPageBreak");
    insert_pgbreak->setToolTip(tr("Insert Page Break"));
    insert_pgbreak->setStatusTip(tr("Insert a page break at the current text cursor."));
    insert_pgbreak->setIconText(tr("Page Break"));
    m_actions.insert("InsertPageBreak", insert_pgbreak);
    connect(insert_pgbreak, SIGNAL(triggered(bool)),
            this, SLOT(insertPageBreak()));

    QAction *insert_shortdate = new QAction(tr("&Short Timestamp"));
    insert_shortdate->setObjectName("InsertShortTS");
    insert_shortdate->setToolTip(tr("Insert Short Timestamp"));
    insert_shortdate->setStatusTip(tr("Insert a short timestamp at the current text cursor."));
    insert_shortdate->setIconText(tr("Short TS"));
    m_actions.insert("InsertShortTimestamp", insert_shortdate);
    //connect(insert_shortdate, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *insert_fulldate = new QAction(tr("Full &Timestamp"));
    insert_fulldate->setObjectName("InsertLongTS");
    insert_fulldate->setToolTip(tr("Insert Full Timestamp"));
    insert_fulldate->setStatusTip(tr("Insert a full timestamp at the current text cursor."));
    insert_fulldate->setIconText(tr("Full TS"));
    m_actions.insert("InsertFullTimestamp", insert_fulldate);
    //connect(insert_fulldate, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *insert_color_css = new QAction(tr("C&olor (CSS)..."));
    insert_color_css->setObjectName("InsertColorCSS");
    insert_color_css->setToolTip(tr("Insert CSS Color"));
    insert_color_css->setStatusTip(tr("Insert a CSS rgb() color code at the current text cursor."));
    insert_color_css->setIconText(tr("Insert CSS Color"));
    m_actions.insert("InsertColorCSS", insert_color_css);
    //connect(insert_color, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *insert_color = new QAction(tr("&Color (HTML)..."));
    insert_color->setObjectName("InsertColor");
    insert_color->setToolTip(tr("Insert HTML Color"));
    insert_color->setStatusTip(tr("Insert a HTML HEX color code at the current text cursor."));
    insert_color->setIconText(tr("Insert Color"));
    m_actions.insert("InsertColor", insert_color);
    //connect(insert_color, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *touppercase = new QAction(tr("To &Uppercase"));
    touppercase->setObjectName("ToUppercase");
    touppercase->setToolTip(tr("To Uppercase"));
    touppercase->setStatusTip(tr("Convert the current selection to uppercase."));
    touppercase->setIconText(tr("To Uppercase"));
    m_actions.insert("ToUppercase", touppercase);
    //connect(insert_longdate, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));


    QAction *tolowercase = new QAction(tr("To &Lowercase"));
    tolowercase->setObjectName("ToLowercase");
    tolowercase->setToolTip(tr("To Lowercase"));
    tolowercase->setStatusTip(tr("Convert the current selection to lowercases."));
    tolowercase->setIconText(tr("To Lowercase"));
    m_actions.insert("ToLowercase", tolowercase);
    //connect(tolowercase, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *cap_words = new QAction(tr("Capitalize &Words"));
    cap_words->setObjectName("CapitalizeWords");
    cap_words->setToolTip(tr("Capitalize Words"));
    cap_words->setStatusTip(tr("Capitalize all words in the selection."));
    cap_words->setIconText(tr("Capitalize Words"));
    m_actions.insert("CapitalizeWords", cap_words);
    //connect(cap_words, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *cap_sentences = new QAction(tr("Capitalize &Sentences"));
    cap_sentences->setObjectName("CapitalizeSentences");
    cap_sentences->setToolTip(tr("Capitalize Sentences"));
    cap_sentences->setStatusTip(tr("Capitalize all sentences in the selection."));
    cap_sentences->setIconText(tr("Capitalize Words"));
    m_actions.insert("CapitalizeSentences", cap_sentences);
    //connect(cap_sentences, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *educate_quotes = new QAction(tr("&Educate Quotes"));
    educate_quotes->setObjectName("EducateQuotes");
    educate_quotes->setToolTip(tr("Educate Quotes"));
    educate_quotes->setStatusTip(tr("Convert all quotation marks to educated values."));
    educate_quotes->setIconText(tr("Educate Quotes"));
    m_actions.insert("EducateQuotes", educate_quotes);
    //connect(educate_quotes, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *straighten_quotes = new QAction(tr("Straighten &Quotes"));
    straighten_quotes->setObjectName("StraightenQuotes");
    straighten_quotes->setToolTip(tr("Straighten Quotes"));
    straighten_quotes->setStatusTip(tr("Convert all quotation marks to straight values."));
    straighten_quotes->setIconText(tr("Straighten Quotes"));
    m_actions.insert("StraightenQuotes", straighten_quotes);
    //connect(educate_quotes, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *b64_enc = new QAction(tr("&Base64 Encode"));
    b64_enc->setObjectName("Base64Encode");
    b64_enc->setToolTip(tr("Base64 Encode"));
    b64_enc->setStatusTip(tr("Encode the selection with Base64."));
    b64_enc->setIconText(tr("Base64 Encode"));
    m_actions.insert("Base64Encode", b64_enc);
    //connect(educate_quotes, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *b64_dec = new QAction(tr("Base64 &Decode"));
    b64_dec->setObjectName("Base64Decode");
    b64_dec->setToolTip(tr("Base64 Decode"));
    b64_dec->setStatusTip(tr("Decode the selected Base64 to plain text."));
    b64_dec->setIconText(tr("Base64 Decode"));
    m_actions.insert("Base64Decode", b64_dec);
    //connect(educate_quotes, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *pipe_cmd = new QAction(tr("&Pipe Through Command..."));
    pipe_cmd->setObjectName("PipeThroughCommand");
    pipe_cmd->setToolTip(tr("Pipe Through Command"));
    pipe_cmd->setStatusTip(tr("Pipe the selected text through a command, and place the result back in the buffer."));
    pipe_cmd->setIconText(tr("Pipe Through Command"));
    m_actions.insert("PipeCommand", pipe_cmd);
    //connect(pipe_cmd, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    QAction *zapg = new QAction(tr("&Zap Gremlins..."));
    zapg->setObjectName("ZapGremlins");
    zapg->setToolTip(tr("ZapGremlins"));
    zapg->setStatusTip(tr("Locate and take action on null, control and non-ASCII charaters (gremlins)."));
    zapg->setIconText(tr("Zap Gremlins"));
    m_actions.insert("ZapGremlins", zapg);
    //connect(educate_quotes, SIGNAL(triggered(bool)),
    //        this, SLOT(selectAll()));

    // -------- FIND MENU ITEMS ---------

    QAction *find = new QAction(tr("&Find && Replace..."));
    find->setObjectName("Find");
    find->setToolTip(tr("Find"));
    find->setStatusTip(tr("Find and Replace in the current document."));
    find->setIconText(tr("Find"));
    find->setShortcut(QKeySequence::Find);
    m_actions.insert("FindAndReplace", find);
    connect(find, SIGNAL(triggered(bool)), this, SLOT(findAndReplace()));
    find->setIcon(QIcon::fromTheme("edit-find-replace",
                                   QIcon(":/IconActions/FindAndReplace")));

    QAction *find_next = new QAction(tr("Find &Next"));
    find_next->setObjectName("FindNext");
    find_next->setToolTip(tr("Find Next"));
    find_next->setStatusTip(tr("Find the next match."));
    find_next->setIconText(tr("Find Next"));
    find_next->setShortcut(QKeySequence::FindNext);
    m_actions.insert("FindNext", find_next);
    find_next->setIcon(QIcon::fromTheme("edit-find-replace",
                                   QIcon(":/IconActions/Find")));

    QAction *find_prev = new QAction(tr("Find &Previous"));
    find_prev->setObjectName("FindPrevious");
    find_prev->setToolTip(tr("Find Previous"));
    find_prev->setStatusTip(tr("Find the previous match."));
    find_prev->setIconText(tr("Find Previous"));
    find_prev->setShortcut(QKeySequence::FindPrevious);
    m_actions.insert("FindPrevious", find_prev);

    QAction *find_all = new QAction(tr("Find &All"));
    find_all->setObjectName("FindAll");
    find_all->setToolTip(tr("Find All"));
    find_all->setStatusTip(tr("Find all matches in the current document."));
    find_all->setIconText(tr("Find All"));
    //find_prev->setShortcut(QKeySequence::);
    m_actions.insert("FindAll", find_all);

    QAction *replace = new QAction(tr("&Replace"));
    replace->setObjectName("Replace");
    replace->setToolTip(tr("Replace"));
    replace->setStatusTip(tr("Make a replacement."));
    replace->setIconText(tr("Replace"));
    replace->setShortcut(QKeySequence::Replace);
    m_actions.insert("Replace", replace);
    replace->setIcon(QIcon::fromTheme("edit-find-replace",
                                   QIcon(":/IconActions/Replace")));

    QAction *replace_next = new QAction(tr("Replace && Find Ne&xt"));
    replace_next->setObjectName("ReplaceNext");
    replace_next->setToolTip(tr("Replace & Find Next"));
    replace_next->setStatusTip(tr("Make a replacement, and find the next match."));
    replace_next->setIconText(tr("Find Next"));
    //replace_next->setShortcut(QKeySequence::Replace);
    m_actions.insert("ReplaceNext", replace_next);

    QAction *replace_prev = new QAction(tr("Replace && Find Pre&vious"));
    replace_prev->setObjectName("ReplacePrevious");
    replace_prev->setToolTip(tr("Replace & Find Previous"));
    replace_prev->setStatusTip(tr("Make a replacement, and find the previous match."));
    replace_prev->setIconText(tr("Replace & Find Previous"));
    m_actions.insert("ReplacePrevious", replace_prev);

    QAction *replace_all = new QAction(tr("Replace A&ll"));
    replace_all->setObjectName("ReplaceAll");
    replace_all->setToolTip(tr("Replace All"));
    replace_all->setStatusTip(tr("Replace all matches in the current document."));
    replace_all->setIconText(tr("Replace All"));
    m_actions.insert("ReplaceAll", replace_all);

    QAction *toggle_bookmark = new QAction(tr("Toggle &Bookmark"));
    toggle_bookmark->setObjectName("ToggleBookmark");
    toggle_bookmark->setToolTip(tr("Toggle Bookmark"));
    toggle_bookmark->setStatusTip(tr("Toggle a bookmark on the current line."));
    toggle_bookmark->setIconText(tr("Toggle Bookmark"));
    toggle_bookmark->setShortcut(QKeySequence("Ctrl+F2"));
    toggle_bookmark->setIcon(QIcon(":/IconActions/Bookmark"));
    m_actions.insert("ToggleBookmark", toggle_bookmark);

    QAction *clear_bookmarks = new QAction(tr("&Clear Bookmarks"));
    clear_bookmarks->setObjectName("ClearBookmarks");
    clear_bookmarks->setToolTip(tr("Clear Bookmarks"));
    clear_bookmarks->setStatusTip(tr("Clear all bookmarks in the current file."));
    clear_bookmarks->setIconText(tr("Clear Bookmarks"));
    m_actions.insert("ClearBookmarks", clear_bookmarks);

    QAction *next_bookmark = new QAction(tr("Next Bookmar&k"));
    next_bookmark->setObjectName("NextBookmark");
    next_bookmark->setToolTip(tr("Next Bookmark"));
    next_bookmark->setStatusTip(tr("Move the cursor to the next bookmark."));
    next_bookmark->setIconText(tr("Next Bookmark"));
    next_bookmark->setIcon(QIcon(":/IconActions/NextBookmark"));
    next_bookmark->setShortcut(QKeySequence("F2"));

    m_actions.insert("NextBookmark", next_bookmark);

    QAction *prev_bookmark = new QAction(tr("Previo&us Bookmark"));
    prev_bookmark->setObjectName("PreviousBookmark");
    prev_bookmark->setToolTip(tr("Previous Bookmark"));
    prev_bookmark->setStatusTip(tr("Move the cursor to the previous bookmark."));
    prev_bookmark->setIconText(tr("Previous Bookmark"));
    prev_bookmark->setShortcut(QKeySequence("Shift+F2"));
    prev_bookmark->setIcon(QIcon(":/IconActions/PreviousBookmark"));
    m_actions.insert("PreviousBookmark", prev_bookmark);

    QAction *find_line = new QAction(tr("Find &Line by Number..."));
    find_line->setObjectName("FindLine");
    find_line->setToolTip(tr("Find Line by Number"));
    find_line->setStatusTip(tr("Jump to a line by number."));
    find_line->setIconText(tr("Line No."));
    find_line->setShortcut(QKeySequence("Ctrl+L"));
    m_actions.insert("FindLine", find_line);
    connect(find_line, SIGNAL(triggered(bool)),
            this, SLOT(promptForLineNumber()));


    // -------- VIEW MENU ITEMS ---------

    QAction *showtoolbar = new QAction(tr("&Toolbar"));
    showtoolbar->setObjectName("ShowToolbar");
    showtoolbar->setStatusTip(tr("Show or hide the toolbar."));
    showtoolbar->setIconText(tr("Toolbar"));
    showtoolbar->setToolTip(tr("Toggle Toolbar"));
    m_actions.insert("ShowToolbar", showtoolbar);
    connect(showtoolbar, SIGNAL(triggered(bool)), this, SLOT(triggerToolbar()));
#ifndef Q_WS_MAC
    showtoolbar->setCheckable(true);
#else
    showtoolbar->setShortcut(QKeySequence("Ctrl+Alt+T"));
#endif

    QAction *showstatusbar = new QAction(tr("&Status Bar"));
    showstatusbar->setObjectName("ShowStatusBar");
#ifndef Q_WS_MAC
    showstatusbar->setCheckable(true);
#else
    showstatusbar->setShortcut(QKeySequence("Ctrl+/"));
#endif
    showstatusbar->setToolTip(tr("Toggle Status Bar"));
    showstatusbar->setStatusTip(tr("Show or hide the status bar."));
    showstatusbar->setIconText(tr("Status Bar"));
    m_actions.insert("ShowStatusBar", showstatusbar);
    connect(showstatusbar, SIGNAL(triggered(bool)), this, SLOT(triggerStatusBar()));

    QAction *linenumbers = new QAction(tr("Line &Numbers"));
    linenumbers->setObjectName("ShowLineNumbers");
#ifndef Q_WS_MAC
    linenumbers->setCheckable(true);
#else
    linenumbers->setShortcut(QKeySequence("Ctrl+Shift+L"));
#endif
    linenumbers->setToolTip(tr("Toggle Line Numbers"));
    linenumbers->setStatusTip(tr("Show or hide line numbering."));
    linenumbers->setIconText(tr("Status Bar"));
    m_actions.insert("ShowLineNumbers", linenumbers);
    connect(linenumbers, SIGNAL(triggered(bool)), this, SLOT(triggerLineNumbers()));

    QAction *wordwrap = new QAction(tr("&Word Wrap"));
    wordwrap->setObjectName("WordWrap");
    wordwrap->setCheckable(true);
    wordwrap->setToolTip(tr("Toggle Word Wrap"));
    //wordwrap->setStatusTip(tr("Redo the next un-did action."));
    wordwrap->setIconText(tr("Word Wrap"));
    m_actions.insert("WordWrap", wordwrap);

    QAction *whitespace = new QAction(tr("&Visualize Whitespace"));
    whitespace->setObjectName("VisualizeWhiteSpace");
#ifndef Q_WS_MAC
    whitespace->setCheckable(true);
#endif
    whitespace->setToolTip(tr("Toggle White Space Characters"));
    whitespace->setStatusTip(tr("Toggle white space characters."));
    whitespace->setIconText(tr("Visualize Whitespace"));
    m_actions.insert("VisualizeWhiteSpace", whitespace);
    connect(whitespace, SIGNAL(triggered(bool)), this, SLOT(triggerVisualizeWhitespace()));
    whitespace->setIcon(QIcon(":/IconActions/VisualizeWhitespace"));

    QAction *fold_sel = new QAction(tr("&Fold Selection"));
    fold_sel->setObjectName("FoldSelection");
    fold_sel->setToolTip(tr("Fold Selection"));
    fold_sel->setStatusTip(tr("Fold the current selection."));
    fold_sel->setIconText(tr("Fold Selection"));
    fold_sel->setShortcut(QKeySequence("Shift+Ctrl+<"));
    m_actions.insert("FoldSelection", fold_sel);

    QAction *fold_block = new QAction(tr("Fold &Block"));
    fold_block->setObjectName("FoldBlock");
    fold_block->setToolTip(tr("Fold Block"));
    fold_block->setStatusTip(tr("Fold the current block."));
    fold_block->setIconText(tr("Fold Block"));
    fold_block->setShortcut(QKeySequence("Ctrl+<"));

    m_actions.insert("FoldBlock", fold_block);

    QAction *unfold = new QAction(tr("&Unfold Once"));
    unfold->setObjectName("Unfold");
    unfold->setToolTip(tr("Unfold"));
    unfold->setStatusTip(tr("Unfold the selected folded text block."));
    unfold->setIconText(tr("Unfold"));
    unfold->setShortcut(QKeySequence("Ctrl+>"));
    m_actions.insert("Unfold", unfold);

    QAction *unfold_all = new QAction(tr("Unfold &All Blocks"));
    unfold_all->setObjectName("UnfoldAll");
    unfold_all->setToolTip(tr("Unfold All"));
    unfold_all->setStatusTip(tr("Unfold all folded text blocks."));
    unfold_all->setIconText(tr("Unfold All"));
    unfold_all->setShortcut(QKeySequence("Shift+Ctrl+>"));
    m_actions.insert("UnfoldAll", unfold_all);

    // TODO: enable folding
    fold_sel->setDisabled(true);
    fold_block->setDisabled(true);
    unfold->setDisabled(true);
    unfold_all->setDisabled(true);

    QAction *colors = new QAction(tr("Screen &Colors..."));
    colors->setObjectName("Colors");
    colors->setToolTip(tr("Screen Colors"));
    colors->setStatusTip(tr("Configure Screen Colors and Syntax Highlighting."));
    colors->setIconText(tr("Colors"));
    colors->setIcon(QIcon(":/IconActions/ScreenColors"));

    m_actions.insert("Colors", colors);
    connect(colors, SIGNAL(triggered(bool)), this, SLOT(configureColors()));

    QAction *fonts = new QAction(tr("Screen &Font..."));
    fonts->setObjectName("Fonts");
    fonts->setToolTip(tr("Configure Fonts"));
    fonts->setStatusTip(tr("Configure Screen Fonts."));
    fonts->setIconText(tr("Fonts"));
    fonts->setIcon(QIcon(":/IconActions/Fonts"));

    m_actions.insert("Fonts", fonts);
#ifdef Q_WS_MAC
    fonts->setShortcut(QKeySequence("Ctrl+T"));
#endif
    connect(fonts, SIGNAL(triggered(bool)), this, SLOT(configureFont()));

    QAction *prefs = new QAction(tr("&Preferences..."));
    prefs->setObjectName("Preferences");
    prefs->setToolTip(tr("Preferences"));
    prefs->setStatusTip(tr("Set application preferences."));
    prefs->setIconText(tr("Preferences"));
    m_actions.insert("Preferences", prefs);
    connect(prefs, SIGNAL(triggered(bool)), this, SLOT(showPreferences()));

    // Syntax actions

    QActionGroup *syntax = new QActionGroup(this);

    QAction *sn_plain = new QAction(tr("&Plain Text"));
    sn_plain->setObjectName("SyntaxPlain");
    sn_plain->setToolTip(tr("Plain Text Syntax"));
    sn_plain->setStatusTip(tr("Use no syntax highlighting and language features."));
    sn_plain->setIconText(tr("Plain Text"));
    sn_plain->setCheckable(true);
    m_actions.insert("SyntaxPlain", sn_plain);
    m_language_feature_actions.append(sn_plain);
    syntax->addAction(sn_plain);
    connect(sn_plain, SIGNAL(triggered(bool)), this, SLOT(changeSyntax()));

    QAction *sn_cpp = new QAction(tr("&C/C++"));
    sn_cpp->setObjectName("SyntaxCPP");
    sn_cpp->setToolTip(tr("C/C++ Syntax"));
    sn_cpp->setStatusTip(tr("Use C/C++ syntax highlighting and language features.."));
    sn_cpp->setIconText(tr("C/C++"));
    sn_cpp->setCheckable(true);
    m_actions.insert("SyntaxCPP", sn_cpp);
    m_language_feature_actions.append(sn_cpp);
    connect(sn_cpp, SIGNAL(triggered(bool)), this, SLOT(changeSyntax()));
    syntax->addAction(sn_cpp);

    QAction *sn_html = new QAction(tr("&HTML"));
    sn_html->setObjectName("SyntaxHTML");
    sn_html->setToolTip(tr("HTML Syntax"));
    sn_html->setStatusTip(tr("Use HTML syntax highlighting and language features.."));
    sn_html->setIconText(tr("HTML"));
    sn_html->setCheckable(true);
    m_actions.insert("SyntaxHTML", sn_html);
    m_language_feature_actions.append(sn_html);
    connect(sn_html, SIGNAL(triggered(bool)), this, SLOT(changeSyntax()));
    syntax->addAction(sn_html);


    // Line Ending actions

    QActionGroup *lineendings = new QActionGroup(this);

    QAction *le_unix = new QAction(tr("&Unix (LF)"));
    le_unix->setObjectName("UnixLineEndings");
    le_unix->setToolTip(tr("UNIX (LF) Line Endings"));
    le_unix->setStatusTip(tr("Use UNIX style (\\n) line endings."));
    le_unix->setIconText(tr("UNIX"));
    le_unix->setCheckable(true);
    lineendings->addAction(le_unix);
    m_actions.insert("UnixLineEnding", le_unix);

    QAction *le_mac = new QAction(tr("&Classic Mac (CR)"));
    le_mac->setObjectName("MacLineEndings");
    le_mac->setToolTip(tr("Preferences"));
    le_mac->setStatusTip(tr("Use Classic Macintosh style (\\r) line endings."));
    le_mac->setIconText(tr("Mac"));
    le_mac->setCheckable(true);
    lineendings->addAction(le_mac);
    m_actions.insert("MacLineEnding", le_mac);

    QAction *le_win = new QAction(tr("&Windows (CRLF)"));
    le_win->setObjectName("WindowsLineEndings");
    le_win->setToolTip(tr("Windows (CR+LF) Line Endings"));
    le_win->setStatusTip(tr("Use Windows style (\\r\\n) line endings."));
    le_win->setIconText(tr("Windows"));
    le_win->setCheckable(true);
    lineendings->addAction(le_win);
    m_actions.insert("WinLineEnding", le_win);


    // TOOLS options

    QAction *check_spelling = new QAction(tr("&Check Spelling..."));
    check_spelling->setObjectName("CheckSpelling");
    check_spelling->setToolTip(tr("Check Spelling"));
    check_spelling->setStatusTip(tr("Check spelling in the current document.."));
    check_spelling->setIconText(tr("Check Spelling"));
    check_spelling->setIcon(QIcon::fromTheme("tools-check-spelling", QIcon(":/IconActions/CheckSpelling")));
    m_actions.insert("CheckSpelling", check_spelling);
#ifndef Q_WS_MAC
    check_spelling->setShortcut(QKeySequence("F7"));
#else
    check_spelling->setShortcut(QKeySequence("Ctrl+Shift+;"));
#endif
    connect(check_spelling, SIGNAL(triggered(bool)), this, SLOT(checkSpelling()));


    QAction *open_terminal = new QAction(tr("Open &Terminal"));
    open_terminal->setObjectName("OpenTerminal");
    open_terminal->setToolTip(tr("Open Terminal"));
    open_terminal->setStatusTip(tr("Open a Terminal window."));
    open_terminal->setIconText(tr("Terminal"));
    m_actions.insert("OpenTerminal", open_terminal);
#ifdef Q_WS_WIN
    open_terminal->setToolTip(tr("Open Command Prompt"));
    open_terminal->setStatusTip(tr("Open a Command Prompt window."));
    open_terminal->setIconText(tr("Command Prompt"));
    open_terminal->setText(tr("&Command Prompt"));

    QAction *open_ps = new QAction(tr("Windows &PowerShell"));
    open_ps->setObjectName("OpenPowerShell");
    open_ps->setToolTip(tr("Open PowerShell"));
    open_ps->setStatusTip(tr("Open a PowerShell window."));
    open_ps->setIconText(tr("PowerShell"));
    m_actions.insert("OpenPowerShell", open_ps);

    open_terminal->setEnabled(false);
    open_ps->setEnabled(false);
#endif

    QAction *snippet_mgr = new QAction(tr("Manage &Snippets..."));
    snippet_mgr->setObjectName("SnippetManager");
    snippet_mgr->setToolTip(tr("Manage Snippets"));
    snippet_mgr->setStatusTip(tr("Show and edit snippets."));
    snippet_mgr->setIconText(tr("Snippets"));
    m_actions.insert("SnippetManager", snippet_mgr);

    QAction *user_tools = new QAction(tr("Configure &User Tools..."));
    user_tools->setObjectName("ConfigureUserTools");
    user_tools->setToolTip(tr("Configure User Tools"));
    user_tools->setStatusTip(tr("Configure user defined tools."));
    user_tools->setIconText(tr("User Tools"));
    m_actions.insert("ConfigureUserTools", user_tools);

    // TODO: tools
    user_tools->setEnabled(false);
    snippet_mgr->setEnabled(false);

    QAction *preview_browser = new QAction(tr("&Preview in Web Browser"));
    preview_browser->setObjectName("PreviewInBrowser");
    preview_browser->setToolTip(tr("Preview in Web Browser"));
    preview_browser->setStatusTip(tr("Preview in your default web browser."));
    preview_browser->setIconText(tr("Browser"));
    m_actions.insert("PreviewInBrowser", preview_browser);
#ifdef Q_WS_WIN
    preview_browser->setShortcut(QKeySequence("F10"));
    QString appPath = myApp->findDefaultBrowserWindows();

    appPath = appPath.split('"', QString::SkipEmptyParts).first();
    if(QFile::exists(appPath))
    {
        HICON icon;
        LPCWSTR path = (const wchar_t*)appPath.utf16();
        if(ExtractIconEx(path, 0, &icon, NULL, 1) > 0)
        {
            preview_browser->setIcon(QIcon(luv_pixmapFromWinHICON(icon)));
            DestroyIcon(icon);
        }

        DWORD dummy;
        DWORD fviSize = GetFileVersionInfoSize(path, &dummy);

        LPBYTE lpvi = new BYTE[fviSize];

        if(GetFileVersionInfo(path, 0, fviSize, lpvi))
        {
            UINT uiSize;
            LPBYTE lpb;
            LPBYTE lpb2;
            if(VerQueryValue(lpvi, TEXT("\\VarFileInfo\\Translation"),
                             (void**)&lpb, &uiSize))
            {
                WORD* lpw = (WORD*)lpb;

                WCHAR key[128];
                wsprintf(key, L"\\StringFileInfo\\%04x%04x\\ProductName",
                         lpw[0], lpw[1]);

                if(VerQueryValue(lpvi,
                                   key,
                                   (void**)&lpb2,
                                   &uiSize))
                {
                    QString appName =
                            QString::fromWCharArray((const wchar_t*)lpb2);
                    preview_browser->setText(tr("Preview in %1").arg(appName));
                    preview_browser->setStatusTip(tr("Preview in %1 browser.")
                                                  .arg(appName));
                }
            }
        }
    }
#endif
    /* QAction *scm_log = new QAction(tr("Git &Log"));
    scm_log->setObjectName("GitLog");
    scm_log->setToolTip(tr("Git Log"));
    scm_log->setStatusTip(tr("View a Git log for this file."));
    scm_log->setIconText(tr("Git Log"));
    m_actions.insert("SCMLog", scm_log);

    QAction *scm_add = new QAction(tr("Git &Add"));
    scm_add->setObjectName("Git Add");
    scm_add->setToolTip(tr("Git Add"));
    scm_add->setStatusTip(tr("Add this file into Git."));
    scm_add->setIconText(tr("Git Add"));
    m_actions.insert("SCMAdd", scm_add);

    QAction *scm_blame = new QAction(tr("Git &Blame"));
    scm_blame->setObjectName("Git Blame");
    scm_blame->setToolTip(tr("Git Blame"));
    scm_blame->setStatusTip(tr("Git blame for this file."));
    scm_blame->setIconText(tr("Git Blame"));
    m_actions.insert("SCMBlame", scm_blame); */

    // window actions
    QAction *minimize = new QAction(tr("&Minimize"));
    minimize->setObjectName("Minimize");
    minimize->setToolTip(tr("Minimize Window"));
    minimize->setStatusTip(tr("Minimize this window."));
    minimize->setIconText(tr("Minimize"));
#ifdef Q_WS_MAC
    minimize->setShortcut(QKeySequence("Ctrl+M"));
#endif
    m_actions.insert("Minimize", minimize);

    QAction *zoom = new QAction(tr("&Zoom"));
    zoom->setObjectName("Zoom");
    zoom->setToolTip(tr("Zoom Window"));
    zoom->setStatusTip(tr("Zoom this window to fill the screen."));
    zoom->setIconText(tr("Zoom"));
    m_actions.insert("Zoom", zoom);

    QAction *next_tab = new QAction(tr("Select &Next Tab"));
    next_tab->setObjectName("SelectNextTab");
    next_tab->setToolTip(tr("Next Tab"));
    next_tab->setStatusTip(tr("Select the next tab in the window."));
    next_tab->setIconText(tr("Next Tab"));
    m_actions.insert("SelectNextTab", next_tab);
#ifdef Q_WS_MAC
    next_tab->setShortcut(QKeySequence("META+Tab"));
#endif

    QAction *prev_tab = new QAction(tr("Select &Previous Tab"));
    prev_tab->setObjectName("SelectPreviousTab");
    prev_tab->setToolTip(tr("Previous Tab"));
    prev_tab->setStatusTip(tr("Select the previous tab in the window."));
    prev_tab->setIconText(tr("Previous Tab"));
    m_actions.insert("SelectPreviousTab", prev_tab);
#ifdef Q_WS_MAC
    prev_tab->setShortcut(QKeySequence("META+Shift+Tab"));
#endif
    QAction *help = new QAction(tr("%1 &Help").arg(qApp->applicationName()));
    help->setObjectName("Help");
    help->setToolTip(tr("Help Topics"));
    help->setStatusTip(tr("Show all help topics for the application."));
    help->setIconText(tr("Help"));
    help->setShortcut(QKeySequence::HelpContents);
    m_actions.insert("Help", help);
    help->setIcon(QIcon::fromTheme("help-contents",
                                   QIcon(":/IconActions/Help")));


    QAction *acknowledgements = new QAction(tr("&Acknowledgements"));
    acknowledgements->setObjectName("Acknowledgements");
    acknowledgements->setToolTip(tr("Acknowledgements"));
    acknowledgements->setStatusTip(
                tr("Show third party code and library acknowledgements."));
    acknowledgements->setIconText(tr("Acknowledgements"));
    m_actions.insert("Acknowledgements", acknowledgements);

    QAction *update = new QAction(tr("Check for &Updates..."));
    update->setObjectName("Preferences");
    update->setToolTip(tr("Preferences"));
    update->setStatusTip(tr("Set application preferences."));
    update->setIconText(tr("Preferences"));
    update->setMenuRole(QAction::ApplicationSpecificRole);
    m_actions.insert("Update", update);

    QAction *about = new QAction(tr("&About %1").arg(qApp->applicationName()));
    about->setObjectName("About");
    about->setToolTip(tr("About %1").arg(qApp->applicationName()));
    about->setStatusTip(tr("Show information about the application."));
    about->setIconText(tr("About"));
    m_actions.insert("About", about);
    connect(about, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

#ifdef Q_WS_MAC
    foreach(QAction *a, m_actions)
        a->setIconVisibleInMenu(false);
#endif
}

void EditorWindow::createMainMenu()
{
    m_menubar->setAccessibleName(tr("Menu Bar"));
    m_menubar->setAccessibleDescription(tr("%1 Main Menu Bar")
                                     .arg(qApp->applicationDisplayName()));

    QMenu *file = m_menubar->addMenu(tr("&File"));
    QMenu *edit = m_menubar->addMenu(tr("&Edit"));
    QMenu *search = m_menubar->addMenu(tr("&Search"));
    QMenu *view = m_menubar->addMenu(tr("&View"));
    QMenu *tools = m_menubar->addMenu(tr("&Tools"));
    QMenu *window = m_menubar->addMenu(tr("&Window"));
    QMenu *help = m_menubar->addMenu(tr("&Help"));

/* #ifdef WINDOWS_DESKTOP
    file->setToolTipsVisible(true);
    edit->setToolTipsVisible(true);
    search->setToolTipsVisible(true);
    view->setToolTipsVisible(true);
    tools->setToolTipsVisible(true);
    window->setToolTipsVisible(true);
    help->setToolTipsVisible(true);
#endif */

    file->setObjectName(QString("FileMenu"));
    edit->setObjectName(QString("EditMenu"));
    view->setObjectName(QString("ViewMenu"));
    tools->setObjectName(QString("ToolsMenu"));
    window->setObjectName(QString("WindowMenu"));
    help->setObjectName(QString("HelpMenu"));

    QMenu *mru = new QMenu(tr("Recent Fi&les"));
    connect(mru, SIGNAL(aboutToShow()),
            this, SLOT(buildMRUMenu()));

    connect(mru, SIGNAL(triggered(QAction*)),
            this, SLOT(openMRUItem(QAction*)));

    /* connect(mru, SIGNAL(aboutToHide()),
            this, SLOT(destroyMRUMenuLater())); */

    file->addAction(findActionByName("NewDocument"));
    file->addAction(findActionByName("Open"));
#ifdef Q_WS_MAC
    mru->setTitle(tr("Open Re&cent"));
    file->addMenu(mru);
#endif
    file->addAction(findActionByName("RevealFile"));
    file->addSeparator();
    file->addAction(findActionByName("CloseDocument"));
    file->addAction(findActionByName("CloseAllDocument"));
    file->addAction(findActionByName("CloseOthers"));
    file->addSeparator();
    QMenu *enc = file->addMenu(tr("Text &Encoding"));
    connect(enc, SIGNAL(aboutToShow()), this, SLOT(aboutToShowEncodingMenu()));
    file->addSeparator();
    file->addAction(findActionByName("Save"));
    file->addAction(findActionByName("SaveAs"));
    file->addAction(findActionByName("SaveAll"));


    file->addSeparator();
    file->addAction(findActionByName("PageSetup"));
    file->addAction(findActionByName("Print"));
#ifndef Q_WS_MAC
    file->addAction(findActionByName("PrintPreview"));
    file->addSeparator();
    file->addMenu(mru);
#endif
    file->addSeparator();
    file->addAction(findActionByName("QuitApp"));

    edit->addAction(findActionByName("Undo"));
    edit->addAction(findActionByName("Redo"));
    edit->addSeparator();
    edit->addAction(findActionByName("Cut"));
    edit->addAction(findActionByName("Copy"));
    edit->addAction(findActionByName("Paste"));
    edit->addAction(findActionByName("DeleteSelection"));
    edit->addSeparator();
    edit->addAction(findActionByName("SelectAll"));
    edit->addAction(findActionByName("SelectWord"));
    edit->addAction(findActionByName("SelectLine"));
    edit->addSeparator();

    QMenu *insert = edit->addMenu(tr("&Insert"));
    insert->addAction(findActionByName("InsertPageBreak"));
    insert->addSeparator();
    insert->addAction(findActionByName("InsertFile"));
    insert->addAction(findActionByName("InsertDirList"));
    insert->addSeparator();
    insert->addAction(findActionByName("InsertShortTimestamp"));
    insert->addAction(findActionByName("InsertFullTimestamp"));
    insert->addSeparator();
    insert->addAction(findActionByName("InsertColor"));
    insert->addAction(findActionByName("InsertColorCSS"));

    QMenu *transform = edit->addMenu(tr("&Transform"));
    transform->addAction(findActionByName("ToUppercase"));
    transform->addAction(findActionByName("ToLowercase"));
    transform->addAction(findActionByName("CapitalizeWords"));
    transform->addAction(findActionByName("CapitalizeSentences"));
    transform->addSeparator();
    transform->addAction(findActionByName("EducateQuotes"));
    transform->addAction(findActionByName("StraightenQuotes"));
    transform->addSeparator();
    transform->addAction(findActionByName("Base64Encode"));
    transform->addAction(findActionByName("Base64Decode"));
    transform->addSeparator();
    transform->addAction(findActionByName("PipeCommand"));
    transform->addAction(findActionByName("ZapGremlins"));

    search->addAction(findActionByName("FindAndReplace"));
    search->addSeparator();
    search->addAction(findActionByName("FindNext"));
    search->addAction(findActionByName("FindPrevious"));
    search->addAction(findActionByName("FindAll"));
    search->addSeparator();
    search->addAction(findActionByName("Replace"));
    search->addAction(findActionByName("ReplaceNext"));
    search->addAction(findActionByName("ReplacePrevious"));
    search->addAction(findActionByName("ReplaceAll"));
    search->addSeparator();
    search->addAction(findActionByName("ToggleBookmark"));
    search->addAction(findActionByName("NextBookmark"));
    search->addAction(findActionByName("PreviousBookmark"));
    search->addAction(findActionByName("ClearBookmarks"));
    search->addSeparator();
    search->addAction(findActionByName("FindLine"));

    view->addAction(findActionByName("ShowToolbar"));
    view->addAction(findActionByName("ShowStatusBar"));
    view->addSeparator();
    QMenu *syntax = view->addMenu(tr("&Syntax && Features"));
    syntax->addAction(findActionByName("SyntaxPlain"));
    syntax->addSeparator();
    syntax->addAction(findActionByName("SyntaxCPP"));
    syntax->addAction(findActionByName("SyntaxHTML"));
    QMenu *le = view->addMenu(tr("Line Format"));
    le->addAction(findActionByName("UnixLineEnding"));
    le->addAction(findActionByName("MacLineEnding"));
    le->addAction(findActionByName("WinLineEnding"));
    view->addSeparator();
    view->addAction(findActionByName("ShowLineNumbers"));
    view->addAction(findActionByName("VisualizeWhiteSpace"));
    view->addAction(findActionByName("WordWrap"));
    view->addSeparator();
    view->addAction(findActionByName("FoldBlock"));
    view->addAction(findActionByName("FoldSelection"));
    view->addAction(findActionByName("Unfold"));
    view->addSeparator();
    view->addAction(findActionByName("Colors"));
    view->addAction(findActionByName("Fonts"));

    tools->addAction(findActionByName("CheckSpelling"));
    tools->addAction(findActionByName("PreviewInBrowser"));
    tools->addSeparator();
    tools->addAction(findActionByName("OpenTerminal"));
#ifdef Q_WS_WIN
    tools->addAction(findActionByName("OpenPowerShell"));
#endif
    tools->addSeparator();
    tools->addAction(findActionByName("SnippetManager"));
    tools->addAction(findActionByName("ConfigureUserTools"));
    //tools->addAction(findActionByName("SCMAdd"));
    //tools->addAction(findActionByName("SCMLog"));
    //tools->addAction(findActionByName("SCMBlame"));
#ifndef Q_WS_MAC
    tools->addSeparator();
#endif
    tools->addAction(findActionByName("Preferences"));


#ifdef Q_WS_MAC
    window->addAction(findActionByName("Minimize"));
    window->addAction(findActionByName("Zoom"));
    window->addSeparator();
#endif
    window->addAction(findActionByName("SelectNextTab"));
    window->addAction(findActionByName("SelectPreviousTab"));

#ifdef Q_WS_MAC
    window->addSeparator();
    setWindowMenuMac(window);
#endif

    help->addAction(findActionByName("Help"));
    help->addAction(findActionByName("Acknowledgements"));
    help->addAction(findActionByName("Update"));
#ifndef Q_WS_MAC
    help->addSeparator();
#endif
    //help->addAction(findActionByName("Licensing"));
    help->addAction(findActionByName("About"));

    m_menubar->setContextMenuPolicy(Qt::PreventContextMenu);
    setMenuBar(m_menubar);
}

void EditorWindow::createToolBar()
{
#ifdef Q_WS_MAC
    MacToolBarItem* newDocument = m_toolbar->addItemByAction(findActionByName("NewDocument"));
    MacToolBarItem* open = m_toolbar->addItemByAction(findActionByName("Open"));
    MacToolBarItem* save = m_toolbar->addItemByAction(findActionByName("Save"));
    MacToolBarItem* cut = m_toolbar->addAllowedItemByAction(findActionByName("Cut"));
    MacToolBarItem* copy = m_toolbar->addAllowedItemByAction(findActionByName("Copy"));
    MacToolBarItem* paste = m_toolbar->addAllowedItemByAction(findActionByName("Paste"));
    MacToolBarItem* undo = m_toolbar->addAllowedItemByAction(findActionByName("Undo"));
    MacToolBarItem* redo = m_toolbar->addAllowedItemByAction(findActionByName("Redo"));
    MacToolBarItem* print = m_toolbar->addItemByAction(findActionByName("Print"));
    connect(newDocument, SIGNAL(activated()),
            findActionByName("NewDocument"), SLOT(trigger()));
    connect(open, SIGNAL(activated()),
            findActionByName("Open"), SLOT(trigger()));
    connect(save, SIGNAL(activated()),
            findActionByName("Save"), SLOT(trigger()));
    connect(cut, SIGNAL(activated()),
            findActionByName("Cut"), SLOT(trigger()));
    connect(copy, SIGNAL(activated()),
            findActionByName("Copy"), SLOT(trigger()));
    connect(paste, SIGNAL(activated()),
            findActionByName("Paste"), SLOT(trigger()));
    connect(undo, SIGNAL(activated()),
            findActionByName("Undo"), SLOT(trigger()));
    connect(redo, SIGNAL(activated()),
            findActionByName("Redo"), SLOT(trigger()));
    connect(print, SIGNAL(activated()),
            findActionByName("Print"), SLOT(trigger()));

    m_toolbar->addStandardItem(MacToolBarItem::FlexibleSpace);
    m_toolbar->addAllowedStandardItem(MacToolBarItem::Space);

    SearchWidget *sf = new SearchWidget(0);
    sf->setMaximumHeight(60);
    sf->setMaximumWidth(300);
    m_toolbar->insertSearchWidget(sf);
    m_toolbar->attachToWindow(this->windowHandle());

    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    bool showToolbar = settings.value("ShowToolbar", true).toBool();
    if(!showToolbar)
        findActionByName("ShowToolbar")->setText(tr("Show Toolbar"));
    else
        findActionByName("ShowToolbar")->setText(tr("Hide Toolbar"));

    // On MacOS we let NSToolbar handle the setting of visiblity from preference
#else

#ifdef Q_WS_WIN
    if(IsWindowsVersionOrGreater(10,0,0))
    {
        QPalette p = m_toolbar->palette();
        p.setColor(QPalette::Window, QColor(Qt::white));
        p.setColor(QPalette::Button, QColor(Qt::white));
        m_toolbar->setPalette(p);
        m_toolbar->setAutoFillBackground(true);
    }

    m_toolbar->setIconSize(QSize(24,24));
#endif

    m_toolbar->setWindowTitle(tr("Toolbar"));
    m_toolbar->setFloatable(false);
    m_toolbar->setMovable(false);
    m_toolbar->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    addToolBar(m_toolbar);

    m_toolbar->addAction(findActionByName("NewDocument"));
    m_toolbar->addAction(findActionByName("Open"));
    m_toolbar->addAction(findActionByName("Save"));
    m_toolbar->addAction(findActionByName("CloseDocument"));
    m_toolbar->addSeparator();

    m_toolbar->addAction(findActionByName("Print"));
    m_toolbar->addAction(findActionByName("PrintPreview"));
    m_toolbar->addSeparator();
    m_toolbar->addAction(findActionByName("Cut"));
    m_toolbar->addAction(findActionByName("Copy"));
    m_toolbar->addAction(findActionByName("Paste"));
    m_toolbar->addSeparator();
    m_toolbar->addAction(findActionByName("Undo"));
    m_toolbar->addAction(findActionByName("Redo"));
    m_toolbar->addSeparator();
    m_toolbar->addAction(findActionByName("FindNext"));
    m_toolbar->addAction(findActionByName("Replace"));
    m_toolbar->addSeparator();
    m_toolbar->addAction(findActionByName("CheckSpelling"));
    m_toolbar->addSeparator();
    m_toolbar->addAction(findActionByName("Help"));

    m_toolbar_stretcher->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    m_toolbar->addWidget(m_toolbar_stretcher);
    m_searchbox->setPlaceholderText(tr("Quick Find"));
    m_searchbox->setClearButtonEnabled(true);
    m_searchbox->setFixedHeight(24);
    m_toolbar->addWidget(m_searchbox);
    m_toolbar->setContextMenuPolicy(Qt::PreventContextMenu);

    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    bool showToolbar = settings.value("ShowToolbar", true).toBool();
    findActionByName("ShowToolbar")->setChecked(showToolbar);
    m_toolbar->setVisible(showToolbar);
#endif
}

void EditorWindow::createStatusBar()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    bool showStatus = settings.value("ShowStatusBar", QVariant(true)).toBool();

    m_status->setAccessibleName("Status Bar");
    m_status->setAccessibleDescription("The application status bar.");
    setStatusBar(m_status);
#ifdef Q_WS_MAC
    // TODO: Elimiante style sheet
    m_status->setStyleSheet("background-color: rgb(235, 235, 235);");
    QFont f = m_status->font();
    f.setPointSize(10);
    m_status->setFont(f);
    m_col_ln->setFont(f);
    m_encoding->setFont(f);
    m_lineendings->setFont(f);
    m_language->setFont(f);
#endif
#ifndef Q_WS_MAC
    QLabel *space = new QLabel(tr("Press %1 for help.").arg(QKeySequence(QKeySequence::HelpContents).toString()));
    m_status->addWidget(space, 1);
#endif
    m_col_ln->setText(tr("L: %1 C %2").arg("1").arg("1"));
    m_col_ln->setAlignment(Qt::AlignCenter);
    m_col_ln->setMinimumWidth(80);

    m_status->addPermanentWidget(m_col_ln);

    m_language->setAlignment(Qt::AlignCenter);
    m_language->setMinimumWidth(80);
    m_status->addPermanentWidget(m_language);
    m_encoding->setAlignment(Qt::AlignCenter);
    m_encoding->setMinimumWidth(80);
    m_status->addPermanentWidget(m_encoding);
    m_lineendings->setAlignment(Qt::AlignCenter);
    m_lineendings->setMinimumWidth(80);
    m_status->addPermanentWidget(m_lineendings);

    findActionByName("ShowStatusBar")->setChecked(showStatus);
    if(showStatus)
    {
        m_status->setVisible(true);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowStatusBar");
        s->setText(tr("Hide &Status Bar"));
#endif
    }
    else
    {
        m_status->setVisible(false);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowStatusBar");
        s->setText(tr("Show &Status Bar"));
#endif
    }
}

void EditorWindow::showInFileManager()
{
#ifdef Q_WS_WIN
    qDebug() << "Attempting ShellExecute";
    QString launch("/select,\"%1\"");
    launch = launch.arg(m_active_document->fileName());
    ShellExecuteW(NULL, L"open", L"explorer.exe", launch.toStdWString().data(), NULL, SW_SHOWNORMAL);
#endif
}

void EditorWindow::showAbout()
{
    // don't connect slot directly
    // parent *required* for mac menu bar inheritence
    myApp->aboutApplication(this);
}

void EditorWindow::showPreferences()
{
    // don't connect slot directly
    // parent *required* for mac menu bar inheritence
    myApp->showPreferencesDialog(this);
}

void EditorWindow::newDocument()
{
    if(m_tabwidget)
        m_tabwidget->newDocument();
}

TabWidget* EditorWindow::tabWidget()
{
    if(m_tabwidget)
        return m_tabwidget;

    return NULL;
}

void EditorWindow::tabCloseRequested(int idx)
{
    Document *d = m_tabwidget->documentForIndex(idx);

    if(d == NULL)
        return;

    // if we are not modiifed, just close up without delay.
    if(!d->isModified())
    {
        d->closeDocument();
        if(m_active_document == d)
            m_active_document = Q_NULLPTR;
    }
    else
    {
        int result = promptToSaveChanges(idx);
        if(result == QMessageBox::Cancel)
            return; // don't do anything further

        if(result == QMessageBox::Discard)
        {
            d->closeDocument();
            if(m_active_document == d)
                m_active_document = Q_NULLPTR;
        }

        if(result == QMessageBox::Save)
        {
            saveFile();
            if(!d->isModified() && !d->isUntitled())
            {
                d->closeDocument();
                if(m_active_document == d)
                    m_active_document = Q_NULLPTR;
            }
        }
    }
}

void EditorWindow::tabIndexChanged(int idx)
{
    QAction *cut = findActionByName("Cut");
    QAction *copy = findActionByName("Copy");
    cut->setEnabled(false);
    copy->setEnabled(false);

    disconnect(this, SLOT(selectionChanged()));
    disconnect(this, SLOT(setWindowModified(bool)));
    disconnect(this, SLOT(updateLineColumn()));
    disconnect(this, SLOT(updateLanguageFeature()));
    disconnect(this, SLOT(updateLineEnding()));
    disconnect(this, SLOT(updateNameAndSaveStatus()));

    Document *d = m_tabwidget->documentForIndex(idx);
    if(!d)
    {
        if(tabWidget()->count() == 0)
            close();

        return;
    }

    setWindowFilePath(d->displayName());
    setWindowModified(d->isModified());
    connect(d, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(d, SIGNAL(undoAvailable(bool)),
            this, SLOT(setUndoAvailable(bool)));
    connect(d, SIGNAL(redoAvailable(bool)),
            this, SLOT(setRedoAvailable(bool)));
    connect(d, SIGNAL(cursorPositionChanged()), this,
            SLOT(updateLineColumn()));

    connect(d, SIGNAL(modificationChanged(bool)), this,
            SLOT(setWindowModified(bool)));

    connect(d, SIGNAL(languageFeaturesChanged()),
            this, SLOT(updateLanguageFeature()));

    connect(d, SIGNAL(lineEndingsChanged()),
            this, SLOT(updateLineEnding()));

    connect(d, SIGNAL(updateNameAndSaveStatus()),
            this, SLOT(updateNameAndSaveStatus()));

    clipboardDataChanged();
    selectionChanged();
    canUndoRedo();
    m_encoding->setText(d->encoding());
    if(d->isUntitled())
        setWindowFilePath(tr("Untitled"));
    else
        setWindowFilePath(d->fileName());

    updateLineColumn();
    m_encoding->setText(d->encoding());

    updateLineEnding();
    updateLanguageFeature();

#ifdef Q_WS_WIN
    //QtWin::taskbarActivateTab(d->editorWidget());
#endif

#ifndef Q_WS_MAC
    disconnect(this, SLOT(updateFindString(QString)));
    disconnect(m_searchbox, SIGNAL(textEdited(QString)));
    connect(m_searchbox, SIGNAL(textEdited(QString)),
            d, SLOT(toolbarQuickFindChanged(QString)));
    connect(d, SIGNAL(findStringChanged(QString)),
            this, SLOT(updateFindString(QString)));

    m_searchbox->setText(d->findSettings().find_str);
#endif
    if(m_active_document)
        m_active_document->deactivatePalettes();
    d->activatePalettes();

    QAction *reveal = findActionByName("RevealFile");
    if(d->isUntitled())
        reveal->setEnabled(false);
    else
        reveal->setEnabled(true);

    // Always last so we can properly work with the *old*
    // m_active_document above.
    m_active_document = d;
}

void EditorWindow::updateNameAndSaveStatus()
{
    QAction *reveal = findActionByName("RevealFile");
    if(m_active_document->isUntitled())
        reveal->setEnabled(false);
    else
        reveal->setEnabled(true);
}

void EditorWindow::updateFindString(const QString &findString)
{
#ifndef Q_WS_MAC
    m_searchbox->setText(findString);
#endif
}

void EditorWindow::setUndoAvailable(bool available)
{
    QAction *undo = findActionByName("Undo");
    undo->setEnabled(available);
}

void EditorWindow::setRedoAvailable(bool available)
{
    QAction *redo = findActionByName("Redo");
    redo->setEnabled(available);
}

void EditorWindow::canUndoRedo()
{
    Editor *e = m_tabwidget->currentDocument()->editorWidget();
    setUndoAvailable(e->document()->isUndoAvailable());
    setRedoAvailable(e->document()->isRedoAvailable());
}

void EditorWindow::selectionChanged()
{
    QAction *cut = findActionByName("Cut");
    QAction *copy = findActionByName("Copy");
    Editor *e = m_tabwidget->currentDocument()->editorWidget();
    if(e->textCursor().hasSelection())
    {
        cut->setEnabled(true);
        copy->setEnabled(true);
    }
    else
    {
        cut->setEnabled(false);
        copy->setEnabled(false);
    }
}

void EditorWindow::clipboardDataChanged()
{
    QAction *paste = findActionByName("Paste");
    QClipboard *cb = qApp->clipboard();
    if(cb->mimeData()->hasText())
        paste->setEnabled(true);
    else
        paste->setEnabled(false);
}

void EditorWindow::showOpenFileDialog()
{
    QFileDialog d;
    d.setParent(this);
    d.setWindowTitle(tr("Open Text File"));
    d.setDefaultSuffix("txt");

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

    d.setNameFilters(filters);

    QStringList files;
    if(d.exec())
    {
        Document *doc = NULL;
        bool closeDoc = false;
        files = d.selectedFiles();
        // if we only have a single tab, and it's
        // untitled and unmodified, close it
        if(m_tabwidget->count() == 1)
        {
            doc = m_tabwidget->currentDocument();
            if(doc->isUntitled() && !doc->isModified())
                closeDoc = true;
        }

        foreach(QString file, files)
            myApp->openFile(file);

        if(closeDoc)
            doc->requestTabClose();
    }
}

void EditorWindow::saveFile()
{
    if(m_tabwidget->currentDocument()->isUntitled())
        return saveFileAs();

    m_tabwidget->currentDocument()->saveFile();
}

void EditorWindow::saveFileAs()
{
    QFileDialog *d = new QFileDialog(this);

    d->setWindowTitle(tr("Save File As"));
    d->setDefaultSuffix("txt");

    d->setAcceptMode(QFileDialog::AcceptSave);

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

    d->setNameFilters(filters);

    d->setWindowModality(Qt::WindowModal);
    d->setParent(this, Qt::Sheet);

    connect(d, SIGNAL(finished(int)), d, SLOT(deleteLater()));

    d->exec();

    if(!d->selectedFiles().isEmpty())
    {
        QString fn = d->selectedFiles().first();
        m_tabwidget->currentDocument()->saveFile(fn);
    }
}

void EditorWindow::closeTab()
{
    Document *d = m_tabwidget->currentDocument();

    if(d == NULL)
        return;

    // if we are not modiifed, just close up without delay.
    if(!d->isModified())
    {
        d->closeDocument();
        if(m_active_document == d)
            m_active_document = Q_NULLPTR;
    }
    else
    {
        int result = promptToSaveChanges();
        if(result == QMessageBox::Cancel)
            return; // don't do anything further

        if(result == QMessageBox::Discard)
        {
            d->closeDocument();
            if(m_active_document == d)
                m_active_document = Q_NULLPTR;
        }

        if(result == QMessageBox::Save)
        {
            saveFile();
            if(!d->isModified() && !d->isUntitled())
            {
                d->closeDocument();
                if(m_active_document == d)
                    m_active_document = Q_NULLPTR;
            }
        }
    }
}

void EditorWindow::print()
{
    QPrintDialog *dlg = new QPrintDialog(m_tabwidget->currentDocument()->printer(), this);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowFlags(Qt::Sheet);
    connect(dlg, SIGNAL(finished(int)), dlg, SLOT(deleteLater()));
    connect(dlg, SIGNAL(accepted(QPrinter*)), this, SLOT(printExecute(QPrinter*)));
    dlg->show();
}

void EditorWindow::printExecute(QPrinter *p)
{
    m_tabwidget->currentDocument()->editorWidget()->print(p);
}

void EditorWindow::pageSetup()
{
    QPageSetupDialog* dlg = new QPageSetupDialog(m_tabwidget->currentDocument()->printer(), this);
    dlg->setWindowFlags(Qt::Sheet);
    dlg->setWindowModality(Qt::WindowModal);
    connect(dlg, SIGNAL(finished(int)), dlg, SLOT(deleteLater()));

    dlg->show();
}
void EditorWindow::printPreview()
{
    QPrintPreviewDialog *dlg = new QPrintPreviewDialog(m_tabwidget->currentDocument()->printer(), this);
    connect(dlg, SIGNAL(paintRequested(QPrinter*)), this, SLOT(previewPrint(QPrinter*)));
    dlg->exec();
    dlg->deleteLater();
}
void EditorWindow::previewPrint(QPrinter *p)
{
    m_tabwidget->currentDocument()->editorWidget()->print(p);
}

void EditorWindow::undo()
{
    m_tabwidget->currentDocument()->editorWidget()->undo();
}

void EditorWindow::redo()
{
    m_tabwidget->currentDocument()->editorWidget()->redo();
}

void EditorWindow::cut()
{
    m_tabwidget->currentDocument()->editorWidget()->cut();
}

void EditorWindow::copy()
{
    m_tabwidget->currentDocument()->editorWidget()->copy();
}

void EditorWindow::paste()
{
    m_tabwidget->currentDocument()->editorWidget()->paste();
}

void EditorWindow::selectAll()
{
    m_tabwidget->currentDocument()->editorWidget()->selectAll();
}

void EditorWindow::selectWord()
{
    m_tabwidget->currentDocument()->editorWidget()->selectWord();
}

void EditorWindow::selectLine()
{
    m_tabwidget->currentDocument()->editorWidget()->selectLine();
}

void EditorWindow::deleteSelection()
{
    m_tabwidget->currentDocument()->editorWidget()->textCursor().removeSelectedText();
}

void EditorWindow::aboutToShowEncodingMenu()
{
    QMenu *m = qobject_cast<QMenu*>(sender());
    m->clear();

    QMap<QString, QMenu*> menus;
    Application* a = qobject_cast<Application*>(qApp);
    foreach(QString category, a->encodingCategories())
    {
        QMenu *m2 = m->addMenu(category);
        m2->setObjectName(category);
        menus.insert(category, m2);
    }

    QMap<QString, QString> encodings = a->encodings();
    foreach(QString enc, encodings.keys())
    {
        QMenu *m3 = menus.value(a->categoryForEncoding(enc));
        m3->addAction(encodings.value(enc));
    }
}

void EditorWindow::configureColors()
{
    ColorDialog *cd = new ColorDialog(this);
    connect(cd, SIGNAL(finished(int)), cd, SLOT(deleteLater()));
    cd->open();
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
    uint unsaved = unsavedDocumentCount();

    if(unsaved == 0)
    {
        savePosition();
        event->accept();
        return;
    }

    if(unsaved == 1)
    {
        // let's move over to the item in question
        for(int i = 0; i < tabWidget()->count(); i++)
        {
            if(tabWidget()->documentForIndex(i)->isModified())
            {
                tabWidget()->setCurrentIndex(i);
                break;
            }
        }

        // We can use a normal prompt
        int result = promptToSaveChanges();
        switch(result)
        {
        case QMessageBox::Cancel:
            event->ignore();
            return;
        case QMessageBox::Save:

        case QMessageBox::Discard:
        default:
            savePosition();
            event->accept();
            return;
        }
    }
}

uint EditorWindow::unsavedDocumentCount()
{
    uint unsaved = 0;

    for(int i = 0; i < tabWidget()->count(); i++)
    {
        if(tabWidget()->documentForIndex(i)->isModified())
            unsaved++;
    }

    return unsaved;
}

void EditorWindow::configureFont()
{
    QFontDialog f(this);
#ifdef Q_WS_MAC
    f.setWindowFlag(Qt::Sheet, true);
#endif
    f.setOptions(QFontDialog::MonospacedFonts);
    f.setCurrentFont(m_tabwidget->currentDocument()->editorWidget()->font());
    if(f.exec() == QDialog::Accepted)
    {
        QSettings settings("originull", "startext");
        settings.beginGroup("Preferences");
        settings.setValue("DefaultFontFamily", f.currentFont().toString());
        settings.setValue("DefaultFontSize", f.currentFont().pointSize());
        myApp->updatePreferences();
    }
}

void EditorWindow::checkSpelling()
{
    if(!myApp->checkSpellCheck())
        return;

    tabWidget()->currentDocument()->checkSpelling();
}

int EditorWindow::promptToSaveChanges(int tab_id)
{
    Document *d;

    if(tab_id == -1)
        d = m_tabwidget->currentDocument();
    else
        d = m_tabwidget->documentForIndex(tab_id);

    if(!d)
        return -99;

    QMessageBox m(this);
    m.setWindowFlags(Qt::Sheet);
    m.setIconPixmap(myApp->windowIcon().pixmap(64, 64));
#ifdef Q_WS_WIN
    m.setIcon(QMessageBox::Warning);
    QLabel *w = m.findChild<QLabel*>("qt_msgbox_label",
                               Qt::FindDirectChildrenOnly);
    QFont f("Seoge UI", 12);
    QPalette p = w->palette();
    p.setColor(QPalette::Foreground, QColor(0, 51, 153));
    w->setFont(f);
    w->setPalette(p);
#endif
    m.setText(tr("Do you want to save the changes you made in the document \"%1\"?").arg(d->displayName()));
    m.setInformativeText(tr("Your changes will be lost if you don't save them."));
    m.setStandardButtons(QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Discard);
    m.setWindowModality(Qt::WindowModal);
    return m.exec();
}

void EditorWindow::unimplemented()
{
    QAction *a = qobject_cast<QAction*>(sender());
    QMessageBox m(this);
    m.setWindowFlags(Qt::Sheet);

    m.setText(tr("The feature you requested is unimplemented."));
    m.setInformativeText(tr("It will be coming soon!\n\nAction: %1.").arg(a->text()));
    m.setStandardButtons(QMessageBox::Ok);
    m.setIconPixmap(myApp->windowIcon().pixmap(64, 64));
    m.setWindowModality(Qt::WindowModal);
    m.exec();
}

void EditorWindow::changeSyntax()
{
    QAction *a = qobject_cast<QAction*>(sender());

    if(a->objectName() == QString("SyntaxCPP"))
        m_tabwidget->currentDocument()->setLanguageFeatures(CPP);

    if(a->objectName() == QString("SyntaxHTML"))
        m_tabwidget->currentDocument()->setLanguageFeatures(HTML);

    // we always fall back to a plain text syntax
    if(a->objectName() == QString("SyntaxPlain"))
        m_tabwidget->currentDocument()->setLanguageFeatures(PlainText);
}

void EditorWindow::windowStateChanged(Qt::WindowState state)
{
#ifdef Q_WS_MAC
/*     if(state == Qt::WindowFullScreen)
        setTabBarAccessory(m_tabwidget);
    else
        removeTabBarAccessory(m_tabwidget); */
#endif
    Q_UNUSED(state);
}

void EditorWindow::updateLineColumn()
{
    QTextCursor c =
            m_tabwidget->currentDocument()->editorWidget()->textCursor();

    int col = c.columnNumber();

    c.movePosition(QTextCursor::StartOfLine);

    int l = 1;
    while(c.positionInBlock() > 0)
    {
        c.movePosition(QTextCursor::Up);
        l++;
    }

    QTextBlock b = c.block().previous();

    while(b.isValid())
    {
        l += b.lineCount();
        b = b.previous();
    }

    m_col_ln->setText(tr("L: %1 C: %2").arg(QString::number(l)).arg(col++));
}

void EditorWindow::triggerToolbar()
{
    QSettings settings("originull", "startext");

    settings.beginGroup("Preferences");

    if(!m_toolbar->isVisible())
    {
        m_toolbar->setVisible(true);
        settings.setValue("ShowToolbar", true);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowToolbar");
        s->setText(tr("Hide &Toolbar"));
#endif
    }
    else
    {
        m_toolbar->setVisible(false);
        settings.setValue("ShowToolbar", false);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowToolbar");
        s->setText(tr("Show &Toolbar"));
#endif
    }
}

void EditorWindow::triggerStatusBar()
{
    QSettings settings("originull", "startext");

    settings.beginGroup("Preferences");

    if(!m_status->isVisible())
    {
        m_status->setVisible(true);
        settings.setValue("ShowStatusBar", true);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowStatusBar");
        s->setText(tr("Hide &Status Bar"));
#endif
    }
    else
    {
        m_status->setVisible(false);
        settings.setValue("ShowStatusBar", false);
#ifdef Q_WS_MAC
        QAction *s = findActionByName("ShowStatusBar");
        s->setText(tr("Show &Status Bar"));
#endif
    }
}

void EditorWindow::triggerLineNumbers()
{
    QSettings settings("originull", "startext");

    settings.beginGroup("Preferences");
    bool sl = settings.value("ShowLineNumbers", true).toBool();
    if(sl)
        settings.setValue("ShowLineNumbers", false);
    else
        settings.setValue("ShowLineNumbers", true);

    myApp->requestPreferencesUpdate();
}

void EditorWindow::triggerVisualizeWhitespace()
{
    QSettings settings("originull", "startext");

    settings.beginGroup("Preferences");
    bool sl = settings.value("VisualizeWhiteSpace", false).toBool();
    if(sl)
        settings.setValue("VisualizeWhiteSpace", false);
    else
        settings.setValue("VisualizeWhiteSpace", true);

    myApp->requestPreferencesUpdate();
}

void EditorWindow::updateLineNumberAction()
{
    QAction *s = findActionByName("ShowLineNumbers");
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    bool sl = settings.value("ShowLineNumbers", true).toBool();

    if(!sl)
    {
        s->setChecked(false);
#ifdef Q_WS_MAC
        s->setText(tr("Show Line &Numbers"));
#endif
    }
    else
    {
        s->setChecked(true);
#ifdef Q_WS_MAC
        s->setText(tr("Hide Line &Numbers"));
#endif
    }
}

void EditorWindow::updateWhitespaceAction()
{
    QAction *s = findActionByName("VisualizeWhiteSpace");
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    bool sl = settings.value("VisualizeWhiteSpace", true).toBool();

    if(!sl)
    {
        s->setChecked(false);
#ifdef Q_WS_MAC
        s->setText(tr("Show &Whitespace Visualization"));
#endif
    }
    else
    {
        s->setChecked(true);
#ifdef Q_WS_MAC
        s->setText(tr("Hide &Whitespace Visualization"));
#endif
    }
}

void EditorWindow::updatePreferences()
{
    updateLineNumberAction();
    updateWhitespaceAction();
}

QList<Document*> EditorWindow::documents()
{
    QList<Document*> l;

    for(int i = 0; i <= m_tabwidget->count(); i++)
        l.append(m_tabwidget->documentForIndex(i));

    return l;
}

void EditorWindow::focusDocument(Document *d)
{
    for(int i = 0; i <= m_tabwidget->count(); i++)
    {
        if(m_tabwidget->documentForIndex(i) == d)
            m_tabwidget->setCurrentIndex(i);
    }
}

void EditorWindow::focusInEvent(QFocusEvent *event)
{
    myApp->setCurrentWindow(this);
    QMainWindow::focusInEvent(event);
}

void EditorWindow::insertPageBreak()
{
    m_tabwidget->currentDocument()->editorWidget()->insertPageBreak();
}

void EditorWindow::insertFileContents()
{
    //m_tabwidget->currentDocument()->editorWidget()->insertPageBreak();
}

void EditorWindow::insertDirectoryList()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"));
    m_tabwidget->currentDocument()->editorWidget()->insertDirectoryList(dir);
}

void EditorWindow::setWindowTitle(const QString &title)
{
#ifdef Q_WS_MAC
    //m_titlebar->setTitle(title);
#endif
    QMainWindow::setWindowTitle(title);
}

void EditorWindow::setWindowFilePath(const QString &filePath)
{
    QMainWindow::setWindowFilePath(filePath);
#ifdef Q_WS_MAC
    //m_titlebar->setTitle(filePath);
#endif
}

void EditorWindow::findAndReplace()
{
    m_tabwidget->currentDocument()->showFindDialog();
}

void EditorWindow::updateLanguageFeature()
{
    //uncheckAllLanguageFeatures();
    Document *d = m_tabwidget->currentDocument();
    switch(d->languageFeatureType())
    {
        case CPP:
            findActionByName("SyntaxCPP")->setChecked(true);
            m_language->setText("C/C++");
            break;
        case HTML:
            findActionByName("SyntaxHTML")->setChecked(true);
            m_language->setText("HTML");
            break;
        case PlainText:
            findActionByName("SyntaxPlain")->setChecked(true);
            m_language->setText(tr("Plain Text"));
            break;
        default:
            break;
    }
}

void EditorWindow::updateLineEnding()
{
    Document *d = tabWidget()->currentDocument();
    switch(d->lineEndings())
    {
    case Unix:
        m_lineendings->setText(tr("Unix (LF)"));
        findActionByName("UnixLineEnding")->setChecked(true);
        break;
    case Windows:
        m_lineendings->setText(tr("Windows (CR+LF)"));
        findActionByName("WinLineEnding")->setChecked(true);
        break;
    case Mac:
        m_lineendings->setText(tr("Mac (Old) (CR)"));
        findActionByName("MacLineEnding")->setChecked(true);
        break;
    default:
        m_lineendings->setText(tr("Unknown"));
        break;
    }
}

void EditorWindow::uncheckAllLanguageFeatures()
{
    foreach(QAction *a, m_language_feature_actions)
        a->setChecked(false);
}

void EditorWindow::buildMRUMenu()
{
    QMenu *m = qobject_cast<QMenu*>(sender());
    if(!m_mru_list.isEmpty())
        destroyMRUMenu();

    MRUFiles files = myApp->mruList();
    QMapIterator<time_t, QString> i(files);
    i.toBack();
    while(i.hasPrevious())
    {
        i.previous();
        QString fileName = i.value();
        QAction *a = new QAction(fileName);

        // store a copy of the full file
        a->setProperty("InternalQActionFileName", i.value());
        a->setText(fileName);

        m->addAction(a);
        //connect(a, SIGNAL(triggered(bool)), this, SLOT(openMRUItem()));
        m_mru_list.append(a);
    }
}

void EditorWindow::openMRUItem(QAction *item)
{
    QString fileName =
            item->property("InternalQActionFileName").toString();

    if(fileName.isEmpty() || fileName.isNull())
    {
        qDebug() << "EditorWindow::openMRUItem: false fileName";
        return;
    }

    myApp->openFile(fileName);
    destroyMRUMenu();
}

void EditorWindow::destroyMRUMenu()
{
    foreach(QAction *a, m_mru_list)
    {
        m_mru_list.removeOne(a);
        a->deleteLater();
    }
}

void EditorWindow::promptForLineNumber()
{
    QInputDialog box(this);
    box.setWindowModality(Qt::WindowModal);
    box.setWindowTitle(tr("Find Line by Line Number"));
    box.setLabelText(tr("Line Number:"));
    box.setIntMinimum(0);
    box.setIntMaximum(m_tabwidget->currentDocument()->
                      editorWidget()->document()->lineCount());
    box.setInputMode(QInputDialog::IntInput);

    if(box.exec() == QMessageBox::Accepted)
    {
        QTextCursor c = m_tabwidget->currentDocument()->
                              editorWidget()->textCursor();

        int val = box.intValue()-1;
        QTextBlock b = m_tabwidget->currentDocument()->editorWidget()->
                document()->findBlockByLineNumber(val);

        c.setPosition(b.position());
        m_tabwidget->currentDocument()->editorWidget()->setFocus();
        m_tabwidget->currentDocument()->editorWidget()->setTextCursor(c);
    }
}
