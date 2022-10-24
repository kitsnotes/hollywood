#ifndef APPLICATION_H
#define APPLICATION_H

#include "editor.h"

#ifdef THINK_DIFFERENT
Q_FORWARD_DECLARE_OBJC_CLASS(EditorWidgetServicesResponder);
#endif

class EditorWindow;
class PreferencesDialog;
class SpellCheck;
class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);
    QStringList dialogNameFilters();
    EditorWindow* newEditorWindow(QString filename = QString());
    QStringList encodingCategories();
    QMap<QString, QString> encodings();
    QString categoryForEncoding(QString encoding);
    void requestPreferencesUpdate();
    EditorWindow* currentWindow();
    void setCurrentWindow(EditorWindow* w);
    void parseCommandLine();
    MRUFiles mruList() const;
    bool checkSpellCheck(bool silent = false);
    SpellCheck* spellChecker();
#ifdef Q_WS_WIN
    QString findDefaultBrowserWindows();
#endif
signals:
    void updatePreferences();
public slots:
    void openFile(QString fileName);
    void showPreferencesDialog(QWidget *parent = Q_NULLPTR);
    void aboutApplication(QWidget *parent = Q_NULLPTR);
    void newWindowSlot();
    void addToMRUFiles(const QString &fileName);
private slots:
    void cleanMRUList();
    void rebuildMRUInMemory();
    void windowDestroyed(QObject*);
private:
    void createDefaultSettings();
    void setupEncodings();
    void setupBareMenuBar();
#ifdef Q_WS_WIN
    void createJumpList();
#endif
#ifdef Q_WS_MAC
    void createDockMenu();
    void installMacResponder();
#endif
private:
    QAction* findActionByName(QString name);
    QMap<QString, QAction*> m_actions;
    QMenuBar* m_menubar;

    QMap<QString, QString> m_encodingNames;
    QMap<QString, QString> m_encodingCategories;

    QList<EditorWindow*> m_windows;

    PreferencesDialog *m_prefs;
#ifdef Q_WS_WIN
    QWinJumpList *m_jumplist;
#endif
#ifdef Q_WS_MAC
    QMenu *m_dock_menu;
    EditorWidgetServicesResponder* m_services_responder;
#endif
    SpellCheck *m_spell_check;
    MRUFiles m_mru_files;
    EditorWindow *m_current; // always last
};

#ifdef Q_WS_WIN
QPixmap luv_pixmapFromWinHICON(HICON icon);
QImage luv_imageFromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h);
#endif

#if defined(myApp)
#undef myApp
#endif
#define myApp (static_cast<Application*>(QCoreApplication::instance()))

#endif // APPLICATION_H
