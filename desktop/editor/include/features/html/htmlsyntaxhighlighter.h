#ifndef HTMLSYNTAXHIGHLIGHTER_H
#define HTMLSYNTAXHIGHLIGHTER_H

#include "editor.h"

enum MarkupConstruct
{
    MarkupEntity,
    MarkupTag,
    MarkupComment,
    LastConstruct = MarkupComment
};

enum MarkupState
{
    NormalState = -1,
    InComment,
    InTag
};

class HtmlSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    HtmlSyntaxHighlighter(QTextDocument *parent = 0);
protected:
    void highlightBlock(const QString &text);
private:
    QTextCharFormat m_formats[LastConstruct + 1];
};

#endif // HTMLSYNTAXHIGHLIGHTER_H
