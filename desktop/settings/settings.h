#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QMainWindow>
#include <QListView>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include "settingsmodel.h"
#include "qcategorizedview/qcategorizedview.h"
#include "qcategorizedview/qcategorizedsortfilterproxymodel.h"

#define ARION_SETTINGS_WIDTH 680

class SettingsAppletInterface;
class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
    void showAppletById(const QString &id);
    void setSingleMode(bool single);
protected:
    void closeEvent(QCloseEvent*);
private slots:
    void clicked(const QModelIndex &index);
    void about();
    void requestQuit();
private:
    void showAppletWidget(SettingsAppletInterface* applet);
    void showAll();
    void setupMenu();
    void setupListView();
    uint homeHeight() const;
    QMenuBar *m_menuBar = nullptr;
    QToolBar *m_toolBar = nullptr;
    QLineEdit *m_search = nullptr;
    QCategorizedView *m_list   = nullptr;
    SettingsAppletModel *m_model = nullptr;
    QCategorizedSortFilterProxyModel *m_proxyModel = nullptr;
    QAction *m_back     = nullptr;
    QAction *m_forward  = nullptr;
    QAction *m_showAll  = nullptr;

    unsigned int m_homeWidth = 830;
    unsigned int m_homeHeight = 600;

    SettingsAppletInterface *m_current = nullptr;

    bool m_single = false;
};
#endif // SETTINGS_H
