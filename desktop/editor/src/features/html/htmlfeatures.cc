#include "features/html/htmlfeatures.h"

HtmlFeatures::HtmlFeatures(Document* parent)
    :AbstractFeatures(),
     m_parent(parent)
{
    initFeatureSet(parent);
}

QString HtmlFeatures::featureSetName() const
{
    return QString("HTML");
}


bool HtmlFeatures::initFeatureSet(Document *d)
{
    m_highlighter = new HtmlSyntaxHighlighter(d->editorWidget()->document());

    return true;
}

QSyntaxHighlighter* HtmlFeatures::highlighter() const
{
    return m_highlighter;
}
