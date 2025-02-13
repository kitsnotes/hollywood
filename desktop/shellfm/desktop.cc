#include "desktop.h"
#include "fmapplication.h"
#include <actionmanager.h>
#include <filesystemmodel.h>
#include <fsitemdelegate.h>
#include <executor.h>
#include <desktopmodel.h>
#include <desktopentry.h>
#include <QMimeData>

#include "desktopviewoptions.h"
#include "commonfunctions.h"
#include "opmanager.h"

DesktopWindow::DesktopWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_view(new QListView(this))
    , m_model(new LSDesktopModel(this))
    , m_delegate(new LSFSItemDelegate(this))
    , m_actions(new LSActionManager(this))
    , m_rightclick(new QMenu(this))
    , m_viewOptions(new DesktopViewOptions(this))
{
    m_viewOptions->setHidden(true);
    m_menuBar = new QMenuBar(this);
    setMenuBar(m_menuBar);
    m_menuBar->setVisible(false);
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);
    setObjectName("DesktopWindow");
    setCentralWidget(m_view);
    m_view->setModel(m_model);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_TransparentForMouseEvents);

    //connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &FileWindow::desktopGeometryChanged);

    setWindowFlag(Qt::WindowStaysOnBottomHint, true);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setDragEnabled(true);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setDropIndicatorShown(true);
    m_view->setDragDropMode(QAbstractItemView::DragDrop);

    auto font = m_view->font();
    font.setBold(true);
    m_view->setFont(font);
    m_view->setItemAlignment(Qt::AlignHCenter);
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setItemDelegate(m_delegate);

    m_delegate->setShadowHidden(false);

    QPalette p = m_view->palette();
    p.setColor(QPalette::Base, QColor(Qt::transparent));
    p.setColor(QPalette::Text, QColor(Qt::white));
    m_view->setPalette(p);
    m_delegate->setShadowColor(QColor(Qt::black));

    setupActions();
    setupMenuBar();

    connect(m_view, &QAbstractItemView::activated,
            this, &DesktopWindow::activated);

    connect(m_view, &QAbstractItemView::customContextMenuRequested,
            this, &DesktopWindow::contextMenu);

    resize(qApp->primaryScreen()->size());

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DesktopWindow::selectionChanged);

    disableFileActions();
    enablePaste(false);

    loadViewOptions();
    m_viewOptions->updateViewOptions(m_view);
    connect(m_viewOptions, &DesktopViewOptions::optionsChanged, this, &DesktopWindow::viewOptionsChanged);
    connect(m_viewOptions, &QDialog::rejected, this, &DesktopWindow::viewOptionsClosed);
    connect(LSCommonFunctions::instance(), &LSCommonFunctions::pasteAvailable, this, &DesktopWindow::enablePaste);

}

void DesktopWindow::enablePaste(bool enable, uint count)
{
    if(!enable)
    {
        shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste"));
        shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(enable);
    }
    else
    {
        if(count == 1)
            shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste %1 Item").arg(QString::number(count)));
        else
            shellAction(HWShell::ACT_EDIT_PASTE)->setText(tr("&Paste %1 Items").arg(QString::number(count)));
        shellAction(HWShell::ACT_EDIT_PASTE)->setEnabled(enable);
    }
}

QAction* DesktopWindow::shellAction(HWShell::ShellActions shellAction)
{
    return m_actions->shellAction(shellAction);
}

void DesktopWindow::goQuickAction()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString actionName = action->objectName();
    QString path("");

    if(actionName == "GoHome")
        path = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();

    if(actionName == "GoDocuments")
        path = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();

    if(actionName == "GoDownloads")
        path = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();

    if(actionName == "GoDesktop")
        path = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();

    if(actionName == "GoPhotos")
        path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();

    if(actionName == "GoVideos")
        path = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first();

    if(actionName == "GoMusic")
        path = QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first();

    if(!path.isEmpty())
    {
        FMApplication::instance()->newFileWindow(QUrl::fromLocalFile(path));
        return;
    }

    if(actionName == "GoApplications")
    {
        FMApplication::instance()->newFileWindow(QUrl("applications://"));
        return;
    }
}

