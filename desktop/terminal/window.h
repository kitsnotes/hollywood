#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QApplication>

class PreferencesWindow;
class TabHost;
class TerminalWindow : public QMainWindow
{
    Q_OBJECT
public:
    TerminalWindow(QWidget *parent = nullptr);
    ~TerminalWindow();
private:
    void setupActions();
protected:
    void closeEvent(QCloseEvent *event) override;
signals:
    void windowClosed();
private slots:
    void newTab();
    void currentTabChanged();
    void showPreferences();
    void aboutTerminal();
    void clipboardDataChanged();
    void saveToFile();
    void contextMenuRequested(const QPoint &pos);
private:
    TabHost *m_tabs = nullptr;

    QAction *m_copy = nullptr;
    QAction *m_pasteClipboard = nullptr;
    QAction *m_pasteSelection = nullptr;
    QAction *m_saveSelection = nullptr;
};
#endif // TERMINALWINDOW_H
