#include "actionmanager.h"

LSActionManager::LSActionManager(QObject *parent)
    : QObject(parent)
{
    setupActions();
}

QAction* LSActionManager::shellAction(ArionShell::ShellActions action)
{
    switch(action)
    {
    case ArionShell::ACT_FILE_NEW_TAB:
        return a_NewTab;
    case ArionShell::ACT_FILE_NEW_FOLDER:
        return a_NewFolder;
    case ArionShell::ACT_FILE_NEW_FILE:
        return actionNew_File;
    case ArionShell::ACT_FILE_GET_INFO:
        return actionGet_Information;
    case ArionShell::ACT_FILE_RENAME:
        return a_Rename;
    case ArionShell::ACT_FILE_ARCHIVE:
        return actionAdd_to_Archive;
    case ArionShell::ACT_FILE_TRASH:
        return actionMove_to_Trash;
    case ArionShell::ACT_FILE_OPEN_WITH:
        return actionOp_en_With;
    case ArionShell::ACT_EDIT_UNDO:
        return a_Undo;
    case ArionShell::ACT_EDIT_REDO:
        return a_Redo;
    case ArionShell::ACT_EDIT_CUT:
        return a_Cut;
    case ArionShell::ACT_EDIT_COPY:
        return a_Copy;
    case ArionShell::ACT_EDIT_PASTE:
        return a_Paste;
    case ArionShell::ACT_EDIT_SEL_ALL:
        return actionSelect_All;
    case ArionShell::ACT_EDIT_INV_SEL:
        return a_InvertSelection;
    case ArionShell::ACT_VIEW_ICONS:
        return a_Icons;
    case ArionShell::ACT_VIEW_LIST:
        return a_List;
    case ArionShell::ACT_VIEW_COLUMNS:
        return a_Columns;
    case ArionShell::ACT_VIEW_OPTIONS:
        return a_ViewOptions;
    case ArionShell::ACT_GO_BACK:
        return a_Back;
    case ArionShell::ACT_GO_FORWARD:
        return a_Forward;
    case ArionShell::ACT_GO_ENCLOSING_FOLDER:
        return a_Enclosing_Folder;
    default:
        return nullptr;
    }
}
void LSActionManager::setupActions()
{
    a_NewTab = new QAction(this);
    a_NewTab->setObjectName(QString::fromUtf8("FileNewTab"));
    a_NewTab->setShortcut(QKeySequence(QKeySequence::AddTab));

    a_NewFolder = new QAction(this);
    a_NewFolder->setObjectName(QString::fromUtf8("FileNewFolder"));

    actionNew_File = new QAction(this);
    actionNew_File->setObjectName(QString::fromUtf8("FileNewFile"));

    actionGet_Information = new QAction(this);
    actionGet_Information->setObjectName(QString::fromUtf8("FileGetInfo"));
    actionGet_Information->setShortcut(QKeySequence("Ctrl+I"));

    a_Rename = new QAction(this);
    a_Rename->setObjectName(QString::fromUtf8("FileRename"));

    actionAdd_to_Archive = new QAction(this);
    actionAdd_to_Archive->setObjectName(QString::fromUtf8("FileArchive"));

    actionMove_to_Trash = new QAction(this);
    actionMove_to_Trash->setObjectName(QString::fromUtf8("FileTrash"));
    actionMove_to_Trash->setShortcut(QKeySequence("Ctrl+Delete"));

    a_Eject_Volume = new QAction(this);
    a_Eject_Volume->setObjectName(QString::fromUtf8("FileUnmount"));

    a_Open = new QAction(this);
    a_Open->setObjectName(QString::fromUtf8("FileOpen"));
    a_Open->setShortcut(QKeySequence(QKeySequence::Open));

    actionOp_en_With = new QAction(this);
    actionOp_en_With->setObjectName(QString::fromUtf8("FileOpenWith"));

    a_Undo = new QAction(this);
    a_Undo->setObjectName(QString::fromUtf8("EditUndo"));
    a_Undo->setShortcut(QKeySequence(QKeySequence::Undo));
    a_Undo->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-undo")));
    a_Redo = new QAction(this);
    a_Redo->setObjectName(QString::fromUtf8("EditRedo"));
    a_Redo->setShortcut(QKeySequence(QKeySequence::Redo));
    a_Redo->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-redo")));
    a_Cut = new QAction(this);
    a_Cut->setObjectName(QString::fromUtf8("EditCut"));
    a_Cut->setShortcut(QKeySequence(QKeySequence::Cut));
    a_Cut->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-cut")));
    a_Copy = new QAction(this);
    a_Copy->setObjectName(QString::fromUtf8("EditCopy"));
    a_Copy->setShortcut(QKeySequence(QKeySequence::Copy));
    a_Copy->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-copy")));
    a_Paste = new QAction(this);
    a_Paste->setObjectName(QString::fromUtf8("EditPaste"));
    a_Paste->setShortcut(QKeySequence(QKeySequence::Paste));
    a_Paste->setIcon(QIcon::fromTheme(QString::fromUtf8("edit-paste")));
    actionSelect_All = new QAction(this);
    actionSelect_All->setObjectName(QString::fromUtf8("EditSelectAll"));
    actionSelect_All->setShortcut(QKeySequence(QKeySequence::SelectAll));
    a_InvertSelection = new QAction(this);
    a_InvertSelection->setObjectName(QString::fromUtf8("EditInvertSelection"));


    // View Menu
    QActionGroup *grpViewStyles = new QActionGroup(this);
    a_Icons = new QAction(this);
    a_Icons->setObjectName(QString::fromUtf8("ViewIcons"));
    a_Icons->setCheckable(true);
    a_Icons->setShortcut(QKeySequence("Ctrl+1"));
    a_List = new QAction(this);
    a_List->setObjectName(QString::fromUtf8("ViewList"));
    a_List->setCheckable(true);
    a_List->setShortcut(QKeySequence("Ctrl+2"));
    a_Columns = new QAction(this);
    a_Columns->setObjectName(QString::fromUtf8("ViewColumns"));
    a_Columns->setCheckable(true);
    a_Columns->setShortcut(QKeySequence("Ctrl+3"));
    grpViewStyles->addAction(a_Icons);
    grpViewStyles->addAction(a_List);
    grpViewStyles->addAction(a_Columns);

    a_ViewOptions = new QAction(this);
    a_ViewOptions->setObjectName(QString::fromUtf8("ShowViewOptions"));
    a_ViewOptions->setCheckable(true);
    a_ViewOptions->setShortcut(QKeySequence::fromString("Ctrl+J"));
    a_ViewOptions->setText(tr("&View Options"));
    connect(a_ViewOptions, SIGNAL(toggled(bool)),
            this, SLOT(toggleViewOptions(bool)));

    // Go Menu Options
    a_Back = new QAction(this);
    a_Back->setObjectName(QString::fromUtf8("GoBack"));
    a_Back->setShortcut(QKeySequence(QKeySequence::Back));
    a_Forward = new QAction(this);
    a_Forward->setObjectName(QString::fromUtf8("GoForward"));
    a_Forward->setShortcut(QKeySequence(QKeySequence::Forward));
    a_Enclosing_Folder = new QAction(this);
    a_Enclosing_Folder->setObjectName(QString::fromUtf8("GoEnclosingFolder"));
    /* a_Clear_List = new QAction(this);
    a_Clear_List->setObjectName(QString::fromUtf8("ClearRecentList"));
    a_Home = new QAction(this);
    a_Home->setObjectName(QString::fromUtf8("GoHome"));
    a_Home->setIcon(QIcon::fromTheme("user-home"));
    a_Home->setShortcut(QKeySequence("Ctrl+Shift+H"));
    connect(a_Home, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Documents = new QAction(this);
    a_Documents->setObjectName(QString::fromUtf8("GoDocuments"));
    a_Documents->setIcon(QIcon::fromTheme("folder-documents"));
    a_Documents->setShortcut(QKeySequence("Ctrl+Shift+D"));
    connect(a_Documents, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Photos = new QAction(this);
    a_Photos->setObjectName(QString::fromUtf8("GoPhotos"));
    a_Photos->setIcon(QIcon::fromTheme("folder-pictures"));
    connect(a_Photos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));

    a_Videos = new QAction(this);
    a_Videos->setObjectName(QString::fromUtf8("GoVideos"));
    a_Videos->setIcon(QIcon::fromTheme("folder-videos"));
    connect(a_Videos, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Music = new QAction(this);
    a_Music->setObjectName(QString::fromUtf8("GoMusic"));
    a_Music->setIcon(QIcon::fromTheme("folder-music"));
    connect(a_Music, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Desktop = new QAction(this);
    a_Desktop->setObjectName(QString::fromUtf8("GoDesktop"));
    a_Desktop->setIcon(QIcon::fromTheme("user-desktop"));
    connect(a_Desktop, SIGNAL(triggered()),
            this, SLOT(goQuickAction()));
    a_Applications = new QAction(this);
    a_Applications->setObjectName(QString::fromUtf8("GoApplications"));
    a_Applications->setEnabled(false);
    a_Applications->setShortcut(QKeySequence("Ctrl+Shift+A"));
    a_Network = new QAction(this);
    a_Network->setObjectName(QString::fromUtf8("GoNetwork"));
    a_Network->setIcon(QIcon::fromTheme(QString::fromUtf8("network-workgroup")));
    a_Network->setShortcut(QKeySequence("Ctrl+Shift+N"));
    a_Network->setEnabled(false);

    a_Connect_to_Server = new QAction(this);
    a_Connect_to_Server->setObjectName(QString::fromUtf8("ConnectServer"));
    a_Connect_to_Server->setShortcut(QKeySequence("Ctrl+K")); */

    a_NewTab->setText(tr("New &Tab"));
    a_NewFolder->setText(tr("New &Folder"));
    actionNew_File->setText(tr("New File"));
    actionGet_Information->setText(tr("Get &Information"));
    a_Rename->setText(tr("&Rename"));
    actionAdd_to_Archive->setText(tr("Add to Archive"));
    actionMove_to_Trash->setText(tr("Move to &Trash"));
    a_Eject_Volume->setText(tr("&Eject Volume"));
    a_Open->setText(tr("&Open"));
    actionOp_en_With->setText(tr("Op&en With"));
    a_Undo->setText(tr("&Undo"));
    a_Redo->setText(tr("&Redo"));
    a_Cut->setText(tr("&Cut"));
    a_Copy->setText(tr("C&opy"));
    a_Paste->setText(tr("&Paste"));
    actionSelect_All->setText(tr("Select &All"));
    a_InvertSelection->setText(tr("&Invert Selection"));

    a_Icons->setText(tr("as Icons"));
    a_List->setText(tr("as List"));
    a_Columns->setText(tr("as Columns"));

    a_Back->setText(tr("&Back"));
    a_Back->setIcon(QIcon::fromTheme("go-previous"));
    a_Forward->setText(tr("&Forward"));
    a_Forward->setIcon(QIcon::fromTheme("go-next"));

    a_Enclosing_Folder->setText(tr("&Enclosing Folder"));
    a_Enclosing_Folder->setIcon(QIcon::fromTheme("go-up"));

    /*a_Clear_List->setText(tr("&Clear List"));
    a_Home->setText(tr("&Home Folder"));
    a_Documents->setText(tr("&Documents"));
    a_Photos->setText(tr("&Photos"));
    a_Videos->setText(tr("&Videos"));
    a_Music->setText(tr("&Music"));
    a_Desktop->setText(tr("Des&ktop"));
    a_Applications->setText(tr("&Applications"));
    a_Network->setText(tr("&Network"));
    a_Connect_to_Server->setText(tr("&Connect to Server...")); */
}