void DesktopWindow::openItems()
{
    // TODO: a warning if there's more than like 10 items...
    for(auto idx : m_view->selectionModel()->selectedIndexes())
    {
        activated(idx);
    }
}

void DesktopWindow::copyItems()
{
    QList<QUrl> urls;
    for(auto idx : m_view->selectionModel()->selectedIndexes())
        urls.append(m_model->url(idx));

    auto mimedata = new QMimeData;
    mimedata->setUrls(urls);
    FMApplication::instance()->clipboard()->setMimeData(mimedata);
}

void DesktopWindow::paste()
{
    auto desktop = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();
    auto data = FMApplication::instance()->clipboard()->mimeData()->urls();
    LSCommonFunctions::instance()->operationManager()->copyFiles(data, QUrl::fromLocalFile(desktop));
}

void DesktopWindow::newFolder()
{
    auto desktop = QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    LSCommonFunctions::instance()->newFolder(desktop, this);
}

void DesktopWindow::rename()
{

}

void DesktopWindow::archive()
{

}

void DesktopWindow::trash()
{

}

void DesktopWindow::eject()
{
    for(auto idx : m_view->selectionModel()->selectedIndexes())
    {
        if(m_model->isDevice(idx))
        {
            auto device = m_model->deviceForIndex(idx);
            if(device)
            {
                qDebug() << "ejecting" << device->name();
                device->eject();
            }
        }
    }
}

void DesktopWindow::activated(const QModelIndex &idx)
{
    if(m_model->isTrash(idx))
    {
        FMApplication::instance()->newFileWindow(QUrl("trash://"));
        return;
    }

    if(m_model->isDevice(idx))
    {
        auto dev = m_model->deviceForIndex(idx);
        if(dev)
        {
            auto mountpoint = dev->mountpoint();
            if(!mountpoint.isEmpty())
            {
                FMApplication::instance()->newFileWindow(QUrl::fromLocalFile(mountpoint));
                return;
            }
            else
            {
                // not mounted - handle that...
                // this is a device that is unmounted...
                // mount it up:
                dev->mount();
                // TODO: check for mount, and open it:
            }
        }
        return;
    }

    QFileInfo fileInfo(m_model->fileInfo(idx));
    if(fileInfo.isDir())
    {
        if(fileInfo.isReadable() && fileInfo.isExecutable())
            FMApplication::instance()->newFileWindow(QUrl::fromLocalFile(fileInfo.canonicalFilePath()));
        else
            FMApplication::instance()->showPermissionError(QUrl(fileInfo.canonicalFilePath()));

        return;
    }

    // handle a .desktop file
    if(m_model->isDesktop(idx))
    {
        FMApplication::instance()->executeDesktopOverDBus(m_model->desktopFileForIndex(idx)->fileName());
        return;
    }

    // TODO: handle an appimage
    // TODO: handle executing an executable file???

    FMApplication::instance()->openFileOverDBusWithDefault(fileInfo.canonicalFilePath());
}

void DesktopWindow::contextMenu(const QPoint &pos)
{
    // provision the standard nosel context menu
    if(m_view->selectionModel()->selectedIndexes().isEmpty())
        m_rightclick->popup(pos);
    else
    {
        bool single = m_view->selectionModel()->selectedIndexes().count() == 1 ? true : false;
        bool isdesktop = false;
        bool istrash = false;
        bool isdisk = false;

        if(single)
        {
            auto idx = m_view->selectionModel()->selectedIndexes().first();
            isdesktop = m_model->isDesktop(idx);
            istrash = m_model->isTrash(idx);
            isdisk = m_model->isDevice(idx);
        }

        QModelIndex pointIndex = m_view->indexAt(pos);
        if(!pointIndex.isValid() && m_view->selectionModel()->selectedIndexes().count() > 0)
        {
            // we right-clicked outside of the selection, so clear it
            m_view->clearSelection();
            m_rightclick->popup(pos);
            return;
        }

        auto menu = new QMenu(this);
        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
        updateSingleActionStrings();
        menu->addAction(shellAction(HWShell::ACT_FILE_OPEN));
        /*if(single && !isdesktop && !istrash)
            menu->addMenu(openWithMenu());*/
        menu->addSeparator();
        if(!istrash && !isdesktop)
            menu->addAction(shellAction(HWShell::ACT_FILE_ARCHIVE));
        menu->addAction(shellAction(HWShell::ACT_FILE_RENAME));
        menu->addAction(shellAction(HWShell::ACT_EDIT_COPY));
        menu->addSeparator();
        if(!istrash && !isdisk)
            menu->addAction(shellAction(HWShell::ACT_FILE_TRASH));
        if(isdisk)
            menu->addAction(shellAction(HWShell::ACT_FILE_EJECT));
        menu->addSeparator();
        menu->addAction(shellAction(HWShell::ACT_FILE_GET_INFO));

        menu->popup(mapToGlobal(pos));
    }
}

