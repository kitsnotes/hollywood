#ifndef HTMLFEATURES_H
#define HTMLFEATURES_H

#include "editor.h"
#include "features/html/htmlsyntaxhighlighter.h"
#include "features/abstractfeatures.h"

class HtmlSyntaxHighlighter;
class HtmlFeatures : public AbstractFeatures
{
    Q_OBJECT
public:
    HtmlFeatures(Document *parent);
    QString featureSetName() const;
    bool hasHighlighter() const { return true; }
    bool hasCompleter() const {return false; }
    bool hasBracketMatching() const { return false; }
    QSyntaxHighlighter* highlighter() const;
private:
    bool initFeatureSet(Document* d);
private:
    Document* m_parent;
    HtmlSyntaxHighlighter *m_highlighter;
};

#endif // HTMLFEATURES_H
