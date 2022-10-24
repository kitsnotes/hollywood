#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "editor.h"

#if QT_VERSION >= 0x060000
#include <QtCore5Compat/QTextCodec>
#endif

class Editor;
class EditorWindow;
class SpellCheckDialog;
class FindDialog;
class AbstractFeatures;
class PlainFeatures;

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = Q_NULLPTR);
    ~Document();

    // Language Features
    AbstractFeatures* languageFeatures() const;
    LanguageFeatureType languageFeatureType() const;
    void setLanguageFeatures(LanguageFeatureType f);

    static Document* openFromFile(QString filename);
    bool saveFile(QString filename = QString());
    Editor* editorWidget();
    QString fileName();
    QString displayName();
    bool isModified();
    bool isUntitled();
    LineEnding lineEndings();
    void setLineEndings(LineEnding e);
    QString encoding();
    FindSettings findSettings();
    void setFindSettings(FindSettings &f);
    void showFindDialog();
    QPrinter* printer();
    // insertations
    void insertFile(QString fileName);
    void insertShortTimestamp();
    void insertLongTimestamp();
    // transformations
signals:
    void copyAvailable(bool);
    void cursorPositionChanged();
    void selectionChanged();
    void undoAvailable(bool);
    void redoAvailable(bool);
    void updateNameAndSaveStatus();
    void modificationChanged(bool modified);
    void requestTabClose();
    void findStringChanged(const QString &findString);
    void findSettingsChanged();
    void lineEndingsChanged();
    void languageFeaturesChanged();
public slots:
    void activatePalettes();
    void deactivatePalettes();
    void checkSpelling();
    void updatePreferences();
    void closeDocument();
    void toolbarQuickFindChanged(const QString &findString);
    void findNext();
    void findPrevious();
private slots:
    void dlgFindStringChanged(const QString &findString);
    void findPaletteFinished(int result = 0);
    void privateFind(bool reverse = false);
    // these 3 are all find features
    void caseSensitiveChanged(bool enabled);
    void wrapAroundChanged(bool enabled);
    void wholeWordsChanged(bool enabled);
    void regexpChanged(bool enabled);
private:
    void setupSpellingDialog();
    void destroySpellingDialog();
    void setupFindDialog();
    void destroyFindDialog();
    bool loadFromFile(QString filename);
    bool saveToFile(QString filename = QString());
    LineEnding determineLineEnding(QString b);
private:
    Editor *m_editor;
    SpellCheckDialog *m_spelling;
    bool m_show_spelling;
    QPrinter *m_printer;
    QTextDocument *m_qdocument;
    //QSyntaxHighlighter *m_syntax;
    AbstractFeatures *m_features;
    LanguageFeatureType m_filetype;

    QString m_filename;
    bool m_untitled;
    LineEnding m_lineending;
    QTextCodec *m_encoding;
    FindSettings m_findsettings;
    FindDialog* m_find_dlg;
    bool m_show_find;
};

#endif // DOCUMENT_H
