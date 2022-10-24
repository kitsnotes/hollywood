
#include "editorwidget.h"

#ifdef Q_WS_MAC

extern void disableWindowTabbing();
extern void displayNSMenuContextStyle(QWidget* parent, QMenu* m, QPoint p);
#endif

Editor::Editor(QWidget *parent)
    : QPlainTextEdit(parent),
      m_gutter(NULL),
      m_document(NULL),
      m_tabsize(4),
      m_tab_spaces(false),
      m_auto_indent(true),
      m_wrap_selection(true)
{
#ifdef Q_WS_MAC
    setAttribute(Qt::WA_NativeWindow, true);
#endif
    setBackgroundVisible(true);
    setFrameStyle(QFrame::NoFrame);

    setContextMenuPolicy(Qt::DefaultContextMenu);
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");
    QFont font;
    font.setFamily(settings.value("DefaultFontFamily").toString());
    font.setPointSize(settings.value("DefaultFontSize").toUInt());
    setFont(font);

    m_gutter = new Gutter(this);
    connect(this, SIGNAL(blockCountChanged(int)),
            this, SLOT(updateGutterWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)),
            this, SLOT(updateGutter(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(highlightSelectedLine()));

    updateGutterWidth(0);
    highlightSelectedLine();
    updatePreferences();
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));
}

Editor::~Editor()
{
    disconnect(myApp, SIGNAL(updatePreferences()), this, SLOT(updatePreferences()));
    m_gutter->deleteLater();
}

void Editor::setAppDocument(Document *d)
{
    m_document = d;
}

Document* Editor::appDocument()
{
    return m_document;
}

EditorWindow* Editor::mainWindow()
{
    TabWidget* w = qobject_cast<TabWidget*>(m_document->parent());

    if(!w)
    {
        qDebug() << "No TabWidget* for parent()";
        return Q_NULLPTR;
    }

    EditorWindow* ww =
            qobject_cast<EditorWindow*>(w->parent());

    if(!ww)
    {
        qDebug() << "No EditorWindow* for w->parent()";
        return Q_NULLPTR;
    }

    return ww;
}

int Editor::gutterWidth()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    bool showLineNumbers = settings.value("ShowLineNumbers", true).toBool();

    if(!showLineNumbers)
        return 0;

    int d = 3;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++d;
    }

    int s = 3 + fontMetrics().horizontalAdvance(QChar('9')) * d;
    return s > 0 ? s : 0;
}

void Editor::updateGutterWidth(int width)
{
    Q_UNUSED(width);
    setViewportMargins(gutterWidth(), 0, 0, 0);
}

void Editor::updateGutter(const QRect &rect, int dy)
{
    if(dy)
        m_gutter->scroll(0, dy);
    else
        m_gutter->update(0, rect.y(), m_gutter->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateGutterWidth(0);
}

void Editor::highlightSelectedLine()
{

}

void Editor::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton)
    {
        QMenu *m = createStandardContextMenu();
        e->accept();
        connect(m, SIGNAL(triggered(QAction*)), m, SLOT(deleteLater()));
#if QT_VERSION >= 0x060000
        m->popup(e->globalPosition().toPoint());
#else
        m->popup(e->globalPos());
#endif
        e->accept();
        return;
    }

    QPlainTextEdit::mousePressEvent(e);
}

void Editor::contextMenuEvent(QContextMenuEvent *e)
{
    //qDebug() << "Wanting popUpContextMenu";
    QMenu *m = createStandardContextMenu();
    connect(m, SIGNAL(triggered(QAction*)), m, SLOT(deleteLater()));
#if QT_VERSION >= 0x060000
    m->popup(e->globalPos());
#else
    m->popup(e->globalPos());
#endif

    //qDebug() << "Accepting event - popUpContextMenu";
    e->accept();
}

void Editor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect r = contentsRect();
    m_gutter->setGeometry(QRect(r.left(), r.top(),
                                gutterWidth(), r.height()));
}

void Editor::gutterPaintEvent(QPaintEvent *e)
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    bool showLineNumbers = settings.value("ShowLineNumbers", true).toBool();
    QPainter p(m_gutter);
    p.fillRect(e->rect(), Qt::lightGray);

    QTextBlock b = firstVisibleBlock();
    int bn = b.blockNumber();
    int top = (int) blockBoundingGeometry(b).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(b).height();

    while(b.isValid() && top <= e->rect().bottom())
    {
        if(b.isVisible() && bottom >= e->rect().top())
        {
            QString no = QString::number(bn+1);
            p.setPen(Qt::black);
            if(showLineNumbers)
                p.drawText(0, top, m_gutter->width() - 4,
                       fontMetrics().height(), Qt::AlignRight, no);
        }

        b = b.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(b).height();
        ++bn;
    }
}