void DesktopWindow::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    switch(m_view->selectionModel()->selectedIndexes().count())
    {
    case 0:
        disableFileActions();
        resetActionStrings();
        break;
    case 1:
        resetActionStrings();
        enableFileActions();
        updateSingleActionStrings();
        break;
    case 2:
    default:
        resetActionStrings();
        enableFileActions();
        menu_Open_With->setEnabled(false);
        break;
    }
}

void DesktopWindow::updateSingleActionStrings()
{
    auto idx = m_view->selectionModel()->selectedIndexes().first();
    if(m_model->isDesktop(idx))
    {
        auto desktop = m_model->desktopFileForIndex(idx);
        shellAction(HWShell::ACT_FILE_OPEN)->setText(QString("%1 %2")
                                                         .arg(tr("&Open")).arg(desktop->value("Name").toString()));
        shellAction(HWShell::ACT_FILE_OPEN)->setIcon(desktop->icon());
    }
    if(m_model->isDevice(idx))
    {
        auto dev = m_model->deviceForIndex(idx);
        if(dev)
        {
            if(dev->canEject())
                shellAction(HWShell::ACT_FILE_EJECT)->setText(tr("&Eject %1").arg(dev->name()));
        }
    }
}

void DesktopWindow::resetActionStrings()
{
    shellAction(HWShell::ACT_FILE_OPEN)->setText(tr("&Open"));
    shellAction(HWShell::ACT_FILE_OPEN)->setIcon(QIcon());
    shellAction(HWShell::ACT_FILE_EJECT)->setText(tr("&Eject"));
}

void DesktopWindow::disableFileActions()
{
    shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(false);
    shellAction(HWShell::ACT_FILE_ARCHIVE)->setEnabled(false);
    shellAction(HWShell::ACT_FILE_GET_INFO)->setEnabled(false);
    shellAction(HWShell::ACT_FILE_EJECT)->setEnabled(false);
    shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(false);
    shellAction(HWShell::ACT_FILE_TRASH)->setEnabled(false);
    shellAction(HWShell::ACT_EDIT_CUT)->setEnabled(false);
    shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(false);
    menu_Open_With->setEnabled(false);
}

void DesktopWindow::enableFileActions()
{
    bool enablefile = false;
    bool enabledevice = false;
    for(auto idx : m_view->selectionModel()->selectedIndexes())
    {
        if(m_model->isDevice(idx))
            enabledevice = true;

        auto fileinfo = m_model->fileInfo(idx);
        if(fileinfo == QFileInfo())
            continue;
        else
            enablefile = true;

        if(enablefile && enabledevice)
            break;
    }

    shellAction(HWShell::ACT_FILE_OPEN)->setEnabled(true);
    shellAction(HWShell::ACT_EDIT_COPY)->setEnabled(true);
    shellAction(HWShell::ACT_FILE_GET_INFO)->setEnabled(true);
    shellAction(HWShell::ACT_FILE_RENAME)->setEnabled(true);

    if(enablefile)
    {
        shellAction(HWShell::ACT_FILE_TRASH)->setEnabled(true);
        shellAction(HWShell::ACT_FILE_ARCHIVE)->setEnabled(true);
        if(m_view->selectionModel()->selectedIndexes().count() == 1)
            menu_Open_With->setEnabled(true);
    }

    if(enabledevice)
        shellAction(HWShell::ACT_FILE_EJECT)->setEnabled(true);
}

