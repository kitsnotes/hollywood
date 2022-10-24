#ifndef PLAINFEATURES_H
#define PLAINFEATURES_H

#include "editor.h"
#include "features/abstractfeatures.h"

class PlainFeatures : public AbstractFeatures
{
    Q_OBJECT
public:
    PlainFeatures(Document *parent);
    QString featureSetName() const;
    bool hasHighlighter() const { return false; }
    bool hasCompleter() const { return false; }
    bool hasBracketMatching() const { return false; }
    QSyntaxHighlighter* highlighter() const { return Q_NULLPTR; }
private:
    bool initFeatureSet(Document* d);
private:
    Document* m_parent;
    //HtmlSyntaxHighlighter *m_highlighter;
};


#endif // PLAINFEATURES_H
