#ifndef LSACTIONMANAGER_H
#define LSACTIONMANAGER_H

#include "libshell_int.h"

class LSActionManager : public QObject
{
    Q_OBJECT
public:
    explicit LSActionManager(QObject *parent = nullptr);
    QAction* shellAction(ArionShell::ShellActions action);
signals:
private:
    void setupActions();
private:
    QAction *a_NewTab;
    QAction *a_NewFolder;
    QAction *actionNew_File;
    QAction *actionGet_Information;
    QAction *a_Rename;
    QAction *actionAdd_to_Archive;
    QAction *actionMove_to_Trash;
    QAction *a_Eject_Volume;
    QAction *a_Open;
    QAction *actionOp_en_With;
    QAction *a_Undo;
    QAction *a_Redo;
    QAction *a_Cut;
    QAction *a_Copy;
    QAction *a_Paste;
    QAction *actionSelect_All;
    QAction *a_InvertSelection;

    QAction *a_Icons;
    QAction *a_List;
    QAction *a_Columns;
    QAction *a_ViewOptions;

    QAction *a_Back;
    QAction *a_Forward;
    QAction *a_Enclosing_Folder;

    QAction *a_Home;
    QAction *a_Documents;
    QAction *a_Photos;
    QAction *a_Videos;
    QAction *a_Music;
    QAction *a_Desktop;
    QAction *a_Applications;
    QAction *a_Connect_to_Server;
    QAction *a_Network;
};

#endif // LSACTIONMANAGER_H
