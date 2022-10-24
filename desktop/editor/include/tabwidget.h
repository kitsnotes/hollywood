#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "editor.h"

class Document;
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = nullptr);
    Document* documentForIndex(int idx);
    Document* currentDocument();
signals:
    void currentBufferModificationChanged(bool modififed);
public slots:
    void newDocument();
    bool openFile(QString fileName);
private slots:
    void modificationChanged(bool modififed);
    void updateNameAndSaveStatus();
    void closeTab();
private:
    QList<Document*> *m_documents;
};

#endif // TABWIDGET_H
