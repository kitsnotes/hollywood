#ifndef CSYNTAXHIGHLIGHTER_H
#define CSYNTAXHIGHLIGHTER_H

#include "editor.h"

struct CHighlightRule
{
    QRegularExpression regexp;
    QTextCharFormat format;
};

class CSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    CSyntaxHighlighter(QTextDocument *parent = Q_NULLPTR);
protected:
    void highlightBlock(const QString &text) override;
private:
    void createRuleKeywords();
    void createRuleClass();
    void createRuleComments();
    void createRuleStrings();
    void createRuleFunctions();
private slots:
    void updatePreferences();
private:
    QVector<CHighlightRule> m_rules;
    QRegularExpression m_csregexp;
    QRegularExpression m_ceregexp;

    QColor c_keywords;
    QFont  f_keywords;

    QColor c_strings;
    QFont  f_strings;

    QColor c_comments;
    QFont  f_comments;

    QColor c_classes;
    QFont  f_classes;

    QColor c_functions;
    QFont  f_functions;

    QStringList m_keywords;

    QTextCharFormat classFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat functionFormat;
};

#endif // CSYNTAXHIGHLIGHTER_H
