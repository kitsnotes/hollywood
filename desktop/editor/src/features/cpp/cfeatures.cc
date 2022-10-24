#include "features/cpp/cfeatures.h"

CFeatures::CFeatures(Document* parent)
    :AbstractFeatures(),
     m_parent(parent)
{
    initFeatureSet(parent);
}

QString CFeatures::featureSetName() const
{
    return QString("C/C++");
}

bool CFeatures::initFeatureSet(Document *d)
{
    m_highlighter = new CSyntaxHighlighter(d->editorWidget()->document());
    return true;
}

QSyntaxHighlighter* CFeatures::highlighter() const
{
    return m_highlighter;
}
