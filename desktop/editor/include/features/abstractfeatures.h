#ifndef ABSTRACTFEATURES_H
#define ABSTRACTFEATURES_H

#include "editor.h"

class AbstractFeatures : public QObject
{
public:
    virtual QString featureSetName() const = 0;
    virtual bool hasBracketMatching() const = 0;
    virtual bool hasHighlighter() const = 0;
    virtual bool hasCompleter() const = 0;
    virtual QSyntaxHighlighter* highlighter() const = 0;
};

#endif // ABSTRACTFEATURES_H
