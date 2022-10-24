#include "progman.h"
#include "app.h"
#include <QSettings>
#include <QScreen>
#include <QCloseEvent>
#include <QMenuBar>
#include <QAction>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>

#include <mimeapps.h>
#include <desktopentry.h>

ProgramManager::ProgramManager(QWidget *parent)
    : QMainWindow(parent),
    m_view(new QListView(this)),
    m_mime(new LSMimeApplications(this)),
    m_model(new QStandardItemModel(this))
{
    setObjectName("FMWindow");
    setWindowIcon(QIcon::fromTheme("utilities-terminal"));
    setWindowTitle(tr("Program Manager"));
    setCentralWidget(m_view);
    m_view->setModel(m_model);
    m_view->setEditTriggers(QListView::NoEditTriggers);
    QSettings settings("originull", "hollywood");
    if(settings.contains("CoreShell/Cache/LastWindowPos"))
        restoreGeometry(settings.value("CoreShell/Cache/LastWindowPos").toByteArray());
    else
    {
        // fallback defaults
        resize(560,440);
        move(screen()->geometry().center() - frameGeometry().center());
    }

    connect(m_view, &QAbstractItemView::activated,
            this, &ProgramManager::activated);

    bool hideOnlyShow = settings.value("CoreShell/HideOnlyShow", true).toBool();
    bool quitlaunch =  settings.value("CoreShell/ProgmanQuitLaunch", true).toBool();
    auto menubar = new QMenuBar(this);
    setMenuBar(menubar);
    auto file = menubar->addMenu(tr("&File"));
    auto view = menubar->addMenu(tr("&View"));
    auto quitlaunchact = view->addAction(tr("Quit on Launch"));
    quitlaunchact->setCheckable(true);
    quitlaunchact->setChecked(quitlaunch);
    auto hideapps = view->addAction(tr("Hide OnlyShowIn Applications"));
    hideapps->setCheckable(true);
    hideapps->setChecked(hideOnlyShow);
    auto help = menubar->addMenu(tr("&Help"));
    auto closewnd = file->addAction(tr("&Close Window"));
    connect(closewnd, &QAction::triggered, this, &QMainWindow::close);

    auto about = help->addAction(tr("&About CoreShell"));
    connect(about, &QAction::triggered, CSApplication::instance(),
            &CSApplication::aboutApplication);

    refreshPrograms();
}

ProgramManager::~ProgramManager()
{

}

void ProgramManager::closeEvent(QCloseEvent *event)
{
    QSettings settings("originull", "persistence");
    settings.setValue("CoreShell/Cache/LastWindowPos", saveGeometry());

    event->accept();
}

void ProgramManager::activated(const QModelIndex &idx)
{
    auto dt = m_model->itemFromIndex(idx)->data().toString();
    qDebug() << "Activated Item:" << dt;
    CSApplication::instance()->executeDesktop(dt);
}

void ProgramManager::refreshPrograms()
{
    m_model->clear();
    m_mime->cacheAllDesktops();
    for(auto app : m_mime->allApps())
    {
        if(app->contains(QLatin1String("OnlyShowIn")))
            continue;

        auto si = new QStandardItem(app->icon(), app->value("Name").toString());
        si->setData(app->fileName());
        m_model->appendRow(si);
    }
}