void DesktopWindow::viewActionsToggled(bool toggle)
{
    if(toggle)
        m_viewOptions->updateViewOptions(m_view);

    m_viewOptions->setVisible(toggle);
}

void DesktopWindow::viewOptionsChanged()
{
    auto icosize = m_viewOptions->iconSize();
    m_view->setIconSize(QSize(icosize,icosize));

    auto spacing = m_viewOptions->spacing();
    m_view->setSpacing(spacing);

    auto textsize = m_viewOptions->textsize();
    auto font = m_view->font();
    font.setPointSize(textsize);
    m_view->setFont(font);
    calculateGridSize();
}

void DesktopWindow::viewOptionsClosed()
{
    shellAction(HWShell::ACT_VIEW_OPTIONS)->setChecked(false);
    saveViewOptions();
}

void DesktopWindow::getInfoRequested()
{
    if(m_view->selectionModel()->selectedIndexes().count() == 1)
    {
        // show a dialog for our one file
        if(m_model->isDevice(m_view->selectionModel()->selectedIndexes().first()))
        {
            auto dev = m_model->deviceForIndex(m_view->selectionModel()->selectedIndexes().first());
            if(dev)
            {
                auto mountpoint = dev->mountpoint();
                if(!mountpoint.isEmpty())
                    LSCommonFunctions::instance()->showGetInfoDialog(QUrl::fromLocalFile(mountpoint), this);
                else
                {
                    // not mounted - get the /dev item
                    auto devpath = dev->devDevice();
                    LSCommonFunctions::instance()->showGetInfoDialog(QUrl::fromLocalFile(devpath), this);
                }
            }
            return;
        }

        auto url = QUrl(m_model->url(m_view->selectionModel()->selectedIndexes().first()));
        LSCommonFunctions::instance()->showGetInfoDialog(url, this);
    }
    else
    {
        // multiple selection info requested
    }
}

void DesktopWindow::canUndoChanged()
{
    bool canundo = LSCommonFunctions::instance()->undoStack()->canUndo();
    if(canundo)
    {
        shellAction(HWShell::ACT_EDIT_UNDO)->setEnabled(true);
        shellAction(HWShell::ACT_EDIT_UNDO)->setText(tr("&Undo %1")
                    .arg(LSCommonFunctions::instance()->undoStack()->undoText()));
    }
    else
    {
        shellAction(HWShell::ACT_EDIT_UNDO)->setEnabled(false);
        shellAction(HWShell::ACT_EDIT_UNDO)->setText(tr("&Undo"));
    }
}

void DesktopWindow::canRedoChanged()
{
    bool canredo = LSCommonFunctions::instance()->undoStack()->canRedo();
    if(canredo)
    {
        shellAction(HWShell::ACT_EDIT_REDO)->setEnabled(true);
        shellAction(HWShell::ACT_EDIT_REDO)->setText(tr("&Redo %1")
                  .arg(LSCommonFunctions::instance()->undoStack()->redoText()));
    }
    else
    {
        shellAction(HWShell::ACT_EDIT_REDO)->setEnabled(false);
        shellAction(HWShell::ACT_EDIT_REDO)->setText(tr("&Redo"));
    }
}

void DesktopWindow::show()
{
    QWidget::show();
    m_lswnd = LayerShellQt::Window::get(windowHandle());
    if(m_lswnd)
    {
        qDebug() << "setting layershell LayerBackground onshow";
        m_lswnd->setLayer(LayerShellQt::Window::LayerBackground);
        m_lswnd->setAnchors(LayerShellQt::Window::AnchorTop);
        m_lswnd->setSize(qApp->primaryScreen()->size());
        update();
    }

}

