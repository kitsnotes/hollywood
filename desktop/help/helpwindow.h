#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QTabWidget>
#include <QHelpEngine>
#include <QUrl>

#include "qlitehtmlwidget.h"

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    HelpWindow(QString helpFile, QWidget *parent = nullptr);
    ~HelpWindow();
private:
    void loadHelp();
private slots:
    void openIndex();
    void loadUrl(const QUrl &url);
private:
    QMenuBar *m_menubar;
    QToolBar *m_toolbar;

    QSplitter *m_splitter;
    QWidget *m_dummy;

    QLiteHtmlWidget *m_html;
    QStatusBar *m_status;
private:
    QAction *m_contents = nullptr;
    QAction *m_index = nullptr;
    QActionGroup *m_ag = nullptr;
private:
    QHelpEngine *m_help = nullptr;
    bool m_init = false;
    QString m_helpPath;
};
#endif // HELPWINDOW_H
