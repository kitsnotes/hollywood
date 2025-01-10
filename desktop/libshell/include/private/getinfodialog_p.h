// Hollywood Shell Library
// (C) 2024 Originull Software
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#include <QUrl>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QWidget>
#include <QLineEdit>
#include <sys/stat.h>
#include <QTreeView>

class LSMimeApplications;
class HWFileIconProvider;
class LSGetInfoDialog;
class LSGeneralInfoWidget;
class LSPermissionsAclWidget;
class LSDesktopEntryInfoWidget;
class LSApplicationInfoWidget;
class LSDesktopEntry;

class LSGetInfoDialogPrivate
{
    friend class LSGetInfoDialog;
    friend class LSGeneralInfoWidget;
    friend class LSPermissionsAclWidget;
    friend class LSDesktopEntryInfoWidget;
    friend class LSApplicationInfoWidget;

    LSGetInfoDialogPrivate(const QUrl &target, LSGetInfoDialog *parent);
    ~LSGetInfoDialogPrivate() = default;
    LSGetInfoDialog *d;
    bool isDesktop() const;
    QString targetFile() const;
    QFrame *makeLine(QWidget *parent);

    QString m_target;
    LSMimeApplications *m_mime = nullptr;
    HWFileIconProvider *m_icons = nullptr;
    bool m_init = true;
    ino_t m_inode;

    LSApplicationInfoWidget *m_app = nullptr;
    LSDesktopEntryInfoWidget *m_desktop_widget = nullptr;
    LSGeneralInfoWidget *m_info = nullptr;
    LSPermissionsAclWidget *m_perms = nullptr;

    LSDesktopEntry *m_desktop_entry = nullptr;
};