void DesktopWindow::loadViewOptions()
{
    m_view->setViewMode(QListView::IconMode);
    m_view->setFlow(QListView::TopToBottom);
    m_view->setUniformItemSizes(false);
    m_view->setWordWrap(true);

    QSettings settings("originull", "hollywood");
    // preserve our view states
    settings.beginGroup("ShellFM");
    settings.beginGroup("DesktopViewOptions");
    auto size = settings.value("IconSize", 32).toUInt();
    if(size < 16)
        size = 16;
    if(size > 128)
        size = 128;
    m_view->setIconSize(QSize(size,size));
    auto spacing = settings.value("IconSpacing", 8).toUInt();
    if(spacing < 2)
        spacing = 2;
    if(spacing > 100)
        spacing = 100;

    m_view->setSpacing(spacing);
    m_view->setMovement(QListView::Free);

    auto textsize = settings.value("TextSize", 11).toUInt();
    auto font = m_view->font();
    font.setPointSize(textsize);
    m_view->setFont(font);

    calculateGridSize();
}

void DesktopWindow::saveViewOptions()
{
    QSettings settings("originull", "hollywood");
    // preserve our view states
    settings.beginGroup("ShellFM");
    settings.beginGroup("DesktopViewOptions");
    settings.setValue("IconSize", m_view->iconSize().height());
    settings.setValue("IconSpacing", m_view->spacing());
    settings.setValue("TextSize", m_view->font().pointSize());
}

void DesktopWindow::calculateGridSize()
{
    auto ico_height = m_view->iconSize().height();
    auto font_height = m_view->font().pixelSize();
    auto grid_height = ico_height+font_height+10;
    auto grid_width = grid_height;
    Q_UNUSED(grid_width)
    //m_view->setGridSize(QSize(grid_height, grid_width));
}

