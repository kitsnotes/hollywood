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

class LSGetInfoDialogPrivate
{
    friend class LSGetInfoDialog;
    friend class LSGeneralInfoWidget;
    friend class LSPermissionsAclWidget;
    friend class LSDesktopEntryInfoWidget;

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

    LSDesktopEntryInfoWidget *m_desktop = nullptr;
    LSGeneralInfoWidget *m_info = nullptr;
    LSPermissionsAclWidget *m_perms = nullptr;
};
