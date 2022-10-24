#include "features/plain/plainfeatures.h"

PlainFeatures::PlainFeatures(Document* parent)
    :AbstractFeatures(),
     m_parent(parent)
{
    initFeatureSet(parent);
}

QString PlainFeatures::featureSetName() const
{
    return tr("Plain Text");
}

bool PlainFeatures::initFeatureSet(Document *d)
{
    Q_UNUSED(d);
    //m_highlighter = new CSyntaxHighlighter(d->editorWidget()->document());
    return true;
}

/* QSyntaxHighlighter* PlainFeatures::highlighter() const
{
    return Q_NULLPTR;
    //return m_highlighter;
} */
