/******************************************************************************

 *
 * document.cc - primary document controller class
 *
 * this contains the primary QTextDocument (m_qdocument)
 *               the primary editor widget (m_editor - editor.cc)
 *               nuts, bolts, properties, variables etc
 *               palettes! find & replace (m_find_dlg - finddialog.cc)
 *                         spelling (m_spelling - spellcheckdialog.cc)
 *
 * palettes contained within should only be for document specific functions,
 * ie: when you want to persist the settings for each document (changes on tab
 * change, etc).  When you want to persist globally, define at the application
 * or window level.
 *
******************************************************************************/

#include "document.h"
#include <QFlag>

Document::Document(QObject *parent)
    : QObject(parent),
      m_editor(new Editor),
      m_spelling(Q_NULLPTR),
      m_show_spelling(false),
      m_printer(new QPrinter(QPrinter::HighResolution)),
      m_qdocument(new QTextDocument(this)),
      m_features(new PlainFeatures(this)),
      m_filetype(PlainText),
      m_untitled(true),
      m_lineending(Unix),
      m_encoding(Q_NULLPTR),
      m_find_dlg(Q_NULLPTR),
      m_show_find(false)
{
    QPlainTextDocumentLayout *layout = new QPlainTextDocumentLayout(m_qdocument);
    m_qdocument->setDocumentLayout(layout);

    m_editor->setAccessibleDescription(tr("Edit buffer window"));
    m_editor->setAppDocument(this);
    m_editor->setDocument(m_qdocument);
    m_qdocument->setMetaInformation(QTextDocument::DocumentTitle, tr("Untitled"));
    connect(m_editor, SIGNAL(selectionChanged()),
            this, SIGNAL(selectionChanged()));
    connect(m_editor, SIGNAL(copyAvailable(bool)),
            this, SIGNAL(copyAvailable(bool)));
    connect(m_editor, SIGNAL(cursorPositionChanged()),
            this, SIGNAL(cursorPositionChanged()));

    connect(m_qdocument, SIGNAL(undoAvailable(bool)),
            this, SIGNAL(undoAvailable(bool)));
    connect(m_qdocument, SIGNAL(redoAvailable(bool)),
            this, SIGNAL(redoAvailable(bool)));

    connect(m_qdocument, SIGNAL(modificationChanged(bool)),
            this, SIGNAL(modificationChanged(bool)));

    connect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));

    // TODO: default preference
    m_encoding = QTextCodec::codecForName("utf-8");
    m_filename = QString("");

    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    uint le_val = settings.value("DefaultLineEnding", 0).toUInt();
    switch(le_val)
    {
    case 1:
        m_lineending = Windows;
        break;
    case 2:
        m_lineending = Mac;
        break;
    case 0:
    default:
        m_lineending = Unix;
    }
    QFont font;
    font.setFamily(settings.value("DefaultFontFamily").toString());
    font.setPointSize(settings.value("DefaultFontSize").toUInt());
    if(m_qdocument)
        m_qdocument->setDefaultFont(font);

    //myApp->updatePreferences();
}

Document::~Document()
{
    m_editor->deleteLater();
    m_qdocument->deleteLater();

    destroyFindDialog();
    destroySpellingDialog();
    delete m_printer;
    disconnect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));
}

AbstractFeatures* Document::languageFeatures() const
{
    return m_features;
}

LanguageFeatureType Document::languageFeatureType() const
{
    return m_filetype;
}

void  Document::setFindSettings(FindSettings &f)
{
    m_findsettings = f;
}

FindSettings Document::findSettings()
{
    return m_findsettings;
}

LineEnding Document::lineEndings()
{
    return m_lineending;
}

void Document::setLineEndings(LineEnding e)
{
    m_lineending = e;
    //TODO: conversion or re-save?
    emit lineEndingsChanged();
}

bool Document::saveFile(QString filename)
{
    return saveToFile(filename);
}

QString Document::fileName()
{
    return m_filename;
}

QString Document::displayName()
{
    if(m_untitled)
        return tr("Untitled");
    else if(m_filename.isEmpty())
        return tr("Untitled");
    else
        return m_filename.split("/").last();
}

bool Document::isModified()
{
    return m_editor->document()->isModified();
}

bool Document::isUntitled()
{
    return m_untitled;
}

Editor* Document::editorWidget()
{
    return m_editor;
}

Document* Document::openFromFile(QString filename)
{
    Document *n = new Document;
    if(n->loadFromFile(filename))
        return n;

    delete n;
    return NULL;
}

LineEnding Document::determineLineEnding(QString b)
{
    int unxend = b.count('\n');
    int mac = b.count('\n');
    int win = b.count("\r\n");

    if((unxend == mac) == win)
        return Windows;

    if(win == 0)
    {
        if(unxend)
            return Unix;
        else
            return Mac;
    }

    /* fallback */
    /* TODO: set default preference? */

    return Unix;
}

