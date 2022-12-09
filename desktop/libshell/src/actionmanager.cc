#include "actionmanager.h"
#include "private/actionmanager_p.h"

LSActionManager::LSActionManager(QObject *parent)
    : QObject(parent)
    , d(new LSActionManagerPrivate)
{
    setupActions();
}

QAction* LSActionManager::shellAction(HWShell::ShellActions action)
{
    switch(action)
    {
    case HWShell::ACT_FILE_NEW_TAB:
        return d->a_NewTab;
    case HWShell::ACT_FILE_NEW_FOLDER:
        return d->a_NewFolder;
    case HWShell::ACT_FILE_NEW_FILE:
        return d->actionNew_File;
    case HWShell::ACT_FILE_GET_INFO:
        return d->actionGet_Information;
    case HWShell::ACT_FILE_RENAME:
        return d->a_Rename;
    case HWShell::ACT_FILE_ARCHIVE:
        return d->actionAdd_to_Archive;
    case HWShell::ACT_FILE_TRASH:
        return d->actionMove_to_Trash;
    case HWShell::ACT_FILE_OPEN_WITH:
        return d->a_OpenWith;
    case HWShell::ACT_EDIT_UNDO:
        return d->a_Undo;
    case HWShell::ACT_EDIT_REDO:
        return d->a_Redo;
    case HWShell::ACT_EDIT_CUT:
        return d->a_Cut;
    case HWShell::ACT_EDIT_COPY:
        return d->a_Copy;
    case HWShell::ACT_EDIT_PASTE:
        return d->a_Paste;
    case HWShell::ACT_EDIT_SEL_ALL:
        return d->actionSelect_All;
    case HWShell::ACT_EDIT_INV_SEL:
        return d->a_InvertSelection;
    case HWShell::ACT_VIEW_ICONS:
        return d->a_Icons;
    case HWShell::ACT_VIEW_LIST:
        return d->a_List;
    case HWShell::ACT_VIEW_COLUMNS:
        return d->a_Columns;
    case HWShell::ACT_VIEW_SORT_NONE:
        return d->a_sortNone;
    case HWShell::ACT_VIEW_SORT_NAME:
        return d->a_sortName;
    case HWShell::ACT_VIEW_SORT_SIZE:
        return d->a_sortSize;
    case HWShell::ACT_VIEW_SORT_KIND:
        return d->a_sortKind;
    case HWShell::ACT_VIEW_SORT_MODIFIED:
        return d->a_sortModified;
    case HWShell::ACT_VIEW_SORT_OWNER:
        return d->a_sortOwner;
    case HWShell::ACT_VIEW_SORT_GROUP:
        return d->a_sortGroup;
    case HWShell::ACT_VIEW_SORT_COMMENT:
        return d->a_sortComment;
    case HWShell::ACT_VIEW_SORT_ASC:
        return d->a_sortAsc;
    case HWShell::ACT_VIEW_SORT_DESC:
        return d->a_sortDesc;
    case HWShell::ACT_VIEW_OPTIONS:
        return d->a_ViewOptions;
    case HWShell::ACT_GO_BACK:
        return d->a_Back;
    case HWShell::ACT_GO_FORWARD:
        return d->a_Forward;
    case HWShell::ACT_GO_ENCLOSING_FOLDER:
        return d->a_Enclosing_Folder;
    default:
        return nullptr;
    }
}
void LSActionManager::setupActions()
{
    d->a_NewTab = new QAction(this);
    d->a_NewTab->setObjectName(QString::fromUtf8("FileNewTab"));
    d->a_NewTab->setShortcut(QKeySequence(QKeySequence::AddTab));

    d->a_NewFolder = new QAction(this);
    d->a_NewFolder->setObjectName(QString::fromUtf8("FileNewFolder"));

    d->actionNew_File = new QAction(this);
    d->actionNew_File->setObjectName(QString::fromUtf8("FileNewFile"));

    d->actionGet_Information = new QAction(this);
    d->actionGet_Information->setObjectName(QString::fromUtf8("FileGetInfo"));
    d->actionGet_Information->setShortcut(QKeySequence("Ctrl+I"));

    d->a_Rename = new QAction(this);
    d->a_Rename->setObjectName(QString::fromUtf8("FileRename"));

    d->actionAdd_to_Archive = new QAction(this);
    d->actionAdd_to_Archive->setObjectName(QString::fromUtf8("FileArchive"));

    d->actionMove_to_Trash = new QAction(this);
    d->actionMove_to_Trash->setObjectName(QString::fromUtf8("FileTrash"));
    d->actionMove_to_Trash->setShortcut(QKeySequence("Ctrl+Delete"));

    d->a_Eject_Volume = new QAction(this);
    d->a_Eject_Volume->setObjectName(QString::fromUtf8("FileUnmount"));

    d->a_Open = new QAction(this);
    d->a_Open->setObjectName(QString::fromUtf8("FileOpen"));
    d->a_Open->setShortcut(QKeySequence(QKeySequence::Open));

    d->a_OpenWith = new QAction(this);
    d->a_OpenWith->setObjectName(QString::fromUtf8("FileOpenWith"));

    d->a_Undo = new QAction(this);
    d->a_Undo->setObjectName(QString::fromUtf8("EditUndo"));
    d->a_Undo->setShortcut(QKeySequence(QKeySequence::Undo));
    d->a_Undo->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-undo")));
    d->a_Undo->setIconVisibleInMenu(false);
    d->a_Redo = new QAction(this);
    d->a_Redo->setObjectName(QString::fromUtf8("EditRedo"));
    d->a_Redo->setShortcut(QKeySequence(QKeySequence::Redo));
    d->a_Redo->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-redo")));
    d->a_Redo->setIconVisibleInMenu(false);
    d->a_Cut = new QAction(this);
    d->a_Cut->setObjectName(QString::fromUtf8("EditCut"));
    d->a_Cut->setShortcut(QKeySequence(QKeySequence::Cut));
    d->a_Cut->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-cut")));
    d->a_Cut->setIconVisibleInMenu(false);
    d->a_Copy = new QAction(this);
    d->a_Copy->setObjectName(QString::fromUtf8("EditCopy"));
    d->a_Copy->setShortcut(QKeySequence(QKeySequence::Copy));
    d->a_Copy->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-copy")));
    d->a_Copy->setIconVisibleInMenu(false);
    d->a_Paste = new QAction(this);
    d->a_Paste->setObjectName(QString::fromUtf8("EditPaste"));
    d->a_Paste->setShortcut(QKeySequence(QKeySequence::Paste));
    d->a_Paste->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-paste")));
    d->a_Paste->setIconVisibleInMenu(false);
    d->actionSelect_All = new QAction(this);
    d->actionSelect_All->setObjectName(QString::fromUtf8("EditSelectAll"));
    d->actionSelect_All->setShortcut(QKeySequence(QKeySequence::SelectAll));
    d->actionSelect_All->setIconVisibleInMenu(false);
    d->a_InvertSelection = new QAction(this);
    d->a_InvertSelection->setObjectName(QString::fromUtf8("EditInvertSelection"));


    // View Menu
    QActionGroup *grpViewStyles = new QActionGroup(this);
    QActionGroup *grpViewSortCol = new QActionGroup(this);
    QActionGroup *grpViewSortDir = new QActionGroup(this);
    d->a_Icons = new QAction(this);
    d->a_Icons->setObjectName(QString::fromUtf8("ViewIcons"));
    d->a_Icons->setCheckable(true);
    d->a_Icons->setShortcut(QKeySequence("Ctrl+1"));
    d->a_List = new QAction(this);
    d->a_List->setObjectName(QString::fromUtf8("ViewList"));
    d->a_List->setCheckable(true);
    d->a_List->setShortcut(QKeySequence("Ctrl+2"));
    d->a_Columns = new QAction(this);
    d->a_Columns->setObjectName(QString::fromUtf8("ViewColumns"));
    d->a_Columns->setCheckable(true);
    d->a_Columns->setShortcut(QKeySequence("Ctrl+3"));
    grpViewStyles->addAction(d->a_Icons);
    grpViewStyles->addAction(d->a_List);
    grpViewStyles->addAction(d->a_Columns);

    d->a_sortNone = new QAction(this);
    d->a_sortNone->setObjectName(QString::fromUtf8("SortNone"));
    d->a_sortNone->setCheckable(true);
    d->a_sortNone->setShortcut(QKeySequence("Ctrl+Alt+Shift+0"));
    d->a_sortNone->setText(tr("None"));

    d->a_sortName = new QAction(this);
    d->a_sortName->setObjectName(QString::fromUtf8("SortByName"));
    d->a_sortName->setCheckable(true);
    d->a_sortName->setShortcut(QKeySequence("Ctrl+Alt+Shift+1"));
    d->a_sortName->setText(tr("&Name"));
    d->a_sortSize = new QAction(this);
    d->a_sortSize->setObjectName(QString::fromUtf8("SortBySize"));
    d->a_sortSize->setCheckable(true);
    d->a_sortSize->setShortcut(QKeySequence("Ctrl+Alt+Shift+2"));
    d->a_sortSize->setText(tr("&Size"));
    d->a_sortKind = new QAction(this);
    d->a_sortKind->setObjectName(QString::fromUtf8("SortByKind"));
    d->a_sortKind->setCheckable(true);
    d->a_sortKind->setShortcut(QKeySequence("Ctrl+Alt+Shift+3"));
    d->a_sortKind->setText(tr("&Kind"));

    d->a_sortModified = new QAction(this);
    d->a_sortModified->setObjectName(QString::fromUtf8("SortByName"));
    d->a_sortModified->setCheckable(true);
    d->a_sortModified->setShortcut(QKeySequence("Ctrl+Alt+Shift+4"));
    d->a_sortModified->setText(tr("&Modification Date"));
    d->a_sortOwner = new QAction(this);
    d->a_sortOwner->setObjectName(QString::fromUtf8("SortByOwner"));
    d->a_sortOwner->setCheckable(true);
    d->a_sortOwner->setShortcut(QKeySequence("Ctrl+Alt+Shift+5"));
    d->a_sortOwner->setText(tr("&Owner"));
    d->a_sortGroup = new QAction(this);
    d->a_sortGroup->setObjectName(QString::fromUtf8("SortByGroup"));
    d->a_sortGroup->setCheckable(true);
    d->a_sortGroup->setShortcut(QKeySequence("Ctrl+Alt+Shift+6"));
    d->a_sortGroup->setText(tr("&Group"));
    d->a_sortComment = new QAction(this);
    d->a_sortComment->setObjectName(QString::fromUtf8("SortByComment"));
    d->a_sortComment->setCheckable(true);
    d->a_sortComment->setShortcut(QKeySequence("Ctrl+Alt+Shift+7"));
    d->a_sortComment->setText(tr("&Comment"));

    grpViewSortCol->addAction(d->a_sortNone);
    grpViewSortCol->addAction(d->a_sortName);
    grpViewSortCol->addAction(d->a_sortSize);
    grpViewSortCol->addAction(d->a_sortKind);
    grpViewSortCol->addAction(d->a_sortModified);
    grpViewSortCol->addAction(d->a_sortOwner);
    grpViewSortCol->addAction(d->a_sortGroup);
    grpViewSortCol->addAction(d->a_sortComment);

    d->a_sortAsc = new QAction(this);
    d->a_sortAsc->setObjectName(QString::fromUtf8("SortAsc"));
    d->a_sortAsc->setCheckable(true);
    //d->a_sortAsc->setShortcut(QKeySequence("Ctrl+Alt+Shift+7"));
    d->a_sortAsc->setText(tr("&Ascending Order"));
    d->a_sortDesc = new QAction(this);
    d->a_sortDesc->setObjectName(QString::fromUtf8("SortDesc"));
    d->a_sortDesc->setCheckable(true);
    //d->a_sortDesc->setShortcut(QKeySequence("Ctrl+Alt+Shift+7"));
    d->a_sortDesc->setText(tr("&Descending Order"));

    grpViewSortDir->addAction(d->a_sortAsc);
    grpViewSortDir->addAction(d->a_sortDesc);

    d->a_ViewOptions = new QAction(this);
    d->a_ViewOptions->setObjectName(QString::fromUtf8("ShowViewOptions"));
    d->a_ViewOptions->setCheckable(true);
    d->a_ViewOptions->setShortcut(QKeySequence::fromString("Ctrl+J"));
    d->a_ViewOptions->setText(tr("&View Options"));
    connect(d->a_ViewOptions, SIGNAL(toggled(bool)),
            this, SLOT(toggleViewOptions(bool)));

    // Go Menu Options
    d->a_Back = new QAction(this);
    d->a_Back->setObjectName(QString::fromUtf8("GoBack"));
    d->a_Back->setShortcut(QKeySequence(QKeySequence::Back));
    d->a_Forward = new QAction(this);
    d->a_Forward->setObjectName(QString::fromUtf8("GoForward"));
    d->a_Forward->setShortcut(QKeySequence(QKeySequence::Forward));
    d->a_Enclosing_Folder = new QAction(this);
    d->a_Enclosing_Folder->setObjectName(QString::fromUtf8("GoEnclosingFolder"));
    /* d->a_Clear_List = new QAction(this);
    d->a_Clear_List->setObjectName(QString::fromUtf8("ClearRecentList"));
    d->a_Home = new QAction(this);
    d->a_Home->setObjectName(QString::fromUtf8("GoHome"));
    d->a_Home->setIcon(QIcon::fromTheme("user-home"));
    d->a_Home->setShortcut(QKeySequence("Ctrl+Shift+H"));
    connect(d->a_Home, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    d->a_Documents = new QAction(this);
    d->a_Documents->setObjectName(QString::fromUtf8("GoDocuments"));
    d->a_Documents->setIcon(QIcon::fromTheme("folder-documents"));
    d->a_Documents->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(d->a_Documents, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    d->a_Photos = new QAction(this);
    d->d->a_Photos->setObjectName(QString::fromUtf8("GoPhotos"));
    d->a_Photos->setIcon(QIcon::fromTheme("folder-pictures"));
    connect(d->a_Photos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));

    d->a_Videos = new QAction(this);
    d->a_Videos->setObjectName(QString::fromUtf8("GoVideos"));
    d->a_Videos->setIcon(QIcon::fromTheme("folder-videos"));
    connect(d->a_Videos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    d->a_Music = new QAction(this);
    d->a_Music->setObjectName(QString::fromUtf8("GoMusic"));
    d->a_Music->setIcon(QIcon::fromTheme("folder-music"));
    connect(d->a_Music, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    d->a_Desktop = new QAction(this);
    d->a_Desktop->setObjectName(QString::fromUtf8("GoDesktop"));
    d->a_Desktop->setIcon(QIcon::fromTheme("user-desktop"));
    connect(d->a_Desktop, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    d->a_Applications = new QAction(this);
    d->a_Applications->setObjectName(QString::fromUtf8("GoApplications"));
    d->a_Applications->setEnabled(false);
    d->a_Applications->setShortcut(QKeySequence("Ctrl+Shift+A"));
    d->a_Network = new QAction(this);
    d->a_Network->setObjectName(QString::fromUtf8("GoNetwork"));
    d->a_Network->setIcon(QIcon::fromTheme(QString::fromUtf8("network-workgroup")));
    d->a_Network->setShortcut(QKeySequence("Ctrl+Shift+N"));
    d->a_Network->setEnabled(false);

    d->a_Connect_to_Server = new QAction(this);
    d->a_Connect_to_Server->setObjectName(QString::fromUtf8("ConnectServer"));
    d->a_Connect_to_Server->setShortcut(QKeySequence("Ctrl+K")); */

    d->a_NewTab->setText(tr("New &Tab"));
    d->a_NewFolder->setText(tr("New &Folder"));
    d->actionNew_File->setText(tr("New File"));
    d->actionGet_Information->setText(tr("Get &Information"));
    d->a_Rename->setText(tr("&Rename"));
    d->actionAdd_to_Archive->setText(tr("Add to Archive"));
    d->actionMove_to_Trash->setText(tr("Move to &Trash"));
    d->a_Eject_Volume->setText(tr("&Eject Volume"));
    d->a_Open->setText(tr("&Open"));
    d->a_OpenWith->setText(tr("Op&en With"));
    d->a_Undo->setText(tr("&Undo"));
    d->a_Redo->setText(tr("&Redo"));
    d->a_Cut->setText(tr("&Cut"));
    d->a_Copy->setText(tr("C&opy"));
    d->a_Paste->setText(tr("&Paste"));
    d->actionSelect_All->setText(tr("Select &All"));
    d->a_InvertSelection->setText(tr("&Invert Selection"));

    d->a_Icons->setText(tr("as Icons"));
    d->a_List->setText(tr("as List"));
    d->a_Columns->setText(tr("as Columns"));

    d->a_Back->setText(tr("&Back"));
    d->a_Back->setIcon(QIcon::fromTheme("go-previous"));
    d->a_Forward->setText(tr("&Forward"));
    d->a_Forward->setIcon(QIcon::fromTheme("go-next"));

    d->a_Enclosing_Folder->setText(tr("&Enclosing Folder"));
    d->a_Enclosing_Folder->setIcon(QIcon::fromTheme("go-up"));

    /*d->a_Clear_List->setText(tr("&Clear List"));
    d->a_Home->setText(tr("&Home Folder"));
    d->a_Documents->setText(tr("&Documents"));
    d->a_Photos->setText(tr("&Photos"));
    d->a_Videos->setText(tr("&Videos"));
    d->a_Music->setText(tr("&Music"));
    d->a_Desktop->setText(tr("Des&ktop"));
    d->a_Applications->setText(tr("&Applications"));
    d->a_Network->setText(tr("&Network"));
    d->a_Connect_to_Server->setText(tr("&Connect to Server...")); */
}
