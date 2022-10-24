#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include "editor.h"

struct ParenthesisInfo
{
    char character;
    int pos;
};

struct BraceInfo
{
    char character;
    int pos;
};

struct BracketInfo
{
    char character;
    int pos;
};

class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData();

    QVector<ParenthesisInfo *> parentheses();
    void insert(ParenthesisInfo *info);

private:
    QVector<ParenthesisInfo *> m_parentheses;
    QVector<ParenthesisInfo *> m_brackets;
    QVector<ParenthesisInfo *> m_braces;
};

#endif // TEXTBLOCKDATA_H
