#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include "editor.h"

class Editor;
class Gutter : public QWidget
{
    Q_OBJECT
public:
    explicit Gutter(Editor *editor = Q_NULLPTR);
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Editor *m_editor;
};

class Editor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();
    void setAppDocument(Document* d);
    Document* appDocument();
    EditorWindow* mainWindow();
    int gutterWidth();
    void gutterPaintEvent(QPaintEvent *e);
    void indent();
    void unindent();
    void selectWord();
    void selectLine();
    void insertPageBreak();
    void insertDirectoryList(QString dir);
    void insertFileContents(QString file);
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
private slots:
    void updateGutterWidth(int width);
    void highlightSelectedLine();
    void updateGutter(const QRect& rect, int dy);
    void updatePreferences();
private:
    Gutter *m_gutter;
    Document* m_document;
    unsigned int m_tabsize;
    bool m_tab_spaces;
    bool m_auto_indent;
    bool m_wrap_selection;
};

#endif // EDITORWIDGET_H