bool Document::loadFromFile(QString filename)
{
    QFile file(filename);
    if(!file.exists())
        return false;

    if(file.open(QFile::ReadWrite))
    {
        QTextStream s(&file);
#if QT_VERSION < 0x060000
        s.codec()->setCodecForLocale(QTextCodec::codecForName("utf-8"));
        m_encoding = s.codec();
#endif
        QString b = s.readAll();
        m_lineending = determineLineEnding(b);
        m_qdocument->setPlainText(b);
        file.close();
        QUrl url;
        url.setScheme("file");
        url.setPath(m_filename);
        m_qdocument->setBaseUrl(url);
        m_filename = file.fileName();
        myApp->addToMRUFiles(file.fileName());
        m_untitled = false;
        m_qdocument->setMetaInformation(QTextDocument::DocumentTitle, displayName());
        m_qdocument->setModified(false);

        // set the cursor to top
        QTextCursor cursor = m_editor->textCursor();
        cursor.setPosition(0);
        m_editor->setTextCursor(cursor);
        QTimer::singleShot(0, m_editor, SLOT(setFocus()));
        return true;
    }

    return false;
}

bool Document::saveToFile(QString filename)
{
    if(filename.isEmpty())
        filename = m_filename;

    QFile file(filename);
    if(file.open(QFile::ReadWrite))
    {
        QTextStream s(&file);
        s << m_qdocument->toPlainText();
        s.flush();
        file.close();
        m_filename = file.fileName();
        m_untitled = false;
        m_qdocument->setModified(false);
        m_qdocument->setMetaInformation(QTextDocument::DocumentTitle, displayName());
        emit updateNameAndSaveStatus();
        return true;
    }

    return false;
}

QString Document::encoding()
{
    return m_encoding->name();
}

QPrinter* Document::printer()
{
    return m_printer;
}

void Document::closeDocument()
{
    emit requestTabClose();
}

void Document::setLanguageFeatures(LanguageFeatureType f)
{
    if(m_features)
    {
        QObject *o = qobject_cast<QObject*>(m_features);
        o->deleteLater();
    }

    switch(f)
    {
        case CPP:
            m_features = new CFeatures(this);
            m_filetype = CPP;
            break;
        case HTML:
            m_features = new HtmlFeatures(this);
            m_filetype = HTML;
            break;
        case PlainText:
        default:
            m_features = new PlainFeatures(this);
            m_filetype = PlainText;
    }

    emit languageFeaturesChanged();
}

void Document::activatePalettes()
{
    // called when this document is brought into view
    // from EditorWindow

    if(m_spelling != Q_NULLPTR && m_show_spelling)
        m_spelling->show();

    if(m_find_dlg != Q_NULLPTR && m_show_find)
        m_find_dlg->show();
}

void Document::deactivatePalettes()
{
    if(m_spelling != Q_NULLPTR
            && m_show_spelling)
        m_spelling->hide();

    if(m_find_dlg != Q_NULLPTR && m_show_find)
        m_find_dlg->hide();
}

