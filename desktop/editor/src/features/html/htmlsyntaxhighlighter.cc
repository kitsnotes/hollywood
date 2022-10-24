#include "features/html/htmlsyntaxhighlighter.h"

HtmlSyntaxHighlighter::HtmlSyntaxHighlighter(QTextDocument *parent)
    :QSyntaxHighlighter(parent)
{
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(0, 128, 0));
    entityFormat.setFontWeight(QFont::Bold);
    m_formats[MarkupEntity] = entityFormat;

    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(192, 16, 112));
    tagFormat.setFontWeight(QFont::Bold);
    m_formats[MarkupTag] =  tagFormat;

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(128, 10, 74));
    commentFormat.setFontItalic(true);
    m_formats[MarkupComment] = commentFormat;
}

void HtmlSyntaxHighlighter::highlightBlock(const QString &text)
{
    MarkupState state = (MarkupState)previousBlockState();
    int len = text.length();
    int start = 0, pos = 0;
    while(pos < len)
    {
        switch(state)
        {
            case NormalState:
            default:
                while(pos < len)
                {
                    QChar c = text.at(pos);
                    if(c == '<')
                    {
                        if(text.mid(pos, 4) == "<!--")
                            state = InComment;
                        else
                            state = InTag;
                        break;
                    }
                    else if (c == '&')
                    {
                        start = pos;
                        while(pos < len && text.at(pos++) != ';') ;
                        setFormat(start, pos - start, m_formats[MarkupEntity]);
                    }
                    else
                        ++pos;
                }
                break;
            case InComment:
                start = pos;
                while(pos < len)
                {
                    if(text.mid(pos, 3) == "-->")
                    {
                        pos += 3;
                        state = NormalState;
                        break;
                    }
                    else
                        ++pos;
                }
                setFormat(start, pos - start, m_formats[MarkupComment]);
                break;
            case InTag:
                QChar qu = QChar::Null;
                start = pos;
                while(pos < len)
                {
                    QChar c = text.at(pos);
                    if(qu.isNull())
                    {
                        if(c == '\'' || c == '"')
                            qu = c;
                        else if (c == '>')
                        {
                            ++pos;
                            state = NormalState;
                            break;
                        }
                    }
                    else if (c == qu)
                        qu = QChar::Null;

                    ++pos;
                }
                setFormat(start, pos - start, m_formats[MarkupTag]);
        }
    }
    setCurrentBlockState(state);
}
