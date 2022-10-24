
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent)
    :QTabWidget(parent)
{
    setAccessibleName(tr("Tab Bar"));
    setAccessibleDescription(tr("The application tab bar."));
    setDocumentMode(true);
    tabBar()->setAutoHide(true);
    tabBar()->setTabsClosable(true);
    setMovable(true);

#ifdef Q_WS_MAC
    tabBar()->setExpanding(true);
#endif
}

Document* TabWidget::currentDocument()
{
    Editor *e = qobject_cast<Editor*>(currentWidget());
    if(e)
        return e->appDocument();

    return nullptr;
}

void TabWidget::newDocument()
{
    Document *d = new Document(this);
    int idx = addTab(d->editorWidget(), tr("Untitled"));
    setCurrentIndex(idx);
    connect(d, SIGNAL(updateNameAndSaveStatus()), this, SLOT(updateNameAndSaveStatus()));
    connect(d, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));
    connect(d, SIGNAL(requestTabClose()), this, SLOT(closeTab()));
#ifdef WINDOWS_DESKTOP
    //QtWin::taskbarAddTab(d->editorWidget());
#endif

    QTimer::singleShot(0, d->editorWidget(), SLOT(setFocus()));

}

bool TabWidget::openFile(QString fileName)
{
    Document *d = Document::openFromFile(fileName);
    if(!d)
        return false;

    int idx = addTab(d->editorWidget(), d->displayName());
    setCurrentIndex(idx);
#ifdef WINDOWS_DESKTOP
    //QtWin::taskbarAddTab(d->editorWidget());
#endif

    connect(d, SIGNAL(updateNameAndSaveStatus()), this, SLOT(updateNameAndSaveStatus()));
    connect(d, SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));
    connect(d, SIGNAL(requestTabClose()), this, SLOT(closeTab()));
    return true;
}

void TabWidget::closeTab()
{
    Document *d = qobject_cast<Document*>(sender());
    if(!d)
        return;

    d->deleteLater();

#ifdef WINDOWS_DESKTOP
    //QtWin::taskbarDeleteTab(d->editorWidget());
#endif

    /*int idx = indexOf(d->editorWidget());
    if(count() < 2)
    {
        newDocument();
        idx = indexOf(d->editorWidget());
    }

    removeTab(idx); */
}

Document* TabWidget::documentForIndex(int idx)
{
    Editor *e = qobject_cast<Editor*>(widget(idx));
    if(e)
        return e->appDocument();

    return nullptr;
}

void TabWidget::modificationChanged(bool modified)
{
    Document *d = qobject_cast<Document*>(sender());
    QColor unsaved(Qt::darkRed);

    if(modified)
        tabBar()->setTabTextColor(indexOf(d->editorWidget()), unsaved);
    else
        tabBar()->setTabTextColor(indexOf(d->editorWidget()),
                                  QApplication::palette().text().color());

    if(qobject_cast<Editor*>(currentWidget()) == d->editorWidget())
        emit currentBufferModificationChanged(modified);
}

void TabWidget::updateNameAndSaveStatus()
{
    Document *d = qobject_cast<Document*>(sender());
    tabBar()->setTabText(indexOf(d->editorWidget()), d->displayName());
    emit modificationChanged(d->isModified());
}