void DesktopWindow::setupActions()
{
    FMApplication *myApp = FMApplication::instance();
    a_NewWindow = new QAction(this);
    a_NewWindow->setObjectName(QString::fromUtf8("FileNewWindow"));
    a_NewWindow->setShortcut(QKeySequence(QKeySequence::New));
    connect(a_NewWindow, SIGNAL(triggered()),
            myApp, SLOT(newFileWindow()));

    // filler action - we really rely on the one
    // provided by QToolbar in real (nondesktop) windows
    a_Toolbar = new QAction(this);
    a_Toolbar->setObjectName(QString::fromUtf8("ViewToolBar"));
    a_Toolbar->setCheckable(true);
    a_Toolbar->setDisabled(true);
    a_StatusBar = new QAction(this);
    a_StatusBar->setObjectName(QString::fromUtf8("ViewStatusBar"));
    a_StatusBar->setDisabled(true);

    a_Home = new QAction(this);
    a_Home->setObjectName(QString::fromUtf8("GoHome"));
    a_Home->setIcon(QIcon::fromTheme("user-home"));
    a_Home->setShortcut(QKeySequence("Ctrl+Shift+H"));
    connect(a_Home, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);
    a_Documents = new QAction(this);
    a_Documents->setObjectName(QString::fromUtf8("GoDocuments"));
    a_Documents->setIcon(QIcon::fromTheme("folder-documents"));
    a_Documents->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(a_Documents, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);
    a_Photos = new QAction(this);
    a_Photos->setObjectName(QString::fromUtf8("GoPhotos"));
    a_Photos->setIcon(QIcon::fromTheme("folder-pictures"));
    connect(a_Photos, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);


    a_Videos = new QAction(this);
    a_Videos->setObjectName(QString::fromUtf8("GoVideos"));
    a_Videos->setIcon(QIcon::fromTheme("folder-videos"));
    connect(a_Videos, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);

    a_Music = new QAction(this);
    a_Music->setObjectName(QString::fromUtf8("GoMusic"));
    a_Music->setIcon(QIcon::fromTheme("folder-music"));
    connect(a_Music, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);

    a_Desktop = new QAction(this);
    a_Desktop->setObjectName(QString::fromUtf8("GoDesktop"));
    a_Desktop->setIcon(QIcon::fromTheme("user-desktop"));
    connect(a_Desktop, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);

    a_Applications = new QAction(this);
    a_Applications->setObjectName(QString::fromUtf8("GoApplications"));
    a_Applications->setIcon(QIcon::fromTheme(QString::fromUtf8("folder-activities")));
    a_Applications->setShortcut(QKeySequence("Ctrl+Shift+A"));
    connect(a_Applications, &QAction::triggered,
            this, &DesktopWindow::goQuickAction);

    a_Network = new QAction(this);
    a_Network->setObjectName(QString::fromUtf8("GoNetwork"));
    a_Network->setIcon(QIcon::fromTheme(QString::fromUtf8("network-workgroup")));
    a_Network->setShortcut(QKeySequence("Ctrl+Shift+N"));
    a_Network->setEnabled(false);

    a_Connect_to_Server = new QAction(this);
    a_Connect_to_Server->setObjectName(QString::fromUtf8("ConnectServer"));
    a_Connect_to_Server->setShortcut(QKeySequence("Ctrl+K"));
    a_Connect_to_Server->setEnabled(false);
    // Help Menu
    action_Help = new QAction(this);
    action_Help->setObjectName(QString::fromUtf8("HelpSystem"));

    // Bookmark Menu
    a_Add_Bookmark = new QAction(this);
    a_Add_Bookmark->setObjectName(QString::fromUtf8("BookmarkAdd"));

    a_NewWindow->setText(tr("New &Window"));
    action_Help->setText(tr("Get System &Help"));
    a_Add_Bookmark->setText(tr("&Add Bookmark..."));
    a_Toolbar->setText(tr("Toolbar"));
    a_StatusBar->setText(tr("Status Bar"));

    //a_Clear_List->setText(tr("&Clear List"));
    a_Home->setText(tr("&Home Folder"));
    a_Documents->setText(tr("&Documents"));
    a_Photos->setText(tr("&Photos"));
    a_Videos->setText(tr("&Videos"));
    a_Music->setText(tr("&Music"));
    a_Desktop->setText(tr("Des&ktop"));
    a_Applications->setText(tr("&Applications"));
    a_Network->setText(tr("&Network"));
    a_Connect_to_Server->setText(tr("&Connect to Server..."));

    a_Home->setIconVisibleInMenu(true);
    a_Documents->setIconVisibleInMenu(true);
    a_Desktop->setIconVisibleInMenu(true);
    a_Music->setIconVisibleInMenu(true);
    a_Videos->setIconVisibleInMenu(true);
    a_Home->setIconVisibleInMenu(true);
    a_Applications->setIconVisibleInMenu(true);
    a_Network->setIconVisibleInMenu(true);
}

