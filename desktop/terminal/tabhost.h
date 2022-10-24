#ifndef TABHOST_H
#define TABHOST_H

#include <QWidget>
#include <QMap>

class QTabBar;
class TerminalHost;
class TabHost : public QWidget
{
    Q_OBJECT
    friend class TerminalWindow;
public:
    explicit TabHost(QWidget *parent = nullptr);
public slots:
    void newDefaultTab();
    bool requestCloseAll();
    void copySelection();
    void pasteClipboard();
    void pasteSelection();
    void zoomIn();
    void zoomOut();
    void zoomNormal();
    void clear();
    void profileDeleted(const QString &profile);
signals:
    void requestWindowClose();
    void currentTabChanged();
    void contextMenuRequested(const QPoint &pos);
    void selectionChanged();
private slots:
    void tabChanged(int idx);
    void tabCloseRequest(int idx);
    void removeTab(int idx);
    void termRequestClose();
    void windowTitleChanged(const QString &title);
    void tabTitleChanged(const QString &title);
    void termCopyAvailable(bool copy);
protected:
    TerminalHost* currentHost();
private:
    int indexForTab(const QByteArray &tabId) const;
private:
    QTabBar *m_tabs = nullptr;
    QWidget *m_placeholder = nullptr;
    QWidget *m_current = nullptr;
    QMap<QByteArray,TerminalHost*> m_terminals;
};

#endif // TABHOST_H
