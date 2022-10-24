#ifndef CFEATURES_H
#define CFEATURES_H

#include "editor.h"
#include "features/cpp/csyntaxhighlighter.h"
#include "features/abstractfeatures.h"
#include "features/textblockdata.h"

class TextBlockData;
class CSyntaxHighlighter;
class CFeatures : public AbstractFeatures
{
    Q_OBJECT
public:
    CFeatures(Document *parent);
    QString featureSetName() const;
    bool hasBracketMatching() const { return true; }
    bool hasHighlighter() const { return true; }
    bool hasCompleter() const {return false; }
    QSyntaxHighlighter* highlighter() const;
private:
    bool initFeatureSet(Document* d);
private:
    Document* m_parent;
    CSyntaxHighlighter *m_highlighter;
};

#endif // CFEATURES_H