void Editor::updatePreferences()
{
    QSettings settings("originull", "startext");
    settings.beginGroup("Preferences");

    m_tabsize = settings.value("TabWidth", 4).toUInt();
    m_tab_spaces = settings.value("SpacesForTabs", false).toBool();
    m_auto_indent = settings.value("AutoIndent", true).toBool();
    m_wrap_selection = settings.value("WrapSelection", true).toBool();

    bool m_ws = settings.value("VisualizeWhiteSpace", false).toBool();
    if(m_ws)
    {
        QTextOption opt = document()->defaultTextOption();
        opt.setFlags(QTextOption::ShowTabsAndSpaces|
                     QTextOption::ShowLineAndParagraphSeparators|
                     QTextOption::ShowDocumentTerminator);
        document()->setDefaultTextOption(opt);
    }
    else
    {
        QTextOption opt = document()->defaultTextOption();
        opt.setFlags((QTextOption::Flags)0);
        document()->setDefaultTextOption(opt);
    }
    //uint caretSize = settings.value("CaretSize").toUInt();
    /* if(caretSize == 4)
        setCursorWidth(fontMetrics().maxWidth());
    else
        setCursorWidth(caretSize); */

    QFont font;
    font.setFamily(settings.value("DefaultFontFamily").toString());
    font.setPointSize(settings.value("DefaultFontSize").toUInt());
    setFont(font);

    settings.beginGroup("ColorThemes");
    settings.beginGroup("Default");

    QColor fg(settings.value("ForegroundColor").toString());
    QColor bg(settings.value("BackgroundColor").toString());

    settings.endGroup();
    settings.endGroup();


    QPalette p;
    p = palette();

    p.setColor(QPalette::Window, bg);
    p.setColor(QPalette::WindowText, fg);
    p.setColor(QPalette::Base, bg);
    p.setColor(QPalette::Text, fg);

    setPalette(p);
    update();
}

void Editor::keyPressEvent(QKeyEvent *e)
{
    // Selection Wrap (braces, quotes etc)
    if(e->key() == Qt::Key_QuoteDbl   ||
       e->key() == Qt::Key_Apostrophe ||
       e->key() == Qt::Key_ParenLeft  ||
       e->key() == Qt::Key_BraceLeft  ||
       e->key() == Qt::Key_BracketLeft)
    {
        if(m_wrap_selection && textCursor().hasSelection())
        {
            QString val = textCursor().selectedText();
            switch(e->key())
            {
            case Qt::Key_BraceLeft:
                textCursor().insertText(QString("{%1}").arg(val));
                return e->accept();
            case Qt::Key_BracketLeft:
                textCursor().insertText(QString("[%1]").arg(val));
                return e->accept();
            case Qt::Key_ParenLeft:
                textCursor().insertText(QString("(%1)").arg(val));
                return e->accept();
            case Qt::Key_Apostrophe:
                textCursor().insertText(QString("'%1'").arg(val));
                return e->accept();
            case Qt::Key_QuoteDbl:
                textCursor().insertText(QString("\"%1\"").arg(val));
                return e->accept();
            default:
                break;
            }
        }
    }

    // Indentation
    if(e->key() == Qt::Key_Tab)
    {
        indent();
        return e->accept();
    }

    QPlainTextEdit::keyPressEvent(e);
}

void Editor::indent()
{
    if(!textCursor().hasSelection())
    {
        if(!m_tab_spaces)
           textCursor().insertText("\t");
        else
        {
            QString insert;
            for(uint i = 0; i < m_tabsize; i++)
                insert.append(' ');

            textCursor().insertText(insert);
        }

        return;
    }

    // at this point, we have a selection, so we
    // should perform an indent on all lines.

    QStringList l = textCursor().selectedText().split(QChar::ParagraphSeparator);
    QMutableListIterator<QString> m(l);
    while(m.hasNext())
    {
        if(!m_tab_spaces)
            m.setValue(m.next().prepend("\t"));
        else
        {
            QString space;
            for(uint i = 0; i < m_tabsize; i++)
                space.append(' ');

            m.setValue(m.next().prepend(space));
        }
    }

    textCursor().insertText(l.join(QChar::LineFeed));

    QTextCursor c = textCursor();
    c.movePosition(QTextCursor::Up, QTextCursor::KeepAnchor, l.size() - 1);
    c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);

    setTextCursor(c);
}

void Editor::unindent()
{

}

void Editor::selectLine()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    cursor.endEditBlock();
    setTextCursor(cursor);
}

void Editor::selectWord()
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    cursor.movePosition(QTextCursor::StartOfWord);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);

    cursor.endEditBlock();
    setTextCursor(cursor);

}

void Editor::insertPageBreak()
{
    textCursor().insertText("\f");
}

void Editor::insertDirectoryList(QString dir)
{
    Q_UNUSED(dir);
}

void Editor::insertFileContents(QString file)
{
    Q_UNUSED(file);
}

// ---------- GUTTER -------------

Gutter::Gutter(Editor *editor)
    : QWidget(editor)
{
    m_editor = editor;
    setFont(m_editor->font());
}

QSize Gutter::sizeHint() const
{
    return QSize(m_editor->gutterWidth(), 0);
}

void Gutter::paintEvent(QPaintEvent *e)
{
    m_editor->gutterPaintEvent(e);
}
