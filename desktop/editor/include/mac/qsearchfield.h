#ifndef QSEARCHFIELD_H
#define QSEARCHFIELD_H

#include "luvedit.h"
#include <QMacCocoaViewContainer>

Q_FORWARD_DECLARE_OBJC_CLASS(NSSearchField);

class SearchWidget : public QMacCocoaViewContainer
{
    Q_OBJECT
public:
    SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

    QSize sizeHint() const;
    NSSearchField* searchField() { return m_search; }
private:
    NSSearchField *m_search;
};

QMenu *createSearchMenu(QWidget *parent);

#endif // QSEARCHFIELD_H
