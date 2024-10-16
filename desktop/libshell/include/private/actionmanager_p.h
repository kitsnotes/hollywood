// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QAction>
#include <QActionGroup>
#include <QMenu>
class LSActionManagerPrivate
{
private:
    friend class LSActionManager;
    QMenu *m_openwith;
    QMenu *m_newmenu;
    QAction *a_NewTab;
    QAction *a_NewFolder;
    QAction *actionNew_File;
    QAction *actionGet_Information;
    QAction *a_Rename;
    QAction *actionAdd_to_Archive;
    QAction *actionMove_to_Trash;
    QAction *a_Eject_Volume;
    QAction *a_Open;
    QAction *a_OpenWith;
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

    QAction *a_sortNone;
    QAction *a_sortName;
    QAction *a_sortSize;
    QAction *a_sortKind;
    QAction *a_sortModified;
    QAction *a_sortOwner;
    QAction *a_sortGroup;
    QAction *a_sortComment;

    QAction *a_sortAsc;
    QAction *a_sortDesc;
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

    QActionGroup *m_views;
    QActionGroup *m_sortcol;
    QActionGroup *m_sortorder;
};
