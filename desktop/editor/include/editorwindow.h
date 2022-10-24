#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "editor.h"

class FindDialog;
class SpellCheckDialog;
class TabWidget;
class Document;
#ifdef Q_WS_MAC
class MacToolBar;
#endif
class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();
    QList<Document*> documents();
    void focusDocument(Document* d);
    TabWidget* tabWidget();
public slots:
    void newDocument();
    void showOpenFileDialog();
    void saveFile();
    void saveFileAs();
    void print();
    void pageSetup();
    void printPreview();

    void cut();
    void copy();
    void paste();

    void undo();
    void redo();

    void selectAll();
    void selectWord();
    void selectLine();
    void deleteSelection();

    void insertPageBreak();
    void insertFileContents();
    void insertDirectoryList();

    void aboutToShowEncodingMenu();
    void closeTab();
    int promptToSaveChanges(int tab_id = -1);
    void setWindowTitle(const QString &title);
    void setWindowFilePath(const QString &filePath);
    void findAndReplace();
    void updateLineEnding();
    void updateLanguageFeature();
protected:
    void closeEvent(QCloseEvent *event);
    void focusInEvent(QFocusEvent *event);
private slots:
    void tabCloseRequested(int idx);
    void selectionChanged();
    void tabIndexChanged(int idx);
    void clipboardDataChanged();
    void setUndoAvailable(bool available);
    void setRedoAvailable(bool available);
    void configureColors();
    void configureFont();
    void checkSpelling();
    void previewPrint(QPrinter* p);
    void printExecute(QPrinter* p);
    void changeSyntax();
    void windowStateChanged(Qt::WindowState state);
    void unimplemented();
    void updateLineColumn();
    void triggerStatusBar();
    void triggerToolbar();
    void triggerLineNumbers();
    void triggerVisualizeWhitespace();
    void updatePreferences();
    void showAbout();
    void showPreferences();
    void updateFindString(const QString &findString);
    void buildMRUMenu();
    void openMRUItem(QAction *item);
    void destroyMRUMenu();
    void promptForLineNumber();
    void showInFileManager();
    void updateNameAndSaveStatus();
private:
    QAction* findActionByName(QString name);
    void createActions();
    void createMainMenu();
    void createStatusBar();
    void createToolBar();
    void restorePosition();
    void savePosition();
    void canUndoRedo();
    void updateLineNumberAction();
    void updateWhitespaceAction();
    uint unsavedDocumentCount();
    void uncheckAllLanguageFeatures();
private:
    QMap<QString, QAction*> m_actions;
    QList<QAction*> m_language_feature_actions;
    QList<QAction*> m_mru_list;
    QMenuBar* m_menubar;
#ifdef Q_WS_MAC
    MacToolBar *m_toolbar;
#else
    QToolBar* m_toolbar;
    QWidget *m_toolbar_stretcher;
    QLineEdit *m_searchbox;
#endif
    QStatusBar* m_status;
    TabWidget *m_tabwidget;
    /* Status Bar Items */
    QLabel* m_col_ln;
    QLabel* m_encoding;
    QLabel* m_lineendings;
    QLabel* m_language;
    Document* m_active_document;
};

#endif // EDITORWINDOW_H
