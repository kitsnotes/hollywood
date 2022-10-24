#include "menuserver.h"
#include "application.h"

#include <QLabel>

MSMenuServer::MSMenuServer(QWidget *parent)
    : QWidget(parent),
      m_layout(new QHBoxLayout(this)),
      m_menuBar(new QMenuBar(0)),
      m_spacer(new QSpacerItem(5,5, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding)),
      m_layer(new ArionWaylandClient::WlrLayerSurfaceV1(this))
{
    QScreen *screen = qApp->primaryScreen();
    connect(screen, &QScreen::geometryChanged, this, &MSMenuServer::screenGeometryChanged);
    int max_height = 25;
    setMinimumHeight(max_height);
    setMaximumHeight(max_height);
    setMaximumWidth(screen->geometry().width());
    setMinimumWidth(screen->geometry().width());
    auto testlabel = new QLabel(this);
    testlabel->setText(tr("Test"));
    m_layout->addWidget(m_menuBar);
    m_layout->addSpacerItem(m_spacer);
    m_layout->addWidget(testlabel);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0,0,0,0);
    move(0,0);
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
    setupMenu();
    m_layer->setWindow(windowHandle());
    m_layer->setAnchors(ArionWaylandClient::WlrLayerSurfaceV1::TopAnchor);
    m_layer->setLayer(ArionWaylandClient::WlrLayerSurfaceV1::TopLayer);
    m_layer->setExclusiveZone(18);
    m_layer->setLayerEnabled(true);
}


MSMenuServer::~MSMenuServer()
{

}

void MSMenuServer::installMenu(QMenu *menu)
{
    cleanMenu();
    if(menu == nullptr)
        return;

    for(auto a : menu->actions())
    {
        auto m = QMenu::menuInAction(a);
        if(m != nullptr)
            m_menuBar->addMenu(m);
        else
            m_menuBar->addAction(a);
    }
}

void MSMenuServer::setupMenu()
{
    QIcon starIcon(QIcon::fromTheme("help-about"));
    m_systemMenu = m_menuBar->addMenu(starIcon, "");
    m_systemMenu->menuAction()->setData(9195521);
    m_systemMenu->addAction(tr("&About System"))->setIcon(starIcon);
    m_systemMenu->addAction(tr("Application &Market"));
    m_systemMenu->addSeparator();
    m_systemMenu->addAction(tr("&Exit Shell..."));
}

void MSMenuServer::cleanMenu()
{
    for(auto a : m_menuBar->actions())
    {
        if(a->data() != 9195521)
            m_menuBar->removeAction(a);
    }
}

void MSMenuServer::screenGeometryChanged(const QRect &geometry)
{
    setMaximumWidth(geometry.width());
    setMinimumWidth(geometry.width());
}
