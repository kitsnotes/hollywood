#include "features/textblockdata.h"

TextBlockData::TextBlockData() {}

QVector<ParenthesisInfo *> TextBlockData::parentheses()
{
    return m_parentheses;
}


void TextBlockData::insert(ParenthesisInfo *info)
{
    int i = 0;
    while (i < m_parentheses.size() && info->pos > m_parentheses.at(i)->pos)
        ++i;

    m_parentheses.insert(i, info);
}