void Document::checkSpelling()
{
    if(!m_spelling)
        setupSpellingDialog();

    QTextCursor old = m_editor->textCursor();
    QTextCursor spellCursor(m_qdocument);
    SpellCheck *sc = myApp->spellChecker();

    while(!spellCursor.atEnd())
    {
        myApp->processEvents();
        spellCursor.movePosition(QTextCursor::EndOfWord,
                                 QTextCursor::KeepAnchor, 1);
        QString word = spellCursor.selectedText();

        while(!word.isEmpty()
              && !word.at(0).isLetter()
              && spellCursor.anchor() < spellCursor.position())
        {
            int pos = spellCursor.position();
            spellCursor.setPosition(spellCursor.anchor() + 1, QTextCursor::MoveAnchor);
            spellCursor.setPosition(pos, QTextCursor::KeepAnchor);
            word = spellCursor.selectedText();
        }

        if(!word.isEmpty() && !sc->checkWord(word))
        {
            // we have a mispelling!
            if(!m_spelling->isVisible())
                m_spelling->show();

            QTextCursor tmp(spellCursor);
            tmp.setPosition(spellCursor.anchor());
            editorWidget()->setTextCursor(tmp);
            editorWidget()->ensureCursorVisible();
            /* QTextEdit::ExtraSelection es;
            es.cursor = spellCursor;
            es.format = highlightFormat;
            QList<QTextEdit::ExtraSelection> esl;
            esl << es;
            editorWidget()->setExtraSelections(esl); */
            myApp->processEvents();

            SpellCheckAction a =
                    m_spelling->checkWord(word);

            if(a == Abort)
                return;

            if(a == ChangeWord)
                spellCursor.insertText(m_spelling->replacementWord());

        }
    }

    // end of the proc
    m_editor->setTextCursor(old);
    // destroy the dialog
    destroySpellingDialog();
    // show a complete message box
    QMessageBox box;
    box.setWindowModality(Qt::ApplicationModal);
    box.setWindowTitle(tr("Spelling Check"));
    box.setIcon(QMessageBox::Information);
#ifdef Q_WS_WIN
    //box.setStyleSheet("QLabel#qt_msgbox_label { font-size: 12pt; color: #003399; font: Seoge UI; }");
#endif
    box.setText(tr("The spelling check has completed."));
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

void Document::setupSpellingDialog()
{
    if(m_spelling)
        return;

    m_spelling = new SpellCheckDialog(editorWidget()->mainWindow());
}

void Document::destroySpellingDialog()
{
    if(!m_spelling)
        return;

    m_spelling->close();
    m_spelling->deleteLater();
}

// ****** BEGIN find features ******

void Document::setupFindDialog()
{
    if(m_find_dlg)
        return;

    m_find_dlg = new FindDialog(editorWidget()->mainWindow());
    connect(m_find_dlg, SIGNAL(findStringChanged(QString)),
            this, SLOT(dlgFindStringChanged(QString)));
    connect(m_find_dlg, SIGNAL(caseSensitiveChanged(bool)),
            this, SLOT(caseSensitiveChanged(bool)));
    connect(m_find_dlg, SIGNAL(wholeWordChanged(bool)),
            this, SLOT(wholeWordsChanged(bool)));
    connect(m_find_dlg, SIGNAL(wrapAroundChanged(bool)),
            this, SLOT(wrapAroundChanged(bool)));
    connect(m_find_dlg, SIGNAL(regexpChanged(bool)),
            this, SLOT(regexpChanged(bool)));

    connect(m_find_dlg, SIGNAL(finished(int)),
            this, SLOT(findPaletteFinished(int)));

    connect(m_find_dlg, SIGNAL(findNext()),
            this, SLOT(findNext()));
}

void Document::destroyFindDialog()
{
    if(!m_find_dlg)
        return;

    m_find_dlg->close();
    /* disconnect(m_find_dlg, SIGNAL(findStringChanged(QString)));
    disconnect(m_find_dlg, SIGNAL(caseSensitiveChanged(bool)));
    disconnect(m_find_dlg, SIGNAL(wholeWordChanged(bool)));
    disconnect(m_find_dlg, SIGNAL(wrapAroundChanged(bool)));
    disconnect(m_find_dlg, SIGNAL(regexpChanged(bool))); */

    m_find_dlg->deleteLater();
    m_find_dlg = Q_NULLPTR;
    m_show_find = false;
}

void Document::showFindDialog()
{
    if(!m_find_dlg)
        setupFindDialog();

    m_find_dlg->updateFindSettings(m_findsettings);
    m_find_dlg->show();
    m_show_find = true;
}

void Document::toolbarQuickFindChanged(const QString &findString)
{
    if(m_find_dlg)
        m_find_dlg->setFindString(findString);

    m_findsettings.find_str = findString;
}

void Document::dlgFindStringChanged(const QString &findString)
{
    m_findsettings.find_str = findString;
    emit findStringChanged(findString);
}

void Document::caseSensitiveChanged(bool enabled)
{
    m_findsettings.case_sensitive = enabled;
    emit findSettingsChanged();
}

void Document::wrapAroundChanged(bool enabled)
{
    m_findsettings.wrap_around = enabled;
    emit findSettingsChanged();
}

void Document::wholeWordsChanged(bool enabled)
{
    m_findsettings.whole_words = enabled;
    emit findSettingsChanged();
}

void Document::regexpChanged(bool enabled)
{
    m_findsettings.regexp = enabled;
    emit findSettingsChanged();
}

void Document::findPaletteFinished(int result)
{
    Q_UNUSED(result);
    destroyFindDialog();
}

void Document::privateFind(bool reverse)
{
    if(m_findsettings.find_str.isEmpty())
        return showFindDialog();

    QTextDocument::FindFlags flags;
    if(m_findsettings.case_sensitive)
        flags |= QTextDocument::FindCaseSensitively;

    if(m_findsettings.whole_words)
        flags |= QTextDocument::FindWholeWords;

    if(reverse)
        flags |= QTextDocument::FindBackward;


    if(m_findsettings.regexp)
    {

    }
    else
    {
        QTextCursor c = m_qdocument->find(m_findsettings.find_str,
                          editorWidget()->textCursor(), flags);

        if(!c.isNull())
            editorWidget()->setTextCursor(c);
    }
}

void Document::findNext()
{
    privateFind();
}

void Document::findPrevious()
{
    privateFind(true);
}

// ******** END find features **************

void Document::updatePreferences()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    QFont font;
    font.setFamily(settings.value("DefaultFontFamily").toString());
    font.setPointSize(settings.value("DefaultFontSize").toUInt());
    if(m_qdocument)
        m_qdocument->setDefaultFont(font);
}
