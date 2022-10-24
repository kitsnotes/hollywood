#ifndef MSMENUSERVER_H
#define MSMENUSERVER_H

#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QToolButton>
#include <QApplication>
#include <QScreen>

#include <client/wlrlayersurfacev1.h>

class MSMenuServer : public QWidget
{
    Q_OBJECT

public:
    MSMenuServer(QWidget *parent = nullptr);
    ~MSMenuServer();
    void installMenu(QMenu *menu);
    void installDefaultMenu();
private:
    void setupMenu();
    void cleanMenu();
    void screenGeometryChanged(const QRect &geometry);
private:
    QScreen *m_screen = nullptr;
    QHBoxLayout *m_layout;
    QMenuBar *m_menuBar;
    QSpacerItem *m_spacer;
    QMenu *m_systemMenu = nullptr;
    ArionWaylandClient::WlrLayerSurfaceV1 *m_layer = nullptr;
};
#endif // MSMENUSERVER_H