void DesktopWindow::setupMenuBar()
{
    /* We do the menu bar separate because it can be used
     * by both a browser window and the root desktop window
     * over dbusmenu */
    menu_File = new QMenu(m_menuBar);
    menu_Edit = new QMenu(m_menuBar);
    menu_View = new QMenu(m_menuBar);
    menu_Go = new QMenu(m_menuBar);
    menu_Bookmark = new QMenu(m_menuBar);
    menu_Window = new QMenu(m_menuBar);
    menu_Help = new QMenu(m_menuBar);
    menuRecent_Locations = new QMenu(menu_Go);
    menu_Open_With = new QMenu(menu_File);
    menu_File->setObjectName(QString::fromUtf8("FileMenu"));
    menu_Edit->setObjectName(QString::fromUtf8("EditMenu"));
    menu_View->setObjectName(QString::fromUtf8("ViewMeu"));
    menu_Go->setObjectName(QString::fromUtf8("GoMenu"));
    menuRecent_Locations->setObjectName(QString::fromUtf8("RecentLocationsMenu"));
    menu_Bookmark->setObjectName(QString::fromUtf8("BookmarksMenu"));
    menu_Window->setObjectName(QString::fromUtf8("WindowMenu"));
    menu_Help->setObjectName(QString::fromUtf8("HelpMenu"));
    menu_Open_With->setObjectName(QString::fromUtf8("OpenWithMenu"));

    menu_File->setTitle(tr("&File"));
    menu_Open_With->setTitle(tr("Op&en With"));
    menu_Edit->setTitle(tr("&Edit"));
    menu_View->setTitle(tr("&View"));
    menu_Go->setTitle(tr("&Go"));
    menuRecent_Locations->setTitle(tr("Recent &Locations"));
    menu_Bookmark->setTitle(tr("&Bookmarks"));
    menu_Window->setTitle(tr("&Window"));
    menu_Help->setTitle(tr("&Help"));

    m_menuBar->addAction(menu_File->menuAction());
    m_menuBar->addAction(menu_Edit->menuAction());
    m_menuBar->addAction(menu_View->menuAction());
    m_menuBar->addAction(menu_Go->menuAction());
    m_menuBar->addAction(menu_Bookmark->menuAction());
    m_menuBar->addAction(menu_Window->menuAction());
    m_menuBar->addAction(menu_Help->menuAction());
    menu_File->addAction(a_NewWindow);
    menu_File->addAction(shellAction(HWShell::ACT_FILE_NEW_TAB));
    shellAction(HWShell::ACT_FILE_NEW_TAB)->setEnabled(false);
    menu_File->addSeparator();
    auto menu = new QMenu(tr("Create &New"), this);
    menu_File->addMenu(menu);
    menu->addAction(shellAction(HWShell::ACT_FILE_NEW_FOLDER));
    connect(shellAction(HWShell::ACT_FILE_NEW_FOLDER), &QAction::triggered, this, &DesktopWindow::newFolder);
    menu->addAction(shellAction(HWShell::ACT_FILE_NEW_FILE));
    menu_File->addSeparator();
    menu_File->addAction(shellAction(HWShell::ACT_FILE_OPEN));
    connect(shellAction(HWShell::ACT_FILE_OPEN), &QAction::triggered, this, &DesktopWindow::openItems);
    menu_File->addMenu(menu_Open_With);
    menu_Open_With->addAction(shellAction(HWShell::ACT_FILE_OPEN_WITH));
    menu_File->addAction(shellAction(HWShell::ACT_FILE_GET_INFO));
    connect(shellAction(HWShell::ACT_FILE_GET_INFO), &QAction::triggered, this, &DesktopWindow::getInfoRequested);
    menu_File->addAction(shellAction(HWShell::ACT_FILE_RENAME));
    connect(shellAction(HWShell::ACT_FILE_RENAME), &QAction::triggered, this, &DesktopWindow::rename);
    menu_File->addAction(shellAction(HWShell::ACT_FILE_ARCHIVE));
    connect(shellAction(HWShell::ACT_FILE_ARCHIVE), &QAction::triggered, this, &DesktopWindow::archive);
    menu_File->addSeparator();
    menu_File->addAction(shellAction(HWShell::ACT_FILE_TRASH));
    connect(shellAction(HWShell::ACT_FILE_TRASH), &QAction::triggered, this, &DesktopWindow::trash);
    menu_File->addAction(shellAction(HWShell::ACT_FILE_EJECT));
    connect(shellAction(HWShell::ACT_FILE_EJECT), &QAction::triggered, this, &DesktopWindow::eject);
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_UNDO));
    connect(shellAction(HWShell::ACT_EDIT_UNDO), &QAction::triggered,
            LSCommonFunctions::instance()->undoStack(), &QUndoStack::undo);
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_REDO));
    connect(shellAction(HWShell::ACT_EDIT_REDO), &QAction::triggered,
            LSCommonFunctions::instance()->undoStack(), &QUndoStack::redo);
    shellAction(HWShell::ACT_EDIT_UNDO)->setEnabled(false);
    shellAction(HWShell::ACT_EDIT_REDO)->setEnabled(false);
    menu_Edit->addSeparator();
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_CUT));
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_COPY));
    connect(shellAction(HWShell::ACT_EDIT_COPY), &QAction::triggered,
            this, &DesktopWindow::copyItems);
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_PASTE));
    connect(shellAction(HWShell::ACT_EDIT_PASTE), &QAction::triggered,
            this, &DesktopWindow::paste);
    menu_Edit->addSeparator();
    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_SEL_ALL));
    connect(shellAction(HWShell::ACT_EDIT_SEL_ALL), &QAction::triggered, m_view, &QListView::selectAll);

    menu_Edit->addAction(shellAction(HWShell::ACT_EDIT_INV_SEL));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_ICONS));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_LIST));
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_COLUMNS));
    shellAction(HWShell::ACT_VIEW_ICONS)->setEnabled(false);
    shellAction(HWShell::ACT_VIEW_LIST)->setEnabled(false);
    shellAction(HWShell::ACT_VIEW_COLUMNS)->setEnabled(false);
    menu_View->addSeparator();
    menu_View->addAction(a_Toolbar);
    menu_View->addAction(a_StatusBar);
    menu_View->addSeparator();
    menu_View->addAction(shellAction(HWShell::ACT_VIEW_OPTIONS));
    connect(shellAction(HWShell::ACT_VIEW_OPTIONS), &QAction::toggled, this, &DesktopWindow::viewActionsToggled);
    menu_Go->addAction(shellAction(HWShell::ACT_GO_BACK));
    menu_Go->addAction(shellAction(HWShell::ACT_GO_FORWARD));
    shellAction(HWShell::ACT_GO_BACK)->setEnabled(false);
    shellAction(HWShell::ACT_GO_FORWARD)->setEnabled(false);
    menu_Go->addAction(shellAction(HWShell::ACT_GO_ENCLOSING_FOLDER));
    menu_Go->addSeparator();
    menu_Go->addAction(a_Home);
    menu_Go->addAction(a_Documents);
    menu_Go->addAction(a_Photos);
    menu_Go->addAction(a_Videos);
    menu_Go->addAction(a_Music);
    menu_Go->addAction(a_Desktop);
    menu_Go->addAction(a_Network);
    menu_Go->addAction(a_Applications);
    menu_Go->addSeparator();
    menu_Go->addAction(menuRecent_Locations->menuAction());
    menu_Go->addSeparator();
    menu_Go->addAction(a_Connect_to_Server);
    //menuRecent_Locations->addAction(a_Clear_List);
    menu_Bookmark->addAction(a_Add_Bookmark);
    menu_Help->addAction(action_Help);

    a_Add_Bookmark->setEnabled(false);
    connect(action_Help, &QAction::triggered, FMApplication::instance(), &FMApplication::openHelp);

    // handle the global window list
    connect(menu_Window, &QMenu::aboutToShow, [this]() {
        FMApplication *myApp = FMApplication::instance();
        myApp->setupWindowMenu(menu_Window);
    });
    connect(menu_Window, &QMenu::aboutToHide, [this]() {
        for(auto act : menu_Window->actions())
            act->deleteLater();
    });

    m_rightclick->addAction(shellAction(HWShell::ACT_EDIT_UNDO));
    m_rightclick->addAction(shellAction(HWShell::ACT_EDIT_REDO));
    m_rightclick->addSeparator();
    m_rightclick->addAction(shellAction(HWShell::ACT_FILE_NEW_FOLDER));
    m_rightclick->addSeparator();
    m_rightclick->addAction(shellAction(HWShell::ACT_EDIT_PASTE));
    //m_rightclick->addAction(tr("Paste &Symbolic Links"))->setDisabled(true);
    m_rightclick->addAction(shellAction(HWShell::ACT_EDIT_SEL_ALL));
    m_rightclick->addSeparator();
    auto wp = m_rightclick->addAction(tr("Change &Wallpaper..."));
    connect(wp, &QAction::triggered, FMApplication::instance(), &FMApplication::showWallpaperSettings);
    m_rotate = m_rightclick->addAction(tr("Rotate Wallpaper &Now"));
    connect(m_rotate, &QAction::triggered, FMApplication::instance(), &FMApplication::rotateWallpaper);
    m_rightclick->addSeparator();
    m_rightclick->addAction(shellAction(HWShell::ACT_VIEW_OPTIONS));
}

void DesktopWindow::desktopGeometryChanged(const QRect &geom)
{
    resize(geom.size());
}
